#include "common.h"
#include <glm/gtx/string_cast.hpp>

GLFWwindow *window;

vec3 lightPosition = vec3(3.f, 3.f, 3.f);
vec3 lightColor = vec3(1.f, 1.f, 1.f);
float lightPower = 12.f;

vec3 materialDiffuseColor = vec3(0.1f, 0.1f, 0.1f);
vec3 materialAmbientColor = vec3(0.1f, 0.1f, 0.1f);
vec3 materialSpecularColor = vec3(1.f, 1.f, 1.f);

float verticalAngle = -1.775f;
float horizontalAngle = 0.935f;
float initialFoV = 45.0f;
float speed = 5.0f;
float mouseSpeed = 0.005f;

mat4 model, view, projection;
vec3 eyePoint = vec3(2.f, 1.2f, -0.8f);
vec3 eyeDirection =
    vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle),
         sin(verticalAngle) * sin(horizontalAngle));
vec3 up = vec3(0.f, 1.f, 0.f);

GLuint vboVertexCoords, vboTextureCoords, vboNormalCoords;
GLuint vao;
GLuint textureObject;
GLint uniform_M, uniform_V, uniform_P, uniform_mvp;
GLint uniform_lightColor, uniform_lightPosition, uniform_lightPower;
// material diffuse, ambient, specular color
GLint uniform_diffuseColor, uniform_ambientColor, uniform_specularColor;
GLint uniform_tex;

void computeMatricesFromInputs(mat4 &, mat4 &);
void keyCallback(GLFWwindow *, int, int, int, int);

int main(int argc, char **argv) {
  // Initialise GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
    return -1;
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
  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                            "GLFW window with AntTweakBar", NULL, NULL);

  if (window == NULL) {
    std::cout << "Failed to open GLFW window." << std::endl;
    glfwTerminate();
    return -1;
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
    return -1;
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST); // must enable depth test!!

  FreeImage_Initialise(true);

  /* compile and link shaders */
  // must compile and link shaders BEFORE initializing AntTweakBar!
  GLuint vs = create_shader("vertex_shader.glsl", GL_VERTEX_SHADER);
  GLuint fs = create_shader("fragment_shader.glsl", GL_FRAGMENT_SHADER);

  GLuint program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);
  GLint link_ok;
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);

  if (link_ok == GL_FALSE) {
    std::cout << "Link failed." << std::endl;
  }

  glUseProgram(program);

  // load mesh
  mesh_info_t mesh = load_obj("cube.obj");
  // mesh_info_t mesh = load_obj("stone_monkey.obj");
  // mesh_info_t mesh = load_obj("sand_pool.obj");

  // write vertex coordinate to array
  FACE_INFO &faces = mesh.faceInfos[0];
  int faceNumber = faces.size();

  // every face includes 3 vertices, so faceNumber*3
  // every vertex coord includes 3 components, so faceNumber*3*3
  GLfloat *vertex_coords = new GLfloat[faceNumber * 3 * 3];
  GLfloat *texture_coords = new GLfloat[faceNumber * 3 * 2];
  GLfloat *normal_coords = new GLfloat[faceNumber * 3 * 3];

  for (size_t i = 0; i < faceNumber; i++) {
    // vertex coords
    for (size_t j = 0; j < 3; j++) {
      int idxVertex = faces[i].vertexIndices[j];
      vertex_coords[i * 9 + j * 3 + 0] = mesh.vertexCoords[idxVertex].x;
      vertex_coords[i * 9 + j * 3 + 1] = mesh.vertexCoords[idxVertex].y;
      vertex_coords[i * 9 + j * 3 + 2] = mesh.vertexCoords[idxVertex].z;
    }

    // texture coords
    for (size_t j = 0; j < 3; j++) {
      int idxTexture = faces[i].textureCoordIndices[j];
      texture_coords[i * 6 + j * 2 + 0] = mesh.textureCoords[idxTexture].x;
      texture_coords[i * 6 + j * 2 + 1] = mesh.textureCoords[idxTexture].y;
    }

    // normals
    for (size_t j = 0; j < 3; j++) {
      int idxNormal = faces[i].normalIndices[j];
      normal_coords[i * 9 + j * 3 + 0] = mesh.vertexNormals[idxNormal].x;
      normal_coords[i * 9 + j * 3 + 1] = mesh.vertexNormals[idxNormal].y;
      normal_coords[i * 9 + j * 3 + 2] = mesh.vertexNormals[idxNormal].z;
    }
  }

  // vao
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // vbo for vertex
  glGenBuffers(1, &vboVertexCoords);
  glBindBuffer(GL_ARRAY_BUFFER, vboVertexCoords);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * faceNumber * 3 * 3,
               vertex_coords, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // vbo for texture
  glGenBuffers(1, &vboTextureCoords);
  glBindBuffer(GL_ARRAY_BUFFER, vboTextureCoords);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * faceNumber * 3 * 2,
               texture_coords, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  // vbo for normal
  glGenBuffers(1, &vboNormalCoords);
  glBindBuffer(GL_ARRAY_BUFFER, vboNormalCoords);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * faceNumber * 3 * 3,
               normal_coords, GL_STATIC_DRAW);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);

  // texture
  // FIBITMAP* textureImage = FreeImage_Load(FIF_PNG, "ground.png");
  FIBITMAP *textureImage =
      FreeImage_ConvertTo24Bits(FreeImage_Load(FIF_JPEG, "rock_basecolor.jpg"));

  GLuint activeTex = 10;

  glGenTextures(1, &textureObject);
  glBindTexture(GL_TEXTURE_2D, textureObject);
  glActiveTexture(GL_TEXTURE0 + activeTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(textureImage),
               FreeImage_GetHeight(textureImage), 0, GL_BGR, GL_UNSIGNED_BYTE,
               (void *)FreeImage_GetBits(textureImage));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  uniform_tex = myGetUniformLocation(program, "tex");
  glUniform1i(uniform_tex, activeTex);

  // transform matrix
  uniform_M = myGetUniformLocation(program, "M");
  uniform_V = myGetUniformLocation(program, "V");
  uniform_P = myGetUniformLocation(program, "P");

  // light
  uniform_lightColor = myGetUniformLocation(program, "lightColor");
  glUniform3fv(uniform_lightColor, 1, value_ptr(lightColor));

  uniform_lightPosition = myGetUniformLocation(program, "lightPosition");
  glUniform3fv(uniform_lightPosition, 1, value_ptr(lightPosition));

  uniform_lightPower = myGetUniformLocation(program, "lightPower");
  glUniform1f(uniform_lightPower, lightPower);

  uniform_diffuseColor = myGetUniformLocation(program, "diffuseColor");
  glUniform3fv(uniform_diffuseColor, 1, value_ptr(materialDiffuseColor));

  uniform_ambientColor = myGetUniformLocation(program, "ambientColor");
  glUniform3fv(uniform_ambientColor, 1, value_ptr(materialAmbientColor));

  uniform_specularColor = myGetUniformLocation(program, "specularColor");
  glUniform3fv(uniform_specularColor, 1, value_ptr(materialSpecularColor));

  mat4 model = translate(mat4(1.f), vec3(0.f, 0.f, -4.f));
  mat4 view = lookAt(vec3(0.f, 1.5f, 0.f), // eye position
                     vec3(0.f, 0.f, -4.f), // look at
                     vec3(0.f, 1.f, 0.f)   // up
  );

  mat4 projection =
      perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 10.f);

  glUniformMatrix4fv(uniform_M, 1, GL_FALSE, value_ptr(model));
  glUniformMatrix4fv(uniform_V, 1, GL_FALSE, value_ptr(view));
  glUniformMatrix4fv(uniform_P, 1, GL_FALSE, value_ptr(projection));

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClearColor(0.f, 0.f, 0.4f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // view control
    computeMatricesFromInputs(projection, view);
    glUniformMatrix4fv(uniform_V, 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(uniform_P, 1, GL_FALSE, value_ptr(projection));

    // draw 3d model
    glDrawArrays(GL_TRIANGLES, 0, faceNumber * 3);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  delete[] vertex_coords;
  delete[] texture_coords;
  delete[] normal_coords;

  glfwTerminate();
  FreeImage_DeInitialise();

  return EXIT_SUCCESS;
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
      perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
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
      std::cout << "eyePoint: " << glm::to_string(eyePoint) << '\n';
      std::cout << "verticleAngle: " << fmod(verticalAngle, 6.28f) << ", "
                << "horizontalAngle: " << fmod(horizontalAngle, 6.28f) << endl;
      break;
    }
    default:
      break;
    }
  }
}
