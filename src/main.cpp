#include <cstdlib>
#include <iostream>
#include <GLFW/glfw3.h>


int main()
{
  if (glfwInit() == GL_FALSE){

    std::cerr << "Can't initialize GLFW" << std::endl;
    return 1;
  }


  atexit(glfwTerminate);


  GLFWwindow * const window(glfwCreateWindow(/* int           width   = */ 640,
                                             /* int           height  = */ 480,
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



  while (glfwWindowShouldClose(/* GLFWwindow * window = */ window) == GL_FALSE){
    glClear(/* GLbitfield mask = */ GL_COLOR_BUFFER_BIT);


    glfwSwapBuffers(window);

    glfwWaitEvents();
  }

  return 0;
}
