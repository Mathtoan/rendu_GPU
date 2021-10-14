#include <iostream>
#include <fstream>

#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GL/freeglut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int VBO, VAO, EBO;
int shaderProgram;

// Renvoi le contenu d'un fichier
std::string lit_fichier(const std::string& filename)
{
  std::ifstream ifs;
  ifs.open(filename.c_str());
  if (!ifs)
  {
    std::cout << "ERROR: FILE " << filename << " NOT FOUND" << std::endl;
  }
  return std::string((std::istreambuf_iterator<char>(ifs)),
      (std::istreambuf_iterator<char>()));
}

// Creation d'une image
void load_image(const std::string& filename, unsigned char*& image, int& w, int& h, bool& rgb)
{
  int comp;
  image = stbi_load(filename.c_str(), &w, &h, &comp, STBI_rgb_alpha);
  if(image == nullptr)
    std::cout << "ERROR: IMAGE " << filename << std::endl;

  if(comp == 3)
    rgb = true;
  else
  {
    rgb = false;
  }
}

// Creation texture et envoie sur GPU, bind la texture dans la machine d'état
void load_texture(std::string filename)
{
  unsigned int texture_id;
  unsigned char* im;
  int w;
  int h;
  bool rgb;
  load_image(filename, im, w, h, rgb);

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  if(rgb)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, im);
  else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, im);
  stbi_image_free(im);
}

// Creation d'un shader
// shader_source : code GLSL
// shader_type : enum représentant le type de shader (GL_VERTEX_SHADER ou GL_FRAGMENT_SHADER)
// Renvoie l'identifiant du shader
int compile_shader(const char* shader_source, int shader_type)
{
  int success;
  char log[128];

  // TODO:
  // Créer un shader vide -> glCreateShader(GLenum)
  GLint shader = glCreateShader(shader_type);
  
  // Mettre le code voulu dans le shader
  //  -> glShaderSource(GLuint, 1, const GLchar * const *, NULL);
  glShaderSource(shader, 1, &shader_source, NULL);
  // Compiler le shader -> glCompileShader(GLuint);
  glCompileShader(shader);

  int shader_id;
  // FIN TODO
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(shader_id, 128, nullptr, log);
    std::cout << "ERROR: SHADER COMPILATION FAILED\n" << log << std::endl;
  }
  return shader_id;
}

// Creation programme GPU (vertex + fragment)
// vertex_shader : Contenu du shader
// fragment_shader : Contenu du shader
// Renvoie l'identifiant du programme
int creation_programme(const std::string& vertex_shader, const std::string& fragment_shader)
{
  int success;
  char log[128];

  // TODO:
  // Récupérer le vertex et le fragment shader compilé (utilisez std::string::c_str)
  int vertshader = compile_shader(vertex_shader.c_str(), GL_VERTEX_SHADER);
  int fragshader = compile_shader(fragment_shader.c_str(), GL_FRAGMENT_SHADER);
  // Créer un nouveau programmme vide GPU -> glCreateProgram()
  GLint program  = glCreateProgram();
  // Lier les deux shader au programme GPU -> glAttachShader(GLuint, GLuint)
  glAttachShader(program, vertshader);
  glAttachShader(program, fragshader);
  // Lier le programme a la CG -> glGetLinkProgram()
  glLinkProgram(program);

  int shaderProgram;
  // FIN TODO

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 128, NULL, log);
    std::cout << "ERROR: SHADER LINKING FAILED\n" << log << std::endl;
  }

  // TODO
  // Supprimer les deux shaders -> glDeleteShader(GLuint)
  glDeleteShader(vertshader);
  glDeleteShader(fragshader);

  // FIN TODO

  return shaderProgram;
}

// Méthode d'initialisation pour afficher un carre qui recouvre la fenêtre
void init()
{
  // TODO :
  // Lire les fichiers contenant les programmes des shaders puis les utiliser pour créer le programme

  std::string vertex_shader = lit_fichier("/Users/Toan/TP-Rendu/rendu_GPU/color.vs");
  std::string fragment_shader = lit_fichier("/Users/Toan/TP-Rendu/rendu_GPU/color.fs");
  int shaderProgram = creation_programme(vertex_shader, fragment_shader);

  // Créer un tableau de float contenant les sommets à afficher

  float vertices[] = {
    1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
  };
  // Créer un ficher d'entier non signé contenant les indices de sommets
  unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
  };

  // Créer un VAO -> glGenVertexArrays(GLsizei, GLuint *)
  glGenVertexArrays(1, &VAO);

  // Créer un VBO puis un EBO -> glGenBuffers(GLsizei, GLuint *)
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  // Mettre le VAO en actif dans la machine d'état -> glBindVertexArray(GLuint)
  glBindVertexArray(VAO);
  // Remplir le VBO puis l'EBO en utilisant la machine d'état.
  //  -> glBindBuffer(GLenum, GLuint) et glBufferData(GLenum, GLsizeiptr, const GLvoid*, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  // Specifier comment parcourir les buffers crées (utilise le dernier VBO de type GL_ARRAY_BUFFER) :
  //  -> glVertexAttribPointer(GLuint, GLint, GLenum, GLboolean, glsizei, const void*)
  //      Pour l'indice, se référer au vertex shader !

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  // Activer notre tableau de vertices : glEnableVertexAttribArray(GLuint)
  //
  // Le VAO permet de ne pas répéter les étapes de lecture des buffers à chaque affichage !
  // Les VAO peuvent être obligatoires dans certains cas.

    glEnableVertexAttribArray(0);
  // END TODO


  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Pas nécessaire mais permet de contrôler un peu ce qui est fait...
  // Globalement tous les GlEnable...
  // Décommenter et essayer de comprendre
  //glEnable(GL_CULL_FACE);
  //glFrontFace(GL_CCW);
  //glCullFace(GL_FRONT);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Boucle d'affichage
static void display_callback()
{
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // TODO :
  // Specifier le programme -> glUseProgram(GLuint)
  glUseProgram(shaderProgram);
  // Specifier le VAO à utiliser -> glBindVertexArray(GLuint)
  glBindVertexArray(VAO);
  // Demander affichage -> glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  // END TODO
  glBindVertexArray(0);
  glutSwapBuffers ();
  // Pour afficher de nouveau
  glutPostRedisplay();
}

static void keyboard_callback(unsigned char key, int, int)
{
  switch (key)
  {
    case 'q':
    case 'Q':
    case 27:
              exit(0);
  }
}

int main(int argc, char** argv)
{
  glutInitContextVersion(3, 3);
  glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("opengl");
  glutDisplayFunc(display_callback);
  glutKeyboardFunc(keyboard_callback);
  glewExperimental=true;
  glewInit();

  init();

  glutMainLoop();

  return 0;
}
