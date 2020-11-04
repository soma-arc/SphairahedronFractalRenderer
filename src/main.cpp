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

int readShaderSource(GLuint shader, const char *file)
{
  FILE *fp;
  const GLchar *source;
  GLsizei length;
  int ret;

  fp = fopen(file, "rb");
  if (fp == NULL) {
    perror(file);
    return -1;
  }

  fseek(fp, 0L, SEEK_END);
  length = ftell(fp);

  source = (GLchar *)malloc(length);
  if (source == NULL) {
    fprintf(stderr, "Could not allocate read buffer.\n");
    return -1;
  }

  fseek(fp, 0L, SEEK_SET);
  ret = fread((void *)source, 1, length, fp) != (size_t)length;
  fclose(fp);

  if (ret)
    fprintf(stderr, "Could not read file: %s.\n", file);
  else
    glShaderSource(shader, 1, &source, &length);

  free((void *)source);

  return ret;
}

void printShaderInfoLog(GLuint shader)
{
  GLsizei bufSize;

  glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &bufSize);

  if (bufSize > 1) {
    GLchar *infoLog = (GLchar *)malloc(bufSize);

    if (infoLog != NULL) {
      GLsizei length;

      glGetShaderInfoLog(shader, bufSize, &length, infoLog);
      fprintf(stderr, "InfoLog:\n%s\n\n", infoLog);
      free(infoLog);
    }
    else
      fprintf(stderr, "Could not allocate InfoLog buffer.\n");
  }
}

void printProgramInfoLog(GLuint program)
{
  GLsizei bufSize;

  glGetProgramiv(program, GL_INFO_LOG_LENGTH , &bufSize);

  if (bufSize > 1) {
    GLchar *infoLog = (GLchar *)malloc(bufSize);

    if (infoLog != NULL) {
      GLsizei length;

      glGetProgramInfoLog(program, bufSize, &length, infoLog);
      fprintf(stderr, "InfoLog:\n%s\n\n", infoLog);
      free(infoLog);
    }
    else
      fprintf(stderr, "Could not allocate InfoLog buffer.\n");
  }
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

static GLuint buffer;
typedef GLfloat Position[2];
Position *position;
std::vector<GLint> uniLocations;

void GetUniLocations(GLint program) {
    uniLocations = std::vector<GLint>();
    uniLocations.push_back(glGetUniformLocation(program, "u_resolution"));
}

void SetUniformValues(int width, int height) {
    int index = 0;
    glUniform2f(uniLocations[index++], float(width), float(height));
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

    if (glfwInit() == GL_FALSE){

        std::cerr << "Can't initialize GLFW" << std::endl;
        return 1;
    }

    atexit(glfwTerminate);


    GLFWwindow * const window(glfwCreateWindow(/* int           width   = */ width,
                                               /* int           height  = */ height,
                                               /* const char  * title   = */ "Hello!",
                                               /* GLFWmonitor * monitor = */ NULL,
                                               /* GLFWwindow  * share   = */ NULL));
    if (window == NULL){
        std::cerr << "Can't create GLFW window." << std::endl;
        return 1;
    }

    glfwMakeContextCurrent(/* GLFWwindow *  window = */ window);

    glClearColor(/* GLfloat red   = */ 0.2f,
                 /* GLfloat green = */ 0.2f,
                 /* GLfloat blue  = */ 0.2f,
                 /* GLfloat alpha = */ 0.0f);


    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Can't initialize GLEW" << std::endl;
        return 1;
    }

    GLuint vert_id = 0, frag_id = 0, prog_id = 0;
    {
        bool ret = LoadShader(GL_VERTEX_SHADER, vert_id, "./src/render.vert");
        if (!ret) {
            fprintf(stderr, "Failed to compile vertex shader.\n");
            exit(-1);
        }
    }

    {
        bool ret = LoadShader(GL_FRAGMENT_SHADER, frag_id, "./src/render.frag");
        if (!ret) {
            fprintf(stderr, "Failed to compile fragment shader.\n");
            exit(-1);
        }
    }

    {
        bool ret = LinkShader(prog_id, vert_id, frag_id);
        if (!ret) {
            fprintf(stderr, "Failed to link shader.\n");
            exit(-1);
        }
    }

    glBindAttribLocation(prog_id, 0, "a_vertex");
    glBindFragDataLocation(prog_id, 0, "outColor");
    glUseProgram(prog_id);

    glGenBuffers(1, &buffer);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof (Position) * 4, NULL, GL_STATIC_DRAW);

    position = (Position *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    position[0][0] =  -1.0;
    position[0][1] =  -1.0;
    position[1][0] = -1.0;
    position[1][1] =  1.0;
    position[2][0] =  1.0;
    position[2][1] = -1.0;
    position[3][0] =  1.0;
    position[3][1] =  1.0;

    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GetUniLocations(prog_id);

    while (glfwWindowShouldClose(/* GLFWwindow * window = */ window) == GL_FALSE){
        glClear(/* GLbitfield mask = */ GL_COLOR_BUFFER_BIT);

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog_id);

        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

        SetUniformValues(width, height);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(0);

        glFlush();


        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    return 0;
}
