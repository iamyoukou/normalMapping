#include "common.h"
#include <glm/gtx/string_cast.hpp>

GLFWwindow *window;

vec3 lightPosition = vec3(3.f, 3.f, 3.f);
vec3 lightColor = vec3(1.f, 1.f, 1.f);
float lightPower = 1.f;

vec3 materialDiffuseColor = vec3(0.1f, 0.1f, 0.1f);
vec3 materialAmbientColor = vec3(0.1f, 0.1f, 0.1f);
vec3 materialSpecularColor = vec3(1.f, 1.f, 1.f);

float verticalAngle = -1.51191;
float horizontalAngle = -0.0948035;
float initialFoV = 45.0f;
float speed = 5.0f;
float mouseSpeed = 0.005f;

mat4 model, view, projection;
vec3 eyePoint = vec3(2.088317, -0.040358, -0.151899);
vec3 eyeDirection =
    vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle),
         sin(verticalAngle) * sin(horizontalAngle));
vec3 up = vec3(0.f, 1.f, 0.f);

GLuint exeShader;
GLuint vboVtxCoords, vboUvs, vboNormals;
GLuint vao;
GLuint tboBase, tboNormal;
GLint uniM, uniV, uniP, uniMvp;
GLint uniLightColor, uniLightPosition, uniLightPower;
// material diffuse, ambient, specular color
GLint uniDiffuse, uniAmbient, uniSpecular;
GLint uniTexBase, uniTexNormal;

void computeMatricesFromInputs(mat4 &, mat4 &);
void keyCallback(GLFWwindow *, int, int, int, int);

void initGL();
void initOthers();
void initMatrices();
void initLight();
void initTexture();
GLuint createTexture(GLuint, GLuint, string, string, FREE_IMAGE_FORMAT);

int main(int argc, char **argv) {
  initGL();
  initOthers();

  // build shader program
  exeShader = buildShader("vertex_shader.glsl", "fragment_shader.glsl");
  glUseProgram(exeShader);

  // load mesh
  Mesh mesh = loadObj("cube.obj");

  // write vertex coordinate to array
  int nOfFaces = mesh.faces.size();

  // 3 vertices per face, 3 float per vertex coord, 2 float per tex coord
  GLfloat *aVtxCoords = new GLfloat[nOfFaces * 3 * 3];
  GLfloat *aUvs = new GLfloat[nOfFaces * 3 * 2];
  GLfloat *aNormals = new GLfloat[nOfFaces * 3 * 3];

  for (size_t i = 0; i < nOfFaces; i++) {
    // vertex 1
    int vtxIdx = mesh.faces[i].v1;
    aVtxCoords[i * 9 + 0] = mesh.vertices[vtxIdx].x;
    aVtxCoords[i * 9 + 1] = mesh.vertices[vtxIdx].y;
    aVtxCoords[i * 9 + 2] = mesh.vertices[vtxIdx].z;

    // normal for vertex 1
    int nmlIdx = mesh.faces[i].vn1;
    aNormals[i * 9 + 0] = mesh.faceNormals[nmlIdx].x;
    aNormals[i * 9 + 1] = mesh.faceNormals[nmlIdx].y;
    aNormals[i * 9 + 2] = mesh.faceNormals[nmlIdx].z;

    // uv for vertex 1
    int uvIdx = mesh.faces[i].vt1;
    aUvs[i * 6 + 0] = mesh.uvs[uvIdx].x;
    aUvs[i * 6 + 1] = mesh.uvs[uvIdx].y;

    // vertex 2
    vtxIdx = mesh.faces[i].v2;
    aVtxCoords[i * 9 + 3] = mesh.vertices[vtxIdx].x;
    aVtxCoords[i * 9 + 4] = mesh.vertices[vtxIdx].y;
    aVtxCoords[i * 9 + 5] = mesh.vertices[vtxIdx].z;

    // normal for vertex 2
    nmlIdx = mesh.faces[i].vn2;
    aNormals[i * 9 + 3] = mesh.faceNormals[nmlIdx].x;
    aNormals[i * 9 + 4] = mesh.faceNormals[nmlIdx].y;
    aNormals[i * 9 + 5] = mesh.faceNormals[nmlIdx].z;

    // uv for vertex 2
    uvIdx = mesh.faces[i].vt2;
    aUvs[i * 6 + 2] = mesh.uvs[uvIdx].x;
    aUvs[i * 6 + 3] = mesh.uvs[uvIdx].y;

    // vertex 3
    vtxIdx = mesh.faces[i].v3;
    aVtxCoords[i * 9 + 6] = mesh.vertices[vtxIdx].x;
    aVtxCoords[i * 9 + 7] = mesh.vertices[vtxIdx].y;
    aVtxCoords[i * 9 + 8] = mesh.vertices[vtxIdx].z;

    // normal for vertex 3
    nmlIdx = mesh.faces[i].vn3;
    aNormals[i * 9 + 6] = mesh.faceNormals[nmlIdx].x;
    aNormals[i * 9 + 7] = mesh.faceNormals[nmlIdx].y;
    aNormals[i * 9 + 8] = mesh.faceNormals[nmlIdx].z;

    // uv for vertex 3
    uvIdx = mesh.faces[i].vt3;
    aUvs[i * 6 + 4] = mesh.uvs[uvIdx].x;
    aUvs[i * 6 + 5] = mesh.uvs[uvIdx].y;
  }

  // vao
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // vbo for vertex
  glGenBuffers(1, &vboVtxCoords);
  glBindBuffer(GL_ARRAY_BUFFER, vboVtxCoords);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 3 * 3, aVtxCoords,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // vbo for texture
  glGenBuffers(1, &vboUvs);
  glBindBuffer(GL_ARRAY_BUFFER, vboUvs);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 3 * 2, aUvs,
               GL_STATIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  // vbo for normal
  glGenBuffers(1, &vboNormals);
  glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 3 * 3, aNormals,
               GL_STATIC_DRAW);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);

  initTexture();
  initMatrices();
  initLight();

  // a rough way to solve cursor position initialization problem
  // must call glfwPollEvents once to activate glfwSetCursorPos
  // this is a glfw mechanism problem
  glfwPollEvents();
  glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    // reset
    glClearColor(0.f, 0.f, 0.4f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // view control
    computeMatricesFromInputs(projection, view);
    glUniformMatrix4fv(uniV, 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(uniP, 1, GL_FALSE, value_ptr(projection));

    // draw 3d model
    glDrawArrays(GL_TRIANGLES, 0, nOfFaces * 3);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  delete[] aVtxCoords;
  delete[] aUvs;
  delete[] aNormals;

  glDeleteBuffers(1, &vboVtxCoords);
  glDeleteBuffers(1, &vboUvs);
  glDeleteBuffers(1, &vboNormals);
  glDeleteTextures(1, &tboBase);
  glDeleteVertexArrays(1, &vao);

  glfwTerminate();
  FreeImage_DeInitialise();

  return EXIT_SUCCESS;
}

GLuint createTexture(GLuint texUnit, GLuint shader, string samplerName,
                     string imgDir, FREE_IMAGE_FORMAT imgType) {
  glActiveTexture(GL_TEXTURE0 + texUnit);

  FIBITMAP *texImage =
      FreeImage_ConvertTo24Bits(FreeImage_Load(imgType, imgDir.c_str()));

  GLuint tboTex;
  glGenTextures(1, &tboTex);
  glBindTexture(GL_TEXTURE_2D, tboTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(texImage),
               FreeImage_GetHeight(texImage), 0, GL_BGR, GL_UNSIGNED_BYTE,
               (void *)FreeImage_GetBits(texImage));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  uniTexBase = myGetUniformLocation(shader, samplerName);
  glUniform1i(uniTexBase, texUnit);

  // release
  FreeImage_Unload(texImage);

  return tboTex;
}

void computeMatricesFromInputs(mat4 &newProject, mat4 &newView) {
  // glfwGetTime is called only once, the first time this function is called
  static float lastTime = glfwGetTime();

  // Compute time difference between current and last frame
  float currentTime = glfwGetTime();
  float deltaTime = float(currentTime - lastTime);

  // Get mouse position
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  // std::cout << xpos << ", " << ypos << '\n';

  // Reset mouse position for next frame
  glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

  // Compute new orientation
  //因为事先一步固定光标在屏幕中心
  //所以 WINDOW_WIDTH/2.f - xpos 和 WINDOW_HEIGHT/2.f - ypos 成了移动量
  horizontalAngle += mouseSpeed * float(xpos - WINDOW_WIDTH / 2.f);
  verticalAngle += mouseSpeed * float(-ypos + WINDOW_HEIGHT / 2.f);

  // Direction : Spherical coordinates to Cartesian coordinates conversion
  vec3 direction =
      vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle),
           sin(verticalAngle) * sin(horizontalAngle));

  // Right vector
  vec3 right = vec3(cos(horizontalAngle - 3.14 / 2.f), 0.f,
                    sin(horizontalAngle - 3.14 / 2.f));

  // new up vector
  vec3 newUp = cross(right, direction);

  // Move forward
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    eyePoint += direction * deltaTime * speed;
  }
  // Move backward
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    eyePoint -= direction * deltaTime * speed;
  }
  // Strafe right
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    eyePoint += right * deltaTime * speed;
  }
  // Strafe left
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    eyePoint -= right * deltaTime * speed;
  }

  // float FoV = initialFoV;
  newProject =
      perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.f);
  // Camera matrix
  newView = lookAt(eyePoint, eyePoint + direction, newUp);

  // For the next frame, the "last time" will be "now"
  lastTime = currentTime;
}

void keyCallback(GLFWwindow *keyWnd, int key, int scancode, int action,
                 int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
    case GLFW_KEY_ESCAPE: {
      glfwSetWindowShouldClose(keyWnd, GLFW_TRUE);
      break;
    }
    case GLFW_KEY_F: {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
    }
    case GLFW_KEY_L: {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      break;
    }
    case GLFW_KEY_I: {
      std::cout << "eyePoint: " << to_string(eyePoint) << '\n';
      std::cout << "verticleAngle: " << fmod(verticalAngle, 6.28f) << ", "
                << "horizontalAngle: " << fmod(horizontalAngle, 6.28f) << endl;
      break;
    }
    default:
      break;
    }
  }
}

void initGL() { // Initialise GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
    exit(EXIT_FAILURE);
  }

  // without setting GLFW_CONTEXT_VERSION_MAJOR and _MINOR，
  // OpenGL 1.x will be used
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  // must be used if OpenGL version >= 3.0
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "With normal mapping",
                            NULL, NULL);

  if (window == NULL) {
    std::cout << "Failed to open GLFW window." << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  /* Initialize GLEW */
  // without this, glGenVertexArrays will report ERROR!
  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST); // must enable depth test!!
}

void initOthers() { FreeImage_Initialise(true); }

void initMatrices() {
  // transform matrix
  uniM = myGetUniformLocation(exeShader, "M");
  uniV = myGetUniformLocation(exeShader, "V");
  uniP = myGetUniformLocation(exeShader, "P");

  model = translate(mat4(1.f), vec3(0.f, 0.f, 0.f));
  view = lookAt(eyePoint,     // eye position
                eyeDirection, // look at
                up            // up
  );

  projection =
      perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.f);

  glUniformMatrix4fv(uniM, 1, GL_FALSE, value_ptr(model));
  glUniformMatrix4fv(uniV, 1, GL_FALSE, value_ptr(view));
  glUniformMatrix4fv(uniP, 1, GL_FALSE, value_ptr(projection));
}

void initLight() { // light
  uniLightColor = myGetUniformLocation(exeShader, "lightColor");
  glUniform3fv(uniLightColor, 1, value_ptr(lightColor));

  uniLightPosition = myGetUniformLocation(exeShader, "lightPosition");
  glUniform3fv(uniLightPosition, 1, value_ptr(lightPosition));

  uniLightPower = myGetUniformLocation(exeShader, "lightPower");
  glUniform1f(uniLightPower, lightPower);

  uniDiffuse = myGetUniformLocation(exeShader, "diffuseColor");
  glUniform3fv(uniDiffuse, 1, value_ptr(materialDiffuseColor));

  uniAmbient = myGetUniformLocation(exeShader, "ambientColor");
  glUniform3fv(uniAmbient, 1, value_ptr(materialAmbientColor));

  uniSpecular = myGetUniformLocation(exeShader, "specularColor");
  glUniform3fv(uniSpecular, 1, value_ptr(materialSpecularColor));
}

void initTexture() { // base texture
  tboBase =
      createTexture(10, exeShader, "texBase", "rock_basecolor.jpg", FIF_JPEG);

  // normal texture
  tboNormal =
      createTexture(11, exeShader, "texNormal", "rock_normal.jpg", FIF_JPEG);
}
