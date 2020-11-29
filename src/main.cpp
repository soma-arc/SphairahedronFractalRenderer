#include <stdlib.h>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <jinja2cpp/template.h>
#include <sstream>
#include <string>
#include <string.h>
using namespace std;
#include "args.hxx"
#include "nlohmann/json.hpp"
#include "camera.h"
#include "./cube/cubeA.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLFWwindow* window;

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? \n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
 	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

GLuint LoadShaders(const char * vertex_file_path,
                   std::string fragment_string){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? \n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	GLint Result = GL_FALSE;
	int InfoLogLength;
    FragmentShaderCode = fragment_string;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling fragment shader\n");
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
 	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

    printf("LoadShaders Done\n");
	return ProgramID;
}


int main(int argc, char** argv) {
    args::ArgumentParser parser("Render sphairahedron-based fractals.");
    args::ValueFlag<std::string> inputJson(parser, "json", "Input json file",
                                           {'j', "json"}, "scene.json");
    args::ValueFlag<int> widthArg(parser, "width",
                                 "width (1024)",
                                 {'w', "width"}, 1024);
    args::ValueFlag<int> heightArg(parser, "height",
                                 "height (1024)",
                                 {'h', "height"}, 1024);
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << parser;
        return 0;
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    } catch (args::ValidationError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    std::string inputJsonFileName = args::get(inputJson);
    int width = args::get(widthArg);
    int height = args::get(heightArg);
    std::ifstream ifs(inputJsonFileName);
    nlohmann::json jsonObj;
    if (!ifs) {
        std::cout << "Can't open " << inputJsonFileName << std::endl;
        return 1;
    }
    ifs >> jsonObj;
    ifs.close();




    // CubeA cubeA(0.1, 0.1);
    // printf("cubeA planes zb %f zc %f\n",
    //        cubeA.zb, cubeA.zc);

    // std::string source;
	// std::ifstream FragmentShaderStream("./src/shaders/prism.jinja.frag", std::ios::in);
	// if(FragmentShaderStream.is_open()){
    //     printf("opened\n");
	// 	std::stringstream sstr;
	// 	sstr << FragmentShaderStream.rdbuf();
	// 	source = sstr.str();
	// 	FragmentShaderStream.close();
	// }
    // printf("loaded\n");
    //printf("loaded source\n %s\n", source.c_str());

    int mSamples = jsonObj["maxSamples"];
    int wWidth = jsonObj["windowWidth"];
    int wHeight = jsonObj["windowHeight"];
    printf("max samples %d  w x h %d x %d\n",
           mSamples, wWidth, wHeight);

// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, 0);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Fractal", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window." );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

    // We would expect width and height to be 1024 and 768
    int windowWidth = 1024;
    int windowHeight = 768;
    // But on MacOS X with a retina screen it'll be 1024*2 and 768*2, so we get the actual framebuffer size:
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
//    GLuint programID = LoadShaders( "./src/shaders/renderToTexture.vert",
//                                    "./src/shaders/renderToTexture.frag" );
    printf("gen cubeA\n");
    CubeA cubeA(0.1, 0.6);
    printf("cubeA planes zb %f zc %f\n",
           cubeA.zb, cubeA.zc);

    std::string source;
	//std::ifstream FragmentShaderStream("./src/shaders/prism.jinja.frag", std::ios::in);
    std::ifstream FragmentShaderStream("./src/shaders/limitset.jinja.frag", std::ios::in);
    if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		source = sstr.str();
		FragmentShaderStream.close();
	}
    jinja2::Template tpl;
    tpl.Load(source);

    std::string result = tpl.RenderAsString(cubeA.getShaderTemplateContext()).value();
    printf("rendered result\n %s\n", result.c_str());
    GLuint programID = LoadShaders("./src/shaders/renderToTexture.vert",
                                   result);

    GLuint accTextureID = glGetUniformLocation(programID,
                                               "u_accTexture");
    GLuint BRDFTextureID = glGetUniformLocation(programID,
                                               "u_brdfLUT");
    GLuint textureWeightID = glGetUniformLocation(programID,
                                                  "u_textureWeight");
    GLuint numSamplesID = glGetUniformLocation(programID,
                                               "u_numSamples");
    GLuint resolutionID = glGetUniformLocation(programID,
                                               "u_resolution");
    vector<GLuint> uniLocations;
    Camera camera(Vec3f(0,3,3), Vec3f(0, 0, 0),
                  60, Vec3f(0, -1, 0));
    vector<GLuint> prismPlanesIDs;
    GLuint prismPlanes1Origin = glGetUniformLocation(programID,
                                               "u_prismPlanes[0].origin");
    GLuint prismPlanes1Normal = glGetUniformLocation(programID,
                                               "u_prismPlanes[0].normal");
    GLuint prismPlanes2Origin = glGetUniformLocation(programID,
                                               "u_prismPlanes[1].origin");
    GLuint prismPlanes2Normal = glGetUniformLocation(programID,
                                               "u_prismPlanes[1].normal");
    GLuint prismPlanes3Origin = glGetUniformLocation(programID,
                                               "u_prismPlanes[2].origin");
    GLuint prismPlanes3Normal = glGetUniformLocation(programID,
                                               "u_prismPlanes[2].normal");
    
    GLuint prismSpheres1Center = glGetUniformLocation(programID,
                                                      "u_prismSpheres[0].center");
    GLuint prismSpheres1R = glGetUniformLocation(programID,
                                                 "u_prismSpheres[0].r");
    GLuint prismSpheres2Center = glGetUniformLocation(programID,
                                                      "u_prismSpheres[1].center");
    GLuint prismSpheres2R = glGetUniformLocation(programID,
                                                 "u_prismSpheres[1].r");
    GLuint prismSpheres3Center = glGetUniformLocation(programID,
                                                      "u_prismSpheres[2].center");
    GLuint prismSpheres3R = glGetUniformLocation(programID,
                                                 "u_prismSpheres[2].r");
    GLuint prismDividePlane1Origin = glGetUniformLocation(programID,
                                                          "u_dividePlanes[0].origin");
    GLuint prismDividePlane1Normal = glGetUniformLocation(programID,
                                                          "u_dividePlanes[0].normal");
    GLuint castShadow = glGetUniformLocation(programID,
                                             "u_castShadow");
    GLuint lightDirection = glGetUniformLocation(programID,
                                                 "u_lightDirection");
    GLuint aoID = glGetUniformLocation(programID,
                                       "u_ao");
    GLuint marchingThresholdID = glGetUniformLocation(programID,
                                                      "u_marchingThreshold");
    GLuint fudgeFactorID = glGetUniformLocation(programID,
                                                "u_fudgeFactor");
    GLuint maxIterationsID = glGetUniformLocation(programID,
                                                  "u_maxIterations");

    printf("Camera uniform... \n");
    camera.getUniformLocations(programID, uniLocations);
    printf("Sphairahedron uniform... \n");
    cubeA.getUniformLocations(programID, uniLocations);
    printf("Getting uniform... Done\n");
    GLfloat square[] = {-1, -1,
                        -1, 1,
                        1, -1,
                        1, 1};
    GLuint squarebuffer;
    glGenBuffers(1, &squarebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, squarebuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat) * 8,
                 square,
                 GL_STATIC_DRAW);


	// ---------------------------------------------
	// Render to Texture - specific code begins here
	// ---------------------------------------------

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    std::vector<GLuint> renderedTextures;
	// The texture we're going to render to
    for(int i = 0; i < 2; i++) {
        GLuint renderedTexture;
        glGenTextures(1, &renderedTexture);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, renderedTexture);

        // Give an empty image to OpenGL ( the last "0" means "empty" )
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, windowWidth, windowHeight,
                     0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
        renderedTextures.push_back(renderedTexture);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         renderedTextures[1], 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;


    int w,h,n;
    unsigned char *BRDFData = stbi_load("./src/img/brdfLUT.png",
                                        &w, &h, &n, STBI_rgb);
    printf("Texture %d x %d  %d\n", w, h, n);
    if(BRDFData == nullptr){
        throw(std::string("Failed to load texture"));
    }
    GLuint BRDFTexture;
    glGenTextures(1, &BRDFTexture);
    glBindTexture(GL_TEXTURE_2D, BRDFTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, BRDFData);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
    
    stbi_image_free(BRDFData);

    
	GLuint quad_programID = LoadShaders( "./src/shaders/renderToScreen.vert",
                                         "./src/shaders/renderToScreen.frag" );
	GLuint texID = glGetUniformLocation(quad_programID,
                                        "u_renderedTexture");

	float numSamples = 0.0f;
    int maxSamples = 5;
    printf("Rendering...\n");
	do{
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glViewport(0,0,windowWidth,windowHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		glUseProgram(programID);

		// Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderedTextures[0]);
		glUniform1i(accTextureID, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, BRDFTexture);
		glUniform1i(BRDFTextureID, 1);
        
        glUniform1f(textureWeightID,
                    numSamples / (numSamples + 1.0f));
        glUniform1f(numSamplesID, numSamples);
        glUniform2f(resolutionID, float(windowWidth),
                    float(windowHeight));
        int index = 0;
        printf("camera uniform values\n");
        index = camera.setUniformValues(index, uniLocations);
        printf("cubeA uniform values\n");
        index = cubeA.setUniformValues(index, uniLocations);
        glUniform3f(prismPlanes1Origin,
                    cubeA.planes[0].p1.x(),
                    cubeA.planes[0].p1.y(),
                    cubeA.planes[0].p1.z());
        glUniform3f(prismPlanes1Normal,
                    cubeA.planes[0].normal.x(),
                    cubeA.planes[0].normal.y(),
                    cubeA.planes[0].normal.z());
        glUniform3f(prismPlanes2Origin,
                    cubeA.planes[1].p1.x(),
                    cubeA.planes[1].p1.y(),
                    cubeA.planes[1].p1.z());
        glUniform3f(prismPlanes2Normal,
                    cubeA.planes[1].normal.x(),
                    cubeA.planes[1].normal.y(),
                    cubeA.planes[1].normal.z());
        glUniform3f(prismPlanes3Origin,
                    cubeA.planes[2].p1.x(),
                    cubeA.planes[2].p1.y(),
                    cubeA.planes[2].p1.z());
        glUniform3f(prismPlanes3Normal,
                    cubeA.planes[2].normal.x(),
                    cubeA.planes[2].normal.y(),
                    cubeA.planes[2].normal.z());
        glUniform3f(prismSpheres1Center,
                    cubeA.prismSpheres[0].center.x(),
                    cubeA.prismSpheres[0].center.y(),
                    cubeA.prismSpheres[0].center.z());
        glUniform2f(prismSpheres1R,
                    cubeA.prismSpheres[0].r,
                    cubeA.prismSpheres[0].rSq);
        glUniform3f(prismSpheres2Center,
                    cubeA.prismSpheres[1].center.x(),
                    cubeA.prismSpheres[1].center.y(),
                    cubeA.prismSpheres[1].center.z());
        glUniform2f(prismSpheres2R,
                    cubeA.prismSpheres[1].r,
                    cubeA.prismSpheres[1].rSq);
        glUniform3f(prismSpheres3Center,
                    cubeA.prismSpheres[2].center.x(),
                    cubeA.prismSpheres[2].center.y(),
                    cubeA.prismSpheres[2].center.z());
        glUniform2f(prismSpheres3R,
                    cubeA.prismSpheres[2].r,
                    cubeA.prismSpheres[2].rSq);
        glUniform3f(prismDividePlane1Origin,
                    cubeA.dividePlanes[0].p1.x(),
                    cubeA.dividePlanes[0].p1.y(),
                    cubeA.dividePlanes[0].p1.z());
        glUniform3f(prismDividePlane1Normal,
                    cubeA.dividePlanes[0].normal.x(),
                    cubeA.dividePlanes[0].normal.y(),
                    cubeA.dividePlanes[0].normal.z());
        glUniform1i(castShadow, true);
        glUniform3f(lightDirection, -0.7071067811865475,
                    -0.7071067811865475,
                    0);
        glUniform2f(aoID, 0.0968, 2.0);
        glUniform1f(marchingThresholdID, 0.0001);
        glUniform1f(fudgeFactorID, 0.2);
        glUniform1i(maxIterationsID, 50);
        
        printf("Done\n");
        glBindTexture(GL_TEXTURE_2D, renderedTextures[1]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             renderedTextures[1], 0);
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, squarebuffer);
		glVertexAttribPointer(
			0,                  // attribute
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Index buffer
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, squarebuffer);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        std::reverse(renderedTextures.begin(),
                     renderedTextures.end());

        // --- Render to the screen ---
        if(int(numSamples) != maxSamples) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        // Render on the whole framebuffer, complete from the lower left corner to the upper right
		glViewport(0,0,windowWidth,windowHeight);

		glUseProgram(quad_programID);
		// Bind our texture in Texture Unit 0
        glBindTexture(GL_TEXTURE_2D, renderedTextures[0]);
		glUniform1i(texID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, squarebuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        if(int(numSamples) == maxSamples) break;
        numSamples++;
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );
    printf("Done.\n");

    glReadBuffer( GL_BACK );
    unsigned char *textureData = new unsigned char[windowWidth * windowHeight * 4];
    std::fill(textureData, textureData + windowWidth * windowHeight * 4, 0);

    glReadPixels(
        0,
        0,
        windowWidth,
        windowHeight,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        textureData
	);

    std::string filename = "limitset.png";
    stbi_write_png(filename.c_str(), windowWidth, windowHeight,
                   4, textureData, 0);
    printf("Image was written.\n");
    delete[] textureData;

	// Cleanup VBO and shader
	glDeleteBuffers(1, &squarebuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &renderedTextures[0]);
    glDeleteTextures(1, &renderedTextures[1]);

	glDeleteFramebuffers(1, &FramebufferName);
	glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(quad_programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();


    return 0;
}
