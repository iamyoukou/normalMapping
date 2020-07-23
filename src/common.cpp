#include "common.h"

std::string readFile(const std::string fileName) {
  std::ifstream in;
  in.open(fileName.c_str());
  std::stringstream ss;
  ss << in.rdbuf();
  std::string sOut = ss.str();
  in.close();

  return sOut;
}

// return a shader executable
GLuint buildShader(string vsDir, string fsDir) {
  GLuint vs, fs;
  GLint linkOk;
  GLuint exeShader;

  // compile
  vs = compileShader(vsDir, GL_VERTEX_SHADER);
  fs = compileShader(fsDir, GL_FRAGMENT_SHADER);

  // link
  exeShader = linkShader(vs, fs);

  return exeShader;
}

GLuint compileShader(string fileName, GLenum type) {
  /* read source code */
  string sTemp = readFile(fileName);
  string info;
  const GLchar *source = sTemp.c_str();

  switch (type) {
  case GL_VERTEX_SHADER:
    info = "Vertex";
    break;
  case GL_FRAGMENT_SHADER:
    info = "Fragment";
    break;
  }

  if (source == NULL) {
    std::cout << info << " Shader : Can't read shader source file."
              << std::endl;
    return 0;
  }

  const GLchar *sources[] = {source};
  GLuint objShader = glCreateShader(type);
  glShaderSource(objShader, 1, sources, NULL);
  glCompileShader(objShader);

  GLint compile_ok;
  glGetShaderiv(objShader, GL_COMPILE_STATUS, &compile_ok);
  if (compile_ok == GL_FALSE) {
    std::cout << info << " Shader : Fail to compile." << std::endl;
    printLog(objShader);
    glDeleteShader(objShader);
    return 0;
  }

  return objShader;
}

GLuint linkShader(GLuint vsObj, GLuint fsObj) {
  GLuint exe;
  GLint linkOk;

  exe = glCreateProgram();
  glAttachShader(exe, vsObj);
  glAttachShader(exe, fsObj);
  glLinkProgram(exe);

  // check result
  glGetProgramiv(exe, GL_LINK_STATUS, &linkOk);

  if (linkOk == GL_FALSE) {
    std::cout << "Failed to link shader program." << std::endl;
    printLog(exe);
    glDeleteProgram(exe);

    return 0;
  }

  return exe;
}

void printLog(GLuint &object) {
  GLint log_length = 0;
  if (glIsShader(object)) {
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  } else if (glIsProgram(object)) {
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
  } else {
    cerr << "printlog: Not a shader or a program" << endl;
    return;
  }

  char *log = (char *)malloc(log_length);

  if (glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if (glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);

  cerr << log << endl;
  free(log);
}

GLint myGetUniformLocation(GLuint &prog, string name) {
  GLint location;
  location = glGetUniformLocation(prog, name.c_str());
  if (location == -1) {
    cerr << "Could not bind uniform : " << name << ". "
         << "Did you set the right name? "
         << "Or is " << name << " not used?" << endl;
  }

  return location;
}

// Whenever the vertex attributes have been changed, call this function
// Otherwise, the vertex data on the server side will not be updated
// void updateMesh(Mesh &mesh) {
//   // write vertex coordinate to array
//   int nOfFaces = mesh.faces.size();
//
//   // 3 vertices per face, 3 float per vertex coord, 2 float per tex coord
//   GLfloat *aVtxCoords = new GLfloat[nOfFaces * 3 * 3];
//   // GLfloat *aUvs = new GLfloat[nOfFaces * 3 * 2];
//   // GLfloat *aNormals = new GLfloat[nOfFaces * 3 * 3];
//
//   for (size_t i = 0; i < nOfFaces; i++) {
//     // vertex 1
//     int vtxIdx = mesh.faces[i].v1;
//     aVtxCoords[i * 9 + 0] = mesh.vertices[vtxIdx].x;
//     aVtxCoords[i * 9 + 1] = mesh.vertices[vtxIdx].y;
//     aVtxCoords[i * 9 + 2] = mesh.vertices[vtxIdx].z;
//
//     // normal for vertex 1
//     // int nmlIdx = mesh.faces[i].vn1;
//     // aNormals[i * 9 + 0] = mesh.faceNormals[nmlIdx].x;
//     // aNormals[i * 9 + 1] = mesh.faceNormals[nmlIdx].y;
//     // aNormals[i * 9 + 2] = mesh.faceNormals[nmlIdx].z;
//
//     // uv for vertex 1
//     // int uvIdx = mesh.faces[i].vt1;
//     // aUvs[i * 6 + 0] = mesh.uvs[uvIdx].x;
//     // aUvs[i * 6 + 1] = mesh.uvs[uvIdx].y;
//
//     // vertex 2
//     vtxIdx = mesh.faces[i].v2;
//     aVtxCoords[i * 9 + 3] = mesh.vertices[vtxIdx].x;
//     aVtxCoords[i * 9 + 4] = mesh.vertices[vtxIdx].y;
//     aVtxCoords[i * 9 + 5] = mesh.vertices[vtxIdx].z;
//
//     // normal for vertex 2
//     // nmlIdx = mesh.faces[i].vn2;
//     // aNormals[i * 9 + 3] = mesh.faceNormals[nmlIdx].x;
//     // aNormals[i * 9 + 4] = mesh.faceNormals[nmlIdx].y;
//     // aNormals[i * 9 + 5] = mesh.faceNormals[nmlIdx].z;
//
//     // uv for vertex 2
//     // uvIdx = mesh.faces[i].vt2;
//     // aUvs[i * 6 + 2] = mesh.uvs[uvIdx].x;
//     // aUvs[i * 6 + 3] = mesh.uvs[uvIdx].y;
//
//     // vertex 3
//     vtxIdx = mesh.faces[i].v3;
//     aVtxCoords[i * 9 + 6] = mesh.vertices[vtxIdx].x;
//     aVtxCoords[i * 9 + 7] = mesh.vertices[vtxIdx].y;
//     aVtxCoords[i * 9 + 8] = mesh.vertices[vtxIdx].z;
//
//     // normal for vertex 3
//     // nmlIdx = mesh.faces[i].vn3;
//     // aNormals[i * 9 + 6] = mesh.faceNormals[nmlIdx].x;
//     // aNormals[i * 9 + 7] = mesh.faceNormals[nmlIdx].y;
//     // aNormals[i * 9 + 8] = mesh.faceNormals[nmlIdx].z;
//
//     // uv for vertex 3
//     // uvIdx = mesh.faces[i].vt3;
//     // aUvs[i * 6 + 4] = mesh.uvs[uvIdx].x;
//     // aUvs[i * 6 + 5] = mesh.uvs[uvIdx].y;
//   }
//
//   // vao
//   glBindVertexArray(mesh.vao);
//
//   // vbo for vertex
//   glBindBuffer(GL_ARRAY_BUFFER, mesh.vboVtxs);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 3 * 3,
//   aVtxCoords,
//                GL_STATIC_DRAW);
//
//   // vbo for texture
//   // glBindBuffer(GL_ARRAY_BUFFER, mesh.vboUvs);
//   // glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 3 * 2, aUvs,
//   //              GL_STATIC_DRAW);
//
//   // vbo for normal
//   // glBindBuffer(GL_ARRAY_BUFFER, mesh.vboNormals);
//   // glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 3 * 3,
//   aNormals,
//   //              GL_STATIC_DRAW);
//
//   // delete client data
//   delete[] aVtxCoords;
//   // delete[] aUvs;
//   // delete[] aNormals;
// }

void drawBox(vec3 min, vec3 max) {
  // 8 corners
  GLfloat aVtxs[]{
      min.x, max.y, min.z, // 0
      min.x, min.y, min.z, // 1
      max.x, min.y, min.z, // 2
      max.x, max.y, min.z, // 3
      min.x, max.y, max.z, // 4
      min.x, min.y, max.z, // 5
      max.x, min.y, max.z, // 6
      max.x, max.y, max.z  // 7
  };

  // vertex color
  // GLfloat colorArray[] = {color.x, color.y, color.z, color.x, color.y,
  // color.z,
  //                         color.x, color.y, color.z, color.x, color.y,
  //                         color.z, color.x, color.y, color.z, color.x,
  //                         color.y, color.z, color.x, color.y, color.z,
  //                         color.x, color.y, color.z};

  // vertex index
  GLushort aIdxs[] = {
      0, 1, 2, 3, // front face
      4, 7, 6, 5, // back
      4, 0, 3, 7, // up
      5, 6, 2, 1, // down
      0, 4, 5, 1, // left
      3, 2, 6, 7  // right
  };

  // prepare buffers to draw
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vboVtx;
  glGenBuffers(1, &vboVtx);
  glBindBuffer(GL_ARRAY_BUFFER, vboVtx);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8 * 3, aVtxs, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // GLuint vboColor;
  // glGenBuffers(1, &vboColor);
  // glBindBuffer(GL_ARRAY_BUFFER, vboColor);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(colorArray), colorArray,
  // GL_STATIC_DRAW); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  // glEnableVertexAttribArray(1);

  GLuint ibo;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(aIdxs), aIdxs, GL_STATIC_DRAW);

  // draw box
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  for (size_t i = 0; i < 6; i++) {
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT,
                   (GLvoid *)(sizeof(GLushort) * 4 * i));
  }

  glDeleteBuffers(1, &vboVtx);
  // glDeleteBuffers(1, &vboColor);
  glDeleteBuffers(1, &ibo);
  glDeleteVertexArrays(1, &vao);
}

/* Mesh class */
Mesh::Mesh(const string fileName) {
  loadObj(fileName);
  initBuffers();
  initShader();
  initUniform();
}

Mesh::~Mesh() {
  glDeleteBuffers(1, &vboVtxs);
  glDeleteBuffers(1, &vboUvs);
  glDeleteBuffers(1, &vboNormals);
  glDeleteVertexArrays(1, &vao);
}

void Mesh::initShader() {
  shader = buildShader("./shader/vsPhong.glsl", "./shader/fsPhong.glsl");
}

void Mesh::initUniform() {
  uniModel = myGetUniformLocation(shader, "M");
  uniView = myGetUniformLocation(shader, "V");
  uniProjection = myGetUniformLocation(shader, "P");
  uniEyePoint = myGetUniformLocation(shader, "eyePoint");
  uniLightColor = myGetUniformLocation(shader, "lightColor");
  uniLightPosition = myGetUniformLocation(shader, "lightPosition");
  uniTexBase = myGetUniformLocation(shader, "texBase");
  uniTexNormal = myGetUniformLocation(shader, "texNormal");
}

void Mesh::loadObj(const string fileName) {
  std::ifstream fin;
  fin.open(fileName.c_str());

  if (!(fin.good())) {
    std::cout << "failed to open file : " << fileName << std::endl;
  }

  while (fin.peek() != EOF) { // read obj loop
    std::string s;
    fin >> s;

    // vertex coordinate
    if ("v" == s) {
      float x, y, z;
      fin >> x;
      fin >> y;
      fin >> z;
      vertices.push_back(glm::vec3(x, y, z));
    }
    // texture coordinate
    else if ("vt" == s) {
      float u, v;
      fin >> u;
      fin >> v;
      uvs.push_back(glm::vec2(u, v));
    }
    // face normal (recorded as vn in obj file)
    else if ("vn" == s) {
      float x, y, z;
      fin >> x;
      fin >> y;
      fin >> z;
      faceNormals.push_back(glm::vec3(x, y, z));
    }
    // vertices contained in face, and face normal
    else if ("f" == s) {
      Face f;

      // v1/vt1/vn1
      fin >> f.v1;
      fin.ignore(1);
      fin >> f.vt1;
      fin.ignore(1);
      fin >> f.vn1;

      // v2/vt2/vn2
      fin >> f.v2;
      fin.ignore(1);
      fin >> f.vt2;
      fin.ignore(1);
      fin >> f.vn2;

      // v3/vt3/vn3
      fin >> f.v3;
      fin.ignore(1);
      fin >> f.vt3;
      fin.ignore(1);
      fin >> f.vn3;

      // Note:
      //  v, vt, vn in "v/vt/vn" start from 1,
      //  but indices of std::vector start from 0,
      //  so we need minus 1 for all elements
      f.v1 -= 1;
      f.vt1 -= 1;
      f.vn1 -= 1;

      f.v2 -= 1;
      f.vt2 -= 1;
      f.vn2 -= 1;

      f.v3 -= 1;
      f.vt3 -= 1;
      f.vn3 -= 1;

      faces.push_back(f);
    } else {
      continue;
    }
  } // end read obj loop

  fin.close();
}

void Mesh::initBuffers() {
  // write vertex coordinate to array
  int nOfFaces = faces.size();

  // 3 vertices per face, 3 float per vertex coord, 2 float per tex coord
  GLfloat *aVtxCoords = new GLfloat[nOfFaces * 3 * 3];
  GLfloat *aUvs = new GLfloat[nOfFaces * 3 * 2];
  GLfloat *aNormals = new GLfloat[nOfFaces * 3 * 3];

  for (size_t i = 0; i < nOfFaces; i++) {
    // vertex 1
    int vtxIdx = faces[i].v1;
    aVtxCoords[i * 9 + 0] = vertices[vtxIdx].x;
    aVtxCoords[i * 9 + 1] = vertices[vtxIdx].y;
    aVtxCoords[i * 9 + 2] = vertices[vtxIdx].z;

    // normal for vertex 1
    int nmlIdx = faces[i].vn1;
    aNormals[i * 9 + 0] = faceNormals[nmlIdx].x;
    aNormals[i * 9 + 1] = faceNormals[nmlIdx].y;
    aNormals[i * 9 + 2] = faceNormals[nmlIdx].z;

    // uv for vertex 1
    int uvIdx = faces[i].vt1;
    aUvs[i * 6 + 0] = uvs[uvIdx].x;
    aUvs[i * 6 + 1] = uvs[uvIdx].y;

    // vertex 2
    vtxIdx = faces[i].v2;
    aVtxCoords[i * 9 + 3] = vertices[vtxIdx].x;
    aVtxCoords[i * 9 + 4] = vertices[vtxIdx].y;
    aVtxCoords[i * 9 + 5] = vertices[vtxIdx].z;

    // normal for vertex 2
    nmlIdx = faces[i].vn2;
    aNormals[i * 9 + 3] = faceNormals[nmlIdx].x;
    aNormals[i * 9 + 4] = faceNormals[nmlIdx].y;
    aNormals[i * 9 + 5] = faceNormals[nmlIdx].z;

    // uv for vertex 2
    uvIdx = faces[i].vt2;
    aUvs[i * 6 + 2] = uvs[uvIdx].x;
    aUvs[i * 6 + 3] = uvs[uvIdx].y;

    // vertex 3
    vtxIdx = faces[i].v3;
    aVtxCoords[i * 9 + 6] = vertices[vtxIdx].x;
    aVtxCoords[i * 9 + 7] = vertices[vtxIdx].y;
    aVtxCoords[i * 9 + 8] = vertices[vtxIdx].z;

    // normal for vertex 3
    nmlIdx = faces[i].vn3;
    aNormals[i * 9 + 6] = faceNormals[nmlIdx].x;
    aNormals[i * 9 + 7] = faceNormals[nmlIdx].y;
    aNormals[i * 9 + 8] = faceNormals[nmlIdx].z;

    // uv for vertex 3
    uvIdx = faces[i].vt3;
    aUvs[i * 6 + 4] = uvs[uvIdx].x;
    aUvs[i * 6 + 5] = uvs[uvIdx].y;
  }

  // vao
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // vbo for vertex
  glGenBuffers(1, &vboVtxs);
  glBindBuffer(GL_ARRAY_BUFFER, vboVtxs);
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

  // delete client data
  delete[] aVtxCoords;
  delete[] aUvs;
  delete[] aNormals;
}

void Mesh::setTexture(GLuint &tbo, int texUnit, const string texDir,
                      FREE_IMAGE_FORMAT imgType) {
  glActiveTexture(GL_TEXTURE0 + texUnit);

  FIBITMAP *texImage =
      FreeImage_ConvertTo24Bits(FreeImage_Load(imgType, texDir.c_str()));

  glGenTextures(1, &tbo);
  glBindTexture(GL_TEXTURE_2D, tbo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(texImage),
               FreeImage_GetHeight(texImage), 0, GL_BGR, GL_UNSIGNED_BYTE,
               (void *)FreeImage_GetBits(texImage));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // release
  FreeImage_Unload(texImage);
}

void Mesh::draw(mat4 M, mat4 V, mat4 P, vec3 eye, vec3 lightColor,
                vec3 lightPosition, int unitBaseColor, int unitNormal) {
  glUseProgram(shader);

  glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(M));
  glUniformMatrix4fv(uniView, 1, GL_FALSE, value_ptr(V));
  glUniformMatrix4fv(uniProjection, 1, GL_FALSE, value_ptr(P));

  glUniform3fv(uniEyePoint, 1, value_ptr(eye));

  glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
  glUniform3fv(uniLightPosition, 1, value_ptr(lightPosition));

  glUniform1i(uniTexBase, unitBaseColor); // change base color
  glUniform1i(uniTexNormal, unitNormal);  // change normal

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, faces.size() * 3);
}

void Mesh::translate(glm::vec3 xyz) {
  // move each vertex with xyz
  for (size_t i = 0; i < vertices.size(); i++) {
    vertices[i] += xyz;
  }

  // update aabb
  min += xyz;
  max += xyz;
}

void Mesh::scale(glm::vec3 xyz) {
  // scale each vertex with xyz
  for (size_t i = 0; i < vertices.size(); i++) {
    vertices[i].x *= xyz.x;
    vertices[i].y *= xyz.y;
    vertices[i].z *= xyz.z;
  }

  // update aabb
  min.x *= xyz.x;
  min.y *= xyz.y;
  min.z *= xyz.z;

  max.x *= xyz.x;
  max.y *= xyz.y;
  max.z *= xyz.z;
}

// rotate mesh along x, y, z axes
// xyz specifies the rotated angle along each axis
void Mesh::rotate(glm::vec3 xyz) {}

void Mesh::findAABB() {
  int nOfVtxs = vertices.size();
  vec3 min(0, 0, 0), max(0, 0, 0);

  for (size_t i = 0; i < nOfVtxs; i++) {
    vec3 vtx = vertices[i];

    // x
    if (vtx.x > max.x) {
      max.x = vtx.x;
    }
    if (vtx.x < min.x) {
      min.x = vtx.x;
    }
    // y
    if (vtx.y > max.y) {
      max.y = vtx.y;
    }
    if (vtx.y < min.y) {
      min.y = vtx.y;
    }
    // z
    if (vtx.z > max.z) {
      max.z = vtx.z;
    }
    if (vtx.z < min.z) {
      min.z = vtx.z;
    }
  }

  min = min;
  max = max;
}

void drawPoints(vector<Point> &pts) { // array data
  int nOfPs = pts.size();

  GLfloat *aPos = new GLfloat[nOfPs * 3];
  GLfloat *aColor = new GLfloat[nOfPs * 3];

  // implant data
  for (size_t i = 0; i < nOfPs; i++) {
    // positions
    Point &p = pts[i];
    aPos[i * 3 + 0] = p.pos.x;
    aPos[i * 3 + 1] = p.pos.y;
    aPos[i * 3 + 2] = p.pos.z;

    // colors
    aColor[i * 3 + 0] = p.color.r;
    aColor[i * 3 + 1] = p.color.g;
    aColor[i * 3 + 2] = p.color.b;
  }

  // selete vao
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // position
  GLuint vboPos;
  glGenBuffers(1, &vboPos);
  glBindBuffer(GL_ARRAY_BUFFER, vboPos);
  glBufferData(GL_ARRAY_BUFFER, nOfPs * 3 * sizeof(GLfloat), aPos,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // color
  GLuint vboColor;
  glGenBuffers(1, &vboColor);
  glBindBuffer(GL_ARRAY_BUFFER, vboColor);
  glBufferData(GL_ARRAY_BUFFER, nOfPs * 3 * sizeof(GLfloat), aColor,
               GL_STREAM_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  glDrawArrays(GL_POINTS, 0, nOfPs);

  // release
  delete[] aPos;
  delete[] aColor;
  glDeleteBuffers(1, &vboPos);
  glDeleteBuffers(1, &vboColor);
  glDeleteVertexArrays(1, &vao);
}

Quad::Quad() {
  initData();
  initBuffers();
  initShader();
  initUniform();
}

Quad::~Quad() {}

void Quad::initData() {
  // vertices
  vtxs.push_back(vec3(-1.0f, 1.0f, 0.0f));
  vtxs.push_back(vec3(-1.0f, -1.0f, 0.0f));
  vtxs.push_back(vec3(1.0f, -1.0f, 0.0f));
  vtxs.push_back(vec3(1.0f, 1.0f, 0.0f));

  // uvs
  uvs.push_back(vec2(0.0f, 1.0f));
  uvs.push_back(vec2(0.0f, 0.0f));
  uvs.push_back(vec2(1.0f, 0.0f));
  uvs.push_back(vec2(1.0f, 1.0f));

  // normals
  nms.push_back(vec3(0.0f, 0.0f, 1.0f));
  nms.push_back(vec3(0.0f, 0.0f, 1.0f));
  nms.push_back(vec3(0.0f, 0.0f, 1.0f));
  nms.push_back(vec3(0.0f, 0.0f, 1.0f));

  // tangent for two triangles
  // triangle 1
  // ----------
  vec3 edge1 = vtxs[1] - vtxs[0];
  vec3 edge2 = vtxs[2] - vtxs[0];
  vec2 deltaUV1 = uvs[1] - uvs[0];
  vec2 deltaUV2 = uvs[2] - uvs[0];

  float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

  vec3 tangent1, bitangent1;

  tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
  tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
  tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
  tangent1 = normalize(tangent1);
  tangents.push_back(tangent1);

  bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
  bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
  bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
  bitangent1 = normalize(bitangent1);
  bitangents.push_back(bitangent1);

  // triangle 2
  // ----------
  edge1 = vtxs[2] - vtxs[0];
  edge2 = vtxs[3] - vtxs[0];
  deltaUV1 = uvs[2] - uvs[0];
  deltaUV2 = uvs[3] - uvs[0];

  f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

  vec3 tangent2, bitangent2;

  tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
  tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
  tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
  tangent2 = normalize(tangent2);
  tangents.push_back(tangent2);

  bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
  bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
  bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
  bitangent2 = normalize(bitangent2);
  bitangents.push_back(bitangent2);
}

void Quad::initShader() {
  shader = buildShader("./shader/vsPOM.glsl", "./shader/fsPOM.glsl");
}

void Quad::initUniform() {
  uniModel = myGetUniformLocation(shader, "M");
  uniView = myGetUniformLocation(shader, "V");
  uniProjection = myGetUniformLocation(shader, "P");
  uniEyePoint = myGetUniformLocation(shader, "eyePoint");
  uniLightColor = myGetUniformLocation(shader, "lightColor");
  uniLightPosition = myGetUniformLocation(shader, "lightPosition");
  uniTexBase = myGetUniformLocation(shader, "texBase");
  uniTexNormal = myGetUniformLocation(shader, "texNormal");
  uniTexHeight = myGetUniformLocation(shader, "texHeight");
}

void Quad::initBuffers() {
  GLfloat aVtxCoords[18] = {
      // triangle1
      vtxs[0].x,
      vtxs[0].y,
      vtxs[0].z,
      vtxs[1].x,
      vtxs[1].y,
      vtxs[1].z,
      vtxs[2].x,
      vtxs[2].y,
      vtxs[2].z,
      // triangle 2
      vtxs[0].x,
      vtxs[0].y,
      vtxs[0].z,
      vtxs[2].x,
      vtxs[2].y,
      vtxs[2].z,
      vtxs[3].x,
      vtxs[3].y,
      vtxs[3].z,
  };

  GLfloat aUvs[12] = {
      // triangel 1
      uvs[0].x, uvs[0].y, uvs[1].x, uvs[1].y, uvs[2].x, uvs[2].y,
      // triangle 2
      uvs[0].x, uvs[0].y, uvs[2].x, uvs[2].y, uvs[3].x, uvs[3].y};

  GLfloat aNormals[18] = {
      // triangle1
      nms[0].x,
      nms[0].y,
      nms[0].z,
      nms[1].x,
      nms[1].y,
      nms[1].z,
      nms[2].x,
      nms[2].y,
      nms[2].z,
      // triangle 2
      nms[0].x,
      nms[0].y,
      nms[0].z,
      nms[2].x,
      nms[2].y,
      nms[2].z,
      nms[3].x,
      nms[3].y,
      nms[3].z,
  };

  GLfloat aTangents[6] = {// triangle 1
                          tangents[0].x, tangents[0].y, tangents[0].z,
                          // triangle 2
                          tangents[1].x, tangents[1].y, tangents[1].z};
  GLfloat aBitangents[6] = {// triangle 1
                            bitangents[0].x, bitangents[0].y, bitangents[0].z,
                            // triangle 2
                            bitangents[1].x, bitangents[1].y, bitangents[1].z};

  // vao
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // vbo for vertex
  glGenBuffers(1, &vboVtxs);
  glBindBuffer(GL_ARRAY_BUFFER, vboVtxs);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 18, aVtxCoords,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // vbo for texture
  glGenBuffers(1, &vboUvs);
  glBindBuffer(GL_ARRAY_BUFFER, vboUvs);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, aUvs, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  // vbo for normal
  glGenBuffers(1, &vboNormals);
  glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 18, aNormals, GL_STATIC_DRAW);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);

  // vbo for tangents
  glGenBuffers(1, &vboTangents);
  glBindBuffer(GL_ARRAY_BUFFER, vboTangents);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6, aTangents, GL_STATIC_DRAW);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(3);

  // vbo for bitangents
  glGenBuffers(1, &vboBitangents);
  glBindBuffer(GL_ARRAY_BUFFER, vboBitangents);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6, aBitangents,
               GL_STATIC_DRAW);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(4);
}

void Quad::setTexture(GLuint &tbo, int texUnit, const string texDir,
                      FREE_IMAGE_FORMAT imgType) {
  glActiveTexture(GL_TEXTURE0 + texUnit);

  FIBITMAP *texImage =
      FreeImage_ConvertTo24Bits(FreeImage_Load(imgType, texDir.c_str()));

  glGenTextures(1, &tbo);
  glBindTexture(GL_TEXTURE_2D, tbo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(texImage),
               FreeImage_GetHeight(texImage), 0, GL_BGR, GL_UNSIGNED_BYTE,
               (void *)FreeImage_GetBits(texImage));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // release
  FreeImage_Unload(texImage);
}

void Quad::draw(mat4 M, mat4 V, mat4 P, vec3 eye, vec3 lightColor,
                vec3 lightPosition, int unitBaseColor, int unitNormal,
                int unitHeight) {
  glUseProgram(shader);

  glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(M));
  glUniformMatrix4fv(uniView, 1, GL_FALSE, value_ptr(V));
  glUniformMatrix4fv(uniProjection, 1, GL_FALSE, value_ptr(P));

  glUniform3fv(uniEyePoint, 1, value_ptr(eye));

  glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
  glUniform3fv(uniLightPosition, 1, value_ptr(lightPosition));

  glUniform1i(uniTexBase, unitBaseColor); // change base color
  glUniform1i(uniTexNormal, unitNormal);  // change normal
  glUniform1i(uniTexHeight, unitHeight);  // change height map

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}
