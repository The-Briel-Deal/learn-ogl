#include <fstream>
#include <iostream>
#include <string>

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <tuple>

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
std::string getShaderFromFile(std::string file_path);
bool checkForShaderCompileErrors(unsigned int shader);
bool checkForProgramLinkingErrors(unsigned int program);
void glfwInitialize();
std::tuple<unsigned int, unsigned int> createShaderProgram();

int main() {
  glfwInitialize();

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  glViewport(0, 0, 800, 600);

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  unsigned int shaderProgram, EBO;
  std::tie(shaderProgram, EBO) = createShaderProgram();

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw Vertices with our shaders */
    glUseProgram(shaderProgram);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void framebufferSizeCallback(GLFWwindow * /* window */, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

std::string getShaderFromFile(std::string file_path) {
  std::string vertexShaderSource = "";
  std::string line = "";
  std::ifstream stream;
  stream.open(file_path);

  while (std::getline(stream, line)) {
    vertexShaderSource += line + "\n";
  }
  stream.close();

  return vertexShaderSource;
}

bool checkForShaderCompileErrors(unsigned int shader) {
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    return false;
  }
  return true;
}

bool checkForProgramLinkingErrors(unsigned int program) {
  int success;
  char infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    return false;
  }
  return true;
}

void glfwInitialize() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

std::tuple<unsigned int, unsigned int> createShaderProgram() {
  /** Vertex Shader **/
  auto vertexShaderSource = getShaderFromFile("./vert.glsl");
  auto c_vertexShaderSource = vertexShaderSource.c_str();

  float vertices[] = {
      0.5f,  0.5f,  0.0f, // Top Right
      0.5f,  -0.5f, 0.0f, // Bottom Right
      -0.5f, -0.5f, 0.0f, // Bottom Left
      -0.5f, 0.5f,  0.0f, // Top Left
  };
  unsigned int indices[] = {
      0, 1, 3, // First Triangle
      1, 2, 3  // Second Triangle
  };

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertexShader, 1, &c_vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  if (!checkForShaderCompileErrors(vertexShader)) {
    return std::make_tuple(0, 0);
  }

  /** Fragment Shader **/
  auto fragmentShaderSource = getShaderFromFile("./frag.glsl");
  auto c_fragmentShaderSource = fragmentShaderSource.c_str();

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &c_fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  if (!checkForShaderCompileErrors(fragmentShader)) {
    return std::make_tuple(0,0);
  }

  /** Shader Program **/
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);   // No longer needed after linking
  glDeleteShader(fragmentShader); // No longer needed after linking

  if (!checkForProgramLinkingErrors(shaderProgram)) {
    return std::make_tuple(0,0);
  }

  return std::make_tuple(shaderProgram, EBO);
}
