#version 300 es

precision mediump float;

struct Camera {
    vec3 pos;
    vec3 target;
    float fov;
    vec3 up;
};

struct Sphere {
    vec3 center;
    vec2 r;
};

struct Plane {
    vec3 origin;
    vec3 normal;
};

uniform sampler2D u_accTexture;
uniform sampler2D u_brdfLUT;
uniform float u_textureWeight;
uniform float u_numSamples;
uniform vec2 u_resolution;
uniform Camera u_camera;
uniform Sphere u_prismSpheres[{{ numPrismSpheres }}];
uniform Plane u_prismPlanes[{{ numPrismPlanes }}];
uniform Sphere u_inversionSphere;
uniform Sphere u_sphairahedralSpheres[{{ numSphairahedronSpheres }}];
uniform Sphere u_seedSpheres[{{ numSeedSpheres }}];
uniform Plane u_dividePlanes[{{ numDividePlanes }}];
uniform Sphere u_convexSpheres[{{ numDividePlanes }}];
//uniform Plane u_boundingPlanes[{{ numBoundingPlanes }}];
uniform float u_fudgeFactor;
uniform int u_maxIterations;
{% if numExcavationSpheres > 0 %}
uniform Sphere u_excavationPrismSpheres[{{ numExcavationSpheres }}];
uniform Sphere u_excavationSpheres[{{ numExcavationSpheres }}];
{% endif %}
{% if shaderType == SHADER_TYPE_LIMITSET %}
uniform float u_marchingThreshold;
uniform float u_colorWeight;
uniform bool u_displayPrism;
{% endif %}
uniform vec3 u_lightDirection;
uniform vec2 u_metallicRoughness;
uniform bool u_displaySphairahedralSphere;
uniform bool u_displayConvexSphere;
uniform bool u_displayInversionSphere;
uniform bool u_displayBoundingSphere;
uniform bool u_castShadow;
uniform float u_boundingPlaneY;
uniform vec2 u_ao;
uniform Sphere u_boundingSphere;

const float PI = 3.14159265359;

// front to back blend
vec4 BlendCol(vec4 srcC, vec4 outC){
	srcC.rgb *= srcC.a;
	return outC + srcC * (1.0 - outC.a);
}

vec3 Hsv2rgb(float h, float s, float v){
    vec3 c = vec3(h, s, v);
    const vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

const float DISPLAY_GAMMA_COEFF = 1. / 2.2;
vec4 GammaCorrect(vec4 rgba) {
    return vec4((min(pow(rgba.r, DISPLAY_GAMMA_COEFF), 1.)),
                (min(pow(rgba.g, DISPLAY_GAMMA_COEFF), 1.)),
                (min(pow(rgba.b, DISPLAY_GAMMA_COEFF), 1.)),
                rgba.a);
}

const vec3 BLACK = vec3(0);
const vec3 WHITE = vec3(1);
const vec3 GRAY = vec3(0.78);
const vec3 RED = vec3(0.8, 0, 0);
const vec3 GREEN = vec3(0, 0.8, 0);
const vec3 BLUE = vec3(0, 0, 0.8);
const vec3 YELLOW = vec3(1, 1, 0);
const vec3 PINK = vec3(.78, 0, .78);
const vec3 LIGHT_BLUE = vec3(0, 1, 1);

vec3 CalcRay (const vec3 eye, const vec3 target, const vec3 up, const float fov,
              const vec2 resolution, const vec2 coord){
    float imagePlane = (resolution.y * .5) / tan(fov * .5);
    vec3 v = normalize(target - eye);
    vec3 xaxis = normalize(cross(v, up));
    vec3 yaxis =  normalize(cross(v, xaxis));
    vec3 center = v * imagePlane;
    vec3 origin = center - (xaxis * (resolution.x  *.5)) - (yaxis * (resolution.y * .5));
    return normalize(origin + (xaxis * coord.x) + (yaxis * (resolution.y - coord.y)));
}

vec3 CalcRayOrtho (const vec3 eye, const vec3 target, const vec3 up, const float orthoWidth,
                   const vec2 resolution, const vec2 coord,
                   out vec3 rayOrigin){
    vec3 v = normalize(target - eye);
    vec3 xaxis = normalize(cross(v, up));
    vec3 yaxis =  normalize(cross(v, xaxis));
    vec2 orthoPlane = vec2(orthoWidth, orthoWidth * resolution.y / resolution.x);
    vec3 planeOrigin = eye - (xaxis * (orthoPlane.x * .5)) - (yaxis * (orthoPlane.y * .5));
    rayOrigin = planeOrigin + (xaxis * orthoPlane.x * coord.x / resolution.x) + (yaxis * orthoPlane.y * coord.y / resolution.y);
    return v;
}

struct IsectInfo {
    int objId;
    int objIndex;
    int objComponentId;
    vec3 normal;
    vec3 intersection;
    float mint;
    float maxt;
    vec3 matColor;
    bool hit;
};

float MAX_FLOAT = 1e20;
const float THRESHOLD = 0.001;

IsectInfo NewIsectInfo() {
    IsectInfo isectInfo;
    isectInfo.objId = -1;
    isectInfo.objIndex = -1;
    isectInfo.objComponentId = -1;
    isectInfo.mint = MAX_FLOAT;
    isectInfo.maxt = 9999999.;
    isectInfo.hit = false;
    return isectInfo;
}

bool IntersectBoundingSphere(vec3 sphereCenter, float radius,
                             vec3 rayOrigin, vec3 rayDir,
                             out float t0, out float t1){
	vec3 v = rayOrigin - sphereCenter;
	float b = dot(rayDir, v);
	float c = dot(v, v) - radius * radius;
	float d = b * b - c;
	const float EPSILON = 0.0001;
	if(d >= 0.){
		float s = sqrt(d);
		float tm = -b - s;
		t0 = tm;
		if(tm <= EPSILON){
			tm = -b + s;
            t1 = tm;
			t0 = 0.;
		}else{
			t1 = -b + s;
		}
		if(EPSILON < tm){
			return true;
		}
	}
    t0 = 0.;
    t1 = MAX_FLOAT;
	return false;
}

bool IntersectBoundingPlane(const vec3 n, const vec3 p,
							const vec3 rayOrigin, const vec3 rayDir,
							inout float t0, inout float t1) {
	float d = -dot(p, n);
    float v = dot(n, rayDir);
    float t = -(dot(n, rayOrigin) + d) / v;
    if(THRESHOLD < t){
		if(v < 0.) {
			t0 = max(t, t0);
			t1 = MAX_FLOAT;
		} else {
			t0 = t0;
			t1 = t;
		}
		return true;
    }
    t0 = t0;
    t1 = MAX_FLOAT;
	return (v < 0.);
}

vec2 Rand2n(const vec2 co, const float sampleIndex) {
    vec2 seed = co * (sampleIndex + 1.0);
    seed+=vec2(-1,1);
    // implementation based on: lumina.sourceforge.net/Tutorials/Noise.html
    return vec2(fract(sin(dot(seed.xy ,vec2(12.9898,78.233))) * 43758.5453),
                fract(cos(dot(seed.xy ,vec2(4.898,7.23))) * 23421.631));
}

vec4 DistUnion(vec4 t1, vec4 t2) {
    return (t1.x < t2.x) ? t1 : t2;
}

float DistPlane(const vec3 pos, const vec3 p, const vec3 normal) {
    return dot(pos - p, normal);
}

void IntersectSphere(const int objId, const int objIndex, const int objComponentId,
                     const vec3 matColor,
                     const vec3 sphereCenter, const float radius,
                     const vec3 rayOrigin, const vec3 rayDir, inout IsectInfo isectInfo){
    vec3 v = rayOrigin - sphereCenter;
    float b = dot(rayDir, v);
    float c = dot(v, v) - radius * radius;
    float d = b * b - c;
    if(d >= 0.){
        float s = sqrt(d);
        float t = -b - s;
        if(t <= THRESHOLD) t = -b + s;
        if(THRESHOLD < t && t < isectInfo.mint){
            isectInfo.objId = objId;
            isectInfo.objIndex = objIndex;
            isectInfo.objComponentId = objComponentId;
            isectInfo.matColor = matColor;
            isectInfo.mint = t;
            isectInfo.intersection = (rayOrigin + t * rayDir);
            isectInfo.normal = normalize(isectInfo.intersection - sphereCenter);
            isectInfo.hit = true;
        }
    }
}

float DistSphere(const vec3 pos, const Sphere sphere) {
    return distance(pos, sphere.center) - sphere.r.x;
}

float DistPrism(const vec3 pos) {
    float d = -1.;
	{% for n in range(0, numPrismPlanes) %}
	d = max(DistPlane(pos, u_prismPlanes[{{ n }}].origin,
					  u_prismPlanes[{{ n }}].normal),
			d);
	{% endfor %}
    return d;
}

float DistInfSphairahedronAll(const vec3 pos) {
    float d = DistPrism(pos);
    {% for n in range(0, numExcavationSpheres) %}
	d = max(-DistSphere(pos, u_excavationPrismSpheres[{{ n }}]),
			d);
	{% endfor %}
	{% for n in range(0, numPrismSpheres) %}
	d = max(-DistSphere(pos, u_prismSpheres[{{ n }}]),
			d);
	{% endfor %}
    return d;
}

float DistInfSphairahedron(const vec3 pos) {
    float d = DistPrism(pos);
    {% for n in range(0, numDividePlanes) %}
    d = max(DistPlane(pos, u_dividePlanes[{{ n }}].origin,
					  u_dividePlanes[{{ n }}].normal),
			d);
    {% endfor %}
    {% for n in range(0, numExcavationSpheres) %}
	d = max(-DistSphere(pos, u_excavationPrismSpheres[{{ n }}]),
			d);
	{% endfor %}
	{% for n in range(0, numPrismSpheres) %}
	d = max(-DistSphere(pos, u_prismSpheres[{{ n }}]),
			d);
	{% endfor %}
    return d;
}

const float DIV_PI = 1.0 / PI;
const vec3 dielectricSpecular = vec3(0.04);

// This G term is used in glTF-WebGL-PBR
// Microfacet Models for Refraction through Rough Surfaces
float G1_GGX(float alphaSq, float NoX) {
    float tanSq = (1.0 - NoX * NoX) / max((NoX * NoX), 0.00001);
    return 2. / (1. + sqrt(1. + alphaSq * tanSq));
}

// 1 / (1 + delta(l)) * 1 / (1 + delta(v))
float Smith_G(float alphaSq, float NoL, float NoV) {
    return G1_GGX(alphaSq, NoL) * G1_GGX(alphaSq, NoV);
}

// Height-Correlated Masking and Shadowing
// Smith Joint Masking-Shadowing Function
float GGX_Delta(float alphaSq, float NoX) {
    return (-1. + sqrt(1. + alphaSq * (1. / (NoX * NoX) - 1.))) / 2.;
}

float SmithJoint_G(float alphaSq, float NoL, float NoV) {
    return 1. / (1. + GGX_Delta(alphaSq, NoL) + GGX_Delta(alphaSq, NoV));
}

float GGX_D(float alphaSq, float NoH) {
    float c = (NoH * NoH * (alphaSq - 1.) + 1.);
    return alphaSq / (c * c)  * DIV_PI;
}

vec3 computeIBL(vec3 diffuseColor, vec3 specularColor,
                vec3 reflection, vec3 L,
                float NoL, float NoV) {
    float mixFact = (exp(1. * NoL) - 1.) / (exp(1.) - 1.);
    vec3 diffuse = diffuseColor * mix(vec3(0.2), vec3(1), mixFact);

    // vec2 brdf = textureLod(u_brdfLUT,
    //                        vec2(NoV,
    //                             u_metallicRoughness.y), 0.0).rg;
    vec2 brdf = vec2(0);
    float LoReflect = clamp(dot(L, reflection), 0.0, 1.0);
    mixFact = (exp(2. * LoReflect) - 1.)/(exp(2.) - 1.);
    vec3 specularLight = mix(vec3(0.1, 0.1, 0.3), vec3(1, 1, 1), mixFact);
    vec3 specular = specularLight * (specularColor * brdf.x + brdf.y);
    return diffuse + specular;
}

vec3 BRDF(vec3 baseColor, float metallic, float roughness, vec3 dielectricSpecular,
          vec3 L, vec3 V, vec3 N) {
    vec3 H = normalize(L+V);

    float LoH = clamp(dot(L, H), 0.0, 1.0);
    float NoH = clamp(dot(N, H), 0.0, 1.0);
    float VoH = clamp(dot(V, H), 0.0, 1.0);
    float NoL = clamp(dot(N, L), 0.0, 1.0);
    float NoV = abs(dot(N, V));

    vec3 F0 = mix(dielectricSpecular, baseColor, metallic);
    vec3 cDiff = mix(baseColor * (1. - dielectricSpecular.r),
                     BLACK,
                     metallic);
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    // Schlick's approximation
    vec3 F = F0 + (vec3(1.) - F0) * pow((1. - VoH), 5.);

    vec3 diffuse = (vec3(1.) - F) * cDiff * DIV_PI;

    //float G = SmithJoint_G(alphaSq, NoL, NoV);
    float G = Smith_G(alphaSq, NoL, NoV);

    float D = GGX_D(alphaSq, NoH);

    vec3 specular = (F * G * D) / (4. * NoL * NoV);
    vec3  c = clamp((diffuse + specular) * NoL, 0.0, 1.0);
    c += computeIBL(cDiff, F0, normalize(reflect(-V, N)), L, NoL, NoV);
    return c;
}

float DistSphere(vec3 pos) {
    return distance(pos, vec3(0, 0, 0)) - 3.0;
}

void SphereInvert(inout vec3 pos, inout float dr, vec3 center, vec2 r) {
    vec3 diff = pos - center;
    float lenSq = dot(diff, diff);
    float k = r.y / lenSq;
    dr *= k; // (r * r) / lenSq
    pos = (diff * k) + center;
}

float DistLimitsetTerrain(vec3 pos, out float invNum) {
    float dr = 1.;
    invNum = 0.;

    float d;
    for(int i = 0; i < 2000; i++) {
        if(u_maxIterations <= i) break;
        bool inFund = true;
		{% for n in range(0, numPrismSpheres) %}
		if(distance(pos, u_prismSpheres[{{ n }}].center) < u_prismSpheres[{{ n }}].r.x) {
            invNum += (float({{ (n + 1) *  10 }}) + invNum) * u_colorWeight + 1.;
			SphereInvert(pos, dr,
						 u_prismSpheres[{{ n }}].center,
						 u_prismSpheres[{{ n }}].r);
			continue;
		}
		{% endfor %}

		{% for n in range(0, numPrismPlanes) %}
		pos -= u_prismPlanes[{{ n }}].origin;
		d = dot(pos, u_prismPlanes[{{ n }}].normal);
		if(d > 0.) {
            invNum += (float({{ (n + 1 + numPrismSpheres) *  10 }}) + invNum) * u_colorWeight + 1.;
			pos -= 2. * d * u_prismPlanes[{{ n }}].normal;
			pos += u_prismPlanes[{{ n }}].origin;
			continue;
		}
		pos += u_prismPlanes[{{ n }}].origin;
		{% endfor %}

        if(inFund) break;
    }
    return DistInfSphairahedron(pos) / abs(dr) * u_fudgeFactor;
}

const int ID_PRISM = 0;
const int ID_INI_SPHERES = 1;
bool u_displayRawSphairahedronPrism = false;
float g_invNum;
vec4 distFunc(const vec3 pos) {
    vec4 hit = vec4(MAX_FLOAT, -1, -1, -1);
    float dd = DistLimitsetTerrain(pos, g_invNum);
    return DistUnion(hit, vec4(dd,
                               ID_PRISM, -1, -1));
}

const vec2 NORMAL_COEFF = vec2(0.001, 0.);
vec3 computeNormal(const vec3 p) {
    return normalize(vec3(distFunc(p + NORMAL_COEFF.xyy).x - distFunc(p - NORMAL_COEFF.xyy).x,
                          distFunc(p + NORMAL_COEFF.yxy).x - distFunc(p - NORMAL_COEFF.yxy).x,
                          distFunc(p + NORMAL_COEFF.yyx).x - distFunc(p - NORMAL_COEFF.yyx).x));
}

const int MAX_MARCHING_LOOP = 3000;
void march(const vec3 rayOrg, const vec3 rayDir,
           inout IsectInfo isectInfo,
		   float tmin, float tmax) {
    float rayLength = tmin;
    vec3 rayPos = rayOrg + rayDir * rayLength;
    vec4 dist = vec4(-1);
    for(int i = 0 ; i < MAX_MARCHING_LOOP ; i++) {
        if(rayLength > tmax) break;
        dist = distFunc(rayPos);
        rayLength += dist.x;
        rayPos = rayOrg + rayDir * rayLength;
        if(dist.x < u_marchingThreshold) {
            isectInfo.objId = int(dist.y);
            //isectInfo.objIndex = int(dist.z);
            //isectInfo.objComponentId = int(dist.w);
			isectInfo.matColor = Hsv2rgb((1., -0.13 + (g_invNum) * 0.01), 1., 1.);
            isectInfo.intersection = rayPos;
            isectInfo.normal = computeNormal(rayPos);
            isectInfo.mint = rayLength;
            isectInfo.hit = true;
            break;
        }
    }
}

// This function is based on FractalLab's implementation
// http://hirnsohle.de/test/fractalLab/
float ambientOcclusion(vec3 p, vec3 n, float eps, float aoIntensity ){
    float o = 1.0;
    float k = aoIntensity;
    float d = 2.0 * eps;

    for (int i = 0; i < 5; i++) {
        o -= (d - distFunc(p + n * d).x) * k;
        d += eps;
        k *= 0.5;
    }

    return clamp(o, 0.0, 1.0);
}

float computeShadowFactor (const vec3 rayOrg, const vec3 rayDir,
                           const float mint, const float maxt, const float k) {
    float shadowFactor = 1.0;
    for(float t = mint ; t < maxt ;){
        float d = distFunc(rayOrg + rayDir * t).x;
        if(d < u_marchingThreshold) {
            shadowFactor = 0.;
            break;
        }

        shadowFactor = min(shadowFactor, k * d / t);
        t += d;
    }
    return clamp(shadowFactor, 0.0, 1.0);
}

const vec3 AMBIENT_FACTOR = vec3(0.3);
vec4 computeColor(const vec3 rayOrg, const vec3 rayDir) {
    IsectInfo isectInfo = NewIsectInfo();
    vec3 rayPos = rayOrg;

    vec3 l = vec3(0);
    float alpha = 1.;

    float transparency = 0.8;
    float coeff = 1.;

    for(int depth = 0 ; depth < 8; depth++){
		float tmin = 0.;
		float tmax = MAX_FLOAT;
		bool hit = true;

		// {% if renderMode == 0 %}
        // hit = IntersectBoundingPlane(vec3(0, 1, 0), vec3(0, u_boundingPlaneY, 0),
		//   							 rayPos, rayDir,
        //                              tmin, tmax);
		// {% else %}
		// hit = IntersectBoundingSphere(u_boundingSphere.center - u_boundingSphere.center,
        //                               u_boundingSphere.r.x,
        //                               rayPos, rayDir,
        //                               tmin, tmax);
        // {% endif %}

        // if(hit)
        march(rayPos, rayDir, isectInfo, tmin, tmax);

        if(isectInfo.hit) {
            vec3 matColor = isectInfo.matColor;
            bool transparent = false;
            transparent =  (isectInfo.objId == ID_INI_SPHERES) ?
                true : false;
            vec3 ambient = matColor * AMBIENT_FACTOR;

            if(transparent) {
                vec3 diffuse =  clamp((dot(isectInfo.normal, -u_lightDirection)), 0., 1.) * matColor;
                coeff *= transparency;
                l += (diffuse + ambient) * coeff;
                rayPos = isectInfo.intersection + rayDir * 0.000001 * 2.;
                isectInfo = NewIsectInfo();
                continue;
            } else {
                vec3 c = BRDF(matColor, u_metallicRoughness.x, u_metallicRoughness.y,
                              dielectricSpecular,
                              -u_lightDirection, -rayDir, isectInfo.normal);
                float k = u_castShadow ?
                    computeShadowFactor(isectInfo.intersection + 0.0001 * isectInfo.normal,
                                        -u_lightDirection,
                                        0.001, 5., 100.) : 1.;
                l += (c * k + ambient * ambientOcclusion(isectInfo.intersection,
                                                      isectInfo.normal,
                                                        u_ao.x, u_ao.y )) * coeff;
                break;
            }
        }
        //        alpha = 0.;
        break;
    }

    return vec4(l, alpha);
}

out vec4 outColor;
void main() {
    vec3 sum = vec3(0);
    vec2 coordOffset = Rand2n(gl_FragCoord.xy, u_numSamples);
    vec3 ray = CalcRay(u_camera.pos, u_camera.target, u_camera.up, u_camera.fov,
                       u_resolution, gl_FragCoord.xy + coordOffset);
    // vec3 org = u_camera.pos;
    // vec3 rayOrtho = CalcRayOrtho(u_camera.pos, u_camera.target, u_camera.up, 1.0,
    //                              u_resolution, gl_FragCoord.xy + coordOffset, org);
    vec4 texCol = textureLod(u_accTexture, gl_FragCoord.xy / u_resolution, 0.0);
	outColor = vec4(mix(clamp(computeColor(u_camera.pos, ray), 0.0, 1.0),
                        texCol, u_textureWeight));
    // outColor = vec4(mix(textureLod(u_brdfLUT, gl_FragCoord.xy / u_resolution, 0.0),
    //                     texCol, u_textureWeight));
}
