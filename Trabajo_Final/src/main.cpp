#include <iostream>
#include <ogl/glew.h>
#include <ogl/freeglut.h>

#include "Shaders.h"
#include "Model.h"
#include "Texture.h"

#include <sstream>
std::string toString(const int &i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
}

#define I glm::mat4(1.0)

void funInit();
void funReshape(int w, int h);
void funDisplay();
void funTimer(int value);
void zoom(int button, int state, int x, int y);
void funcamara(int x, int y);
void funKeyboard(unsigned char key, int x, int y);
void funSpecial(int key, int x, int y);

void setLights(glm::mat4 P, glm::mat4 V);
void drawObjectMat(Model model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawObjectTex(Model model, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M);

void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M);

// Shaders
Shaders shaders;

// Modelos
Model plane;
Model sphere;
Model cone;
Model cylinder;

// Texturas (imagenes)
Texture imgNoEmissive;
Texture img2;
Texture img3;
Texture img4;
Texture img5;
Texture img6;
Texture img7;

// Luces y materiales
#define   NLD 1
#define   NLP 1
#define   NLF 1
Light     lightG;
Light     lightD[NLD];
Light     lightP[NLP];
Light     lightF[NLF];

Material  gold;
Material  ruby;
Material  polishedbronze;
Material  cyan;
Material  emerald;


Textures  texSuelo;
Textures  texBase;
Textures  texLuz;

// Viewport
int w = 600;
int h = 600;

GLint speed = 20;
float rotT = 0;
float RotTiovivo = 0;
float RotBrazos = 0;
float TSuperior = 1.3;
float fov = 50.0;
float desX = 0.0;
float desZ = 0.0;
float alphaX=40;
float alphaY=25;
int oldX, oldY;
int tecla_pulsada = 0;
bool spulsado = TRUE;

int main(int argc, char** argv) {

    // Inicializamos GLUT
    glutInit(&argc, argv);
    glutInitContextVersion(3,3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(w,h);
    glutInitWindowPosition(50,50);
    glutCreateWindow("Practica 1");

    // Inicializamos GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        return 0;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    const GLubyte *oglVersion = glGetString(GL_VERSION);
    std::cout <<"This system supports OpenGL Version: " << oglVersion << std::endl;

    // Inicializaciones especÃ­ficas
    funInit();

    // ConfiguraciÃ³n CallBacks
    glutReshapeFunc(funReshape);
    glutDisplayFunc(funDisplay);
    glutTimerFunc(speed,funTimer,0);
    glutMouseFunc(zoom);
    glutMotionFunc(funcamara);
    glutKeyboardFunc(funKeyboard);
    glutSpecialFunc(funSpecial);

    // Bucle principal
    glutMainLoop();

    return 0;
}

void funInit() {
    // Test de profundidad
    glEnable(GL_DEPTH_TEST);
    // glPolygonOffset(1.0,1.0);

    // Transparencias
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Shaders
    shaders.initShaders("resources/shaders/vshader.glsl","resources/shaders/fshader.glsl");

    // Modelos
    plane.initModel("resources/models/plane.obj");
    sphere.initModel("resources/models/sphere.obj");
    cone.initModel("resources/models/cone.obj");
    cylinder.initModel("resources/models/cylinder.obj");

    // Texturas (imagenes)
    imgNoEmissive.initTexture("resources/textures/img1.png");
    img2.initTexture("resources/textures/img2.png");
    img3.initTexture("resources/textures/img3.png");
    img4.initTexture("resources/textures/img4.png");
    img5.initTexture("resources/textures/img5.png");
    img6.initTexture("resources/textures/img6.png");
    img7.initTexture("resources/textures/img7.png");

    // Luz ambiental global
    lightG.ambient        = glm::vec3(0.5, 0.5, 0.5);

    // Luces direccionales
    lightD[0].direction   = glm::vec3(0.0, -1.0, 0.0);
    lightD[0].ambient     = glm::vec3( 0.1, 0.1, 0.1);
    lightD[0].diffuse     = glm::vec3( 0.7, 0.7, 0.7);
    lightD[0].specular    = glm::vec3( 0.7, 0.7, 0.7);

    // Luces posicionales
    lightP[0].position    = glm::vec3(-1.5, 0.2, 0.0);
    lightP[0].ambient     = glm::vec3(0.2, 0.2, 0.2);
    lightP[0].diffuse     = glm::vec3(0.9, 0.9, 0.9);
    lightP[0].specular    = glm::vec3(0.9, 0.9, 0.9);
    lightP[0].c0          = 1.00;
    lightP[0].c1          = 0.22;
    lightP[0].c2          = 0.20;

    // Luces focales
    lightF[0].position    = glm::vec3(3.0,  3.0,  4.0);
    lightF[0].direction   = glm::vec3( 0.0, 0.0, 0.0);
    lightF[0].ambient     = glm::vec3( 0.2,  0.2,  0.2);
    lightF[0].diffuse     = glm::vec3( 0.9,  0.9,  0.9);
    lightF[0].specular    = glm::vec3( 0.9,  0.9,  0.9);
    lightF[0].innerCutOff = 15.0;
    lightF[0].outerCutOff = lightF[0].innerCutOff + 5.0;
    lightF[0].c0          = 1.000;
    lightF[0].c1          = 0.090;
    lightF[0].c2          = 0.032;

    // Materiales
    gold.ambient   = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
    gold.diffuse   = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f );
    gold.specular  = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
    gold.emissive  = glm::vec4(0, 0, 0, 0);
    gold.shininess = 30.0f;

    ruby.ambient   = glm::vec4(0.174500, 0.011750, 0.011750, 0.55);
    ruby.diffuse   = glm::vec4(0.614240, 0.041360, 0.041360, 0.55);
    ruby.specular  = glm::vec4(0.727811, 0.626959, 0.626959, 0.55);
    ruby.emissive  = glm::vec4(0.00, 0.00, 0.00, 0.00);
    ruby.shininess = 76.8;

    polishedbronze.ambient   = glm::vec4(0.25f, 0.148f, 0.06475f, 1.0f);
    polishedbronze.diffuse   = glm::vec4(0.4f, 0.2368f, 0.1036f, 1.0f);
    polishedbronze.specular  = glm::vec4(0.774597f, 0.458561f, 0.200621f, 1.0f);
    polishedbronze.emissive  = glm::vec4(1.0, 1.0, 1.0, 1.0);
    polishedbronze.shininess = 30.8f;

    polishedbronze.ambient   = glm::vec4(0.25f, 0.148f, 0.06475f, 1.0f);
    polishedbronze.diffuse   = glm::vec4(0.4f, 0.2368f, 0.1036f, 1.0f);
    polishedbronze.specular  = glm::vec4(0.774597f, 0.458561f, 0.200621f, 1.0f);
    polishedbronze.emissive  = glm::vec4(0.000000, 0.000000, 0.000000, 1.00);
    polishedbronze.shininess = 76.8f;

    cyan.ambient   = glm::vec4(0.0f,0.1f,0.06f ,1.0f);
    cyan.diffuse   = glm::vec4(0.0f,0.50980392f,0.50980392f,1.0f);
    cyan.specular  = glm::vec4(0.50196078f,0.50196078f,0.50196078f,1.0f);
    cyan.emissive  = glm::vec4(0.000000, 0.000000, 0.000000, 1.00);
    cyan.shininess = 32.0f;

    emerald.ambient   = glm::vec4(0.0215f, 0.1745f, 0.0215f, 0.55f);
    emerald.diffuse   = glm::vec4(0.07568f, 0.61424f, 0.07568f, 0.55f);
    emerald.specular  = glm::vec4(0.633f, 0.727811f, 0.633f, 0.55f);
    emerald.emissive  = glm::vec4(0.000000, 0.000000, 0.000000, 1.00);
    emerald.shininess = 30.0f;

    texSuelo.diffuse     = img5.getTexture();
    texSuelo.specular    = img3.getTexture();
    texSuelo.emissive    = img2.getTexture();
    texSuelo.normal      = img5.getTexture();;
    texSuelo.shininess   = 10.0;

}

void funReshape(int wnew, int hnew) {
    // ConfiguraciÃ³n del Viewport
    glViewport(0, 0, wnew, hnew);

    // ActualizaciÃ³n de w y h
    w = wnew;
    h = hnew;
}

void funDisplay() {
    // Borramos el buffer de color
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Indicamos los shaders a utilizar
    shaders.useShaders();

    // Matriz P
    float fovy   = fov;
    float nplane =  0.1;
    float fplane = 25.0;
    float aspect = (float)w/(float)h;
    glm::mat4 P  = glm::perspective(glm::radians(fovy), aspect, nplane, fplane);

    // Matriz V
    float x = 5.0f*glm::cos(glm::radians(alphaY))*glm::sin(glm::radians(alphaX));
    float y = 5.0f*glm::sin(glm::radians(alphaY));
    float z = 5.0f*glm::cos(glm::radians(alphaY))*glm::cos(glm::radians(alphaX));
    glm::vec3 pos   (x, y, z);
    //glm::vec3 pos   (4.0, 4.0, 4.0);
    glm::vec3 lookat(0.0, 0.0, 0.0);
    glm::vec3 up    (0.0, 1.0,  0.0);
    glm::mat4 V = glm::lookAt(pos, lookat, up);
    shaders.setVec3("ucpos",pos);

    // Fijamos las luces
    setLights(P,V);

    // Dibujamos la escena
    drawSuelo(P,V,I);

    glutSwapBuffers();
}

void setLights(glm::mat4 P, glm::mat4 V) {
    shaders.setLight("ulightG",lightG);
    for(int i=0; i<NLD; i++) shaders.setLight("ulightD["+toString(i)+"]",lightD[i]);
    for(int i=0; i<NLP; i++) shaders.setLight("ulightP["+toString(i)+"]",lightP[i]);
    for(int i=0; i<NLF; i++) shaders.setLight("ulightF["+toString(i)+"]",lightF[i]);

    for(int i=0; i<NLP; i++) {
        glm::mat4 M = glm::translate(I,lightP[i].position) * glm::scale(I,glm::vec3(0.03));
        drawObjectTex(sphere,texLuz,P,V,M);
    }
    for(int i=1; i<NLF; i++) {
        glm::mat4 M = glm::translate(I,lightF[i].position) * glm::scale(I,glm::vec3(0.03));
        drawObjectTex(cone,texLuz,P,V,M);
    }
}

void drawObjectMat(Model model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    shaders.setMat4("uN"  ,glm::transpose(glm::inverse(M)));
    shaders.setMat4("uM"  ,M);
    shaders.setMat4("uPVM",P*V*M);
    shaders.setBool("uWithMaterials",true);
    shaders.setMaterial("umaterial",material);
    model.renderModel(GL_FILL);
}

void drawObjectTex(Model model, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    shaders.setMat4("uN"  ,glm::transpose(glm::inverse(M)));
    shaders.setMat4("uM"  ,M);
    shaders.setMat4("uPVM",P*V*M);
    shaders.setBool("uWithMaterials", false);
    shaders.setTextures("utextures",textures);
    if(textures.normal!=0) shaders.setBool("uWithNormals",true);
    else                   shaders.setBool("uWithNormals",false);
    model.renderModel(GL_FILL);
}

void drawSuelo(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
    glm::mat4 S = glm::scale(I, glm::vec3(2, 0.0, 2));
    drawObjectTex(plane, texSuelo,P,V,M*S);
}


void funTimer(int ignore){
    if (spulsado == TRUE){
        rotT += 2.5;
    }
    glutPostRedisplay();
    glutTimerFunc(speed, funTimer, 0);
}

void zoom(int button, int state, int x, int y){
    if ((button == 3) && (fov>10)){
        fov-=1;
    }
    if ((button == 4) && (fov<90)){
        fov+=1;
    }
    if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        tecla_pulsada = 1;
    } else {
        tecla_pulsada = 0;
    }
    glutPostRedisplay();
}

void funcamara(int x, int y) {
    if (tecla_pulsada == 1)
    {
        alphaX -= x - oldX;
        oldX = x;
        alphaY += y - oldY;
        oldY = y;
    }
}

void funKeyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'r':
            RotTiovivo-=5;
            break;
        case 'R':
            RotTiovivo+=5;
            break;
        case 'a':
            if (RotBrazos>-45){
                RotBrazos-=3;
            }
            break;
        case 'A':
            if (RotBrazos<0){
                RotBrazos+=3;
            }
            break;
        case 'y':
            if (TSuperior>0.35){
                TSuperior-=0.01;
            }
            break;
        case 'Y':
            if (TSuperior<1.30){
                TSuperior+=0.01;
            }
            break;
        case 's':
            spulsado = !spulsado;
            break;
    }
    glutPostRedisplay();
}

void funSpecial(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_UP:    if (desZ>-1.5){desZ -= 0.1;} break;
        case GLUT_KEY_DOWN:  if (desZ<1.5){desZ += 0.1;} break;
        case GLUT_KEY_RIGHT: if (desX<1.5){desX += 0.1;} break;
        case GLUT_KEY_LEFT:  if (desX>-1.5){desX -= 0.1;} break;
    }
    glutPostRedisplay();
}//687