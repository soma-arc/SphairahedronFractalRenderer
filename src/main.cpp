#include <cstdlib>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <jinja2cpp/template.h>
#include "args.hxx"
#include "nlohmann/json.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void CheckError() {
    GLenum err = glGetError();


    if ( err != GL_NO_ERROR ) {
        printf( "OpenGL ERROR code %d:\n", err);
    }
}

GLFWwindow* InitGLFW(int width, int height) {
    printf("Initialize GLFW\n");
    if (glfwInit() == GL_FALSE){

        std::cerr << "Can't initialize GLFW" << std::endl;
        exit(-1);
    }

    atexit(glfwTerminate);


    GLFWwindow * const window(glfwCreateWindow(/* int           width   = */ width,
                                               /* int           height  = */ height,
                                               /* const char  * title   = */ "Fractal",
                                               /* GLFWmonitor * monitor = */ NULL,
                                               /* GLFWwindow  * share   = */ NULL));
    if (window == NULL){
        std::cerr << "Can't create GLFW window." << std::endl;
        exit(-1);
    }

    glfwMakeContextCurrent(/* GLFWwindow *  window = */ window);
    return window;
}

// LoadShader and LinkShader functions are from exrview
// https://github.com/syoyo/tinyexr/tree/master/examples/exrview
bool LoadShader(
    GLenum shaderType,  // GL_VERTEX_SHADER or GL_FRAGMENT_SHADER (or maybe GL_COMPUTE_SHADER)
    GLuint& shader,
    const char* shaderSourceFilename)
{
    GLint val = 0;

    // free old shader/program
    if (shader != 0) glDeleteShader(shader);

    static GLchar srcbuf[16384];
    FILE *fp = fopen(shaderSourceFilename, "rb");
    if (!fp) {
        fprintf(stderr, "failed to load shader: %s\n", shaderSourceFilename);
        return false;
    }
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);
    len = fread(srcbuf, 1, len, fp);
    srcbuf[len] = 0;
    fclose(fp);

    static const GLchar *src = srcbuf;

    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &val);
    if (val != GL_TRUE) {
        char log[4096];
        GLsizei msglen;
        glGetShaderInfoLog(shader, 4096, &msglen, log);
        printf("%s\n", log);
        assert(val == GL_TRUE && "failed to compile shader");
    }

    printf("Load shader [ %s ] OK\n", shaderSourceFilename);
    return true;
}

bool LinkShader(
    GLuint& prog,
    GLuint& vertShader,
    GLuint& fragShader)
{
    GLint val = 0;

    if (prog != 0) {
        glDeleteProgram(prog);
    }

    prog = glCreateProgram();

    glAttachShader(prog, vertShader);
    glAttachShader(prog, fragShader);
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &val);
    assert(val == GL_TRUE && "failed to link shader");

    printf("Link shader OK\n");

    return true;
}

static GLuint g_squareVBO;
std::vector<GLint> g_uniLocations;
float g_numSamples = 0;
int g_maxSamples = 10;
void CreateSquareVbo(GLuint buffer) {
    GLfloat square[] = {-1, -1,
                        -1, 1,
                        1, -1,
                        1, 1};
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat) * 8,
                 square,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GetUniLocations(GLint program) {
    g_uniLocations.clear();
    g_uniLocations.push_back(glGetUniformLocation(program,
                                                "u_accTexture"));
    g_uniLocations.push_back(glGetUniformLocation(program,
                                                "u_textureWeight"));
    g_uniLocations.push_back(glGetUniformLocation(program,
                                                "u_numSamples"));
    g_uniLocations.push_back(glGetUniformLocation(program,
                                                "u_resolution"));
}

void SetUniformValues(int width, int height, GLuint texture) {
    printf("set uniform values\n");
    int index = 0;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(g_uniLocations[index++], 0);
    glUniform1f(g_uniLocations[index++],
                g_numSamples / (g_numSamples + 1));
    glUniform1f(g_uniLocations[index++], g_numSamples);
    glUniform2f(g_uniLocations[index++], float(width), float(height));
    g_numSamples++;
    printf("set uniform values Done\n");
}

GLuint g_renderProgram;
void compileRenderProgram() {
    GLuint vert_id = 0, frag_id = 0;
    {
        bool ret = LoadShader(GL_VERTEX_SHADER, vert_id,
                              "./src/renderToTexture.vert");
        if (!ret) {
            fprintf(stderr, "Failed to compile vertex shader.\n");
            exit(-1);
        }
    }

    {
        bool ret = LoadShader(GL_FRAGMENT_SHADER, frag_id,
                              "./src/renderToTexture.frag");
        if (!ret) {
            fprintf(stderr, "Failed to compile fragment shader.\n");
            exit(-1);
        }
    }

    {
        bool ret = LinkShader(g_renderProgram, vert_id, frag_id);
        if (!ret) {
            fprintf(stderr, "Failed to link shader.\n");
            exit(-1);
        }
    }
    glBindAttribLocation(g_renderProgram, 0, "a_vertex");
    glBindFragDataLocation(g_renderProgram, 0, "outColor");

    printf("Get uniform locations\n");
    GetUniLocations(g_renderProgram);
    printf("Done\n");
}

GLuint g_renderCanvasProgram = 0;
std::vector<GLuint> g_renderingTextures;
GLuint g_textureFrameBuffer;
void InitRenderer(int width, int height) {
    printf("Initialize renderer\n");
    //CreateSquareVbo(g_squareVBO);

    GLfloat square[] = {-1, -1,
                        -1, 1,
                        1, -1,
                        1, 1};
    glGenBuffers(1, &g_squareVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_squareVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat) * 8,
                 square,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    GLuint vert_id = 0, frag_id = 0;
    {
        bool ret = LoadShader(GL_VERTEX_SHADER, vert_id,
                              "./src/render.vert");
        if (!ret) {
            fprintf(stderr, "Failed to compile vertex shader.\n");
            exit(-1);
        }
    }
    {
        bool ret = LoadShader(GL_FRAGMENT_SHADER, frag_id,
                              "./src/render.frag");
        if (!ret) {
            fprintf(stderr, "Failed to compile fragment shader.\n");
            exit(-1);
        }
    }
    {
        bool ret = LinkShader(g_renderCanvasProgram,
                              vert_id,
                              frag_id);
        if (!ret) {
            fprintf(stderr, "Failed to link shader.\n");
            exit(-1);
        }
    }

    glBindAttribLocation(g_renderCanvasProgram, 0, "a_vertex");
    glBindFragDataLocation(g_renderCanvasProgram, 0, "outColor");
    glCreateFramebuffers(1, &g_textureFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    printf("Create textures\n");
    g_renderingTextures.clear();
    for(int i = 0; i < 2; i++) {
        GLuint textureHandle;
        glGenTextures(1, &textureHandle);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureHandle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        g_renderingTextures.push_back(textureHandle);
    }

    printf("Compile Render program\n");
    compileRenderProgram();
    printf("Compile Render program OK\n");
}

void RenderToTexture(int width, int height){
    printf("Render to Texture\n");
    glUseProgram(g_renderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_FRAMEBUFFER, g_textureFrameBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, g_squareVBO);
    //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    //                     g_renderingTextures[0], 0);
    //GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    //glDrawBuffers(1, DrawBuffers);
    glViewport(0, 0, width, height);
    printf("set program\n");
    SetUniformValues(width, height, g_renderingTextures[0]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, g_renderingTextures[1], 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT,
                          GL_FALSE, 0, 0);
    printf("draw arrays\n");
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFlush();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    printf("reverse\n");
    std::reverse(g_renderingTextures.begin(),
                 g_renderingTextures.end());
}

void RenderToCanvas(int width, int height) {
    printf("Render to Canvas\n");
    glViewport(0, 0, width, height);
    glUseProgram(g_renderCanvasProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_renderingTextures[0]);
    GLuint tex = glGetUniformLocation(g_renderCanvasProgram,
                                      "u_texture");
    glUniform1i(tex, g_renderingTextures[0]);
    glBindBuffer(GL_ARRAY_BUFFER, g_squareVBO);
    glVertexAttribPointer(0, 2,
                          GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFlush();
}

void GLFWLoop(GLFWwindow* window, int width, int height) {
    printf("Rendering Start");
    while (glfwWindowShouldClose(/* GLFWwindow * window = */ window) == GL_FALSE){
        glClear(GL_COLOR_BUFFER_BIT);

        RenderToTexture(width, height);
        RenderToCanvas(width, height);

        if(int(g_numSamples) == g_maxSamples) {
            return;
        }
        glfwSwapBuffers(window);
        glfwWaitEvents();
    }
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

    GLFWwindow* window = InitGLFW(width, height);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Can't initialize GLEW" << std::endl;
        return 1;
    }

    InitRenderer(width, height);
    printf("Initialization Done\n");
    //GLFWLoop(window, width, height);
    printf("Rendering Start\n");
    while (glfwWindowShouldClose(/* GLFWwindow * window = */ window) == GL_FALSE){
        glClear(GL_COLOR_BUFFER_BIT);

        RenderToTexture(width, height);
        RenderToCanvas(width, height);

        if(int(g_numSamples) == g_maxSamples) {
            break;
        }
        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    glReadBuffer( GL_BACK );
    unsigned char *textureData = new unsigned char[width * height * 3];
    std::fill(textureData, textureData + width * height * 3, 0);

    glReadPixels(
        0,
        0,
        width,
        height,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        textureData
	);

    std::string filename = "limitset.png";
    stbi_write_png(filename.c_str(), width, height,
                   3, textureData,
                   0);
    delete[] textureData;
    
    return 0;
}
