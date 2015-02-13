//standard libraries
#include <iostream>
using namespace std;

#include <GL/glew.h>
#include <GL/freeglut.h>

//opengl mathematics
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

//for srand(time(NULL)):
#include <ctime>
#include <cstdlib>

#include "definitions.h"
//functions for shader compilation and linking
#include "shaderhelper.h"
//object for drawing
#include "Branch.h"
#include "Leaf.h"
//for textures:
#include "lodepng.h"
//for light source:
#include "LightSource.h"


LightSource lightSource;

//model for drawing: a square from two triangles
Branch *pTree;

//struct for loading shaders
ShaderProgram shaderProgram;

//window size
int windowWidth = 800;
int windowHeight = 600;

//last mouse coordinates
int mouseX,mouseY;

//camera position
glm::vec3 eye(0, 0, 10);
//coordinates of camera look start:
glm::vec3 cen(0.0, 0, 0);
//angles of camera look
float camera_inclination = 0.0f;
float camera_rotation = 0.0f;

//matrices
glm::mat4x4 projectionMatrix;

///defines drawing mode
bool useTexture = true;

//texture identificator
const uint textures_c = 2;
GLuint texId[textures_c];

//names of shader files. program will search for them during execution
//don't forget place it near executable 
char VertexShaderName[] = "Vertex.vert";
char FragmentShaderName[] = "Fragment.frag";

////////////////////////////////////////////////////////

///
void
initTexture()
{
    
    //generate as many textures as you need
	glGenTextures(textures_c, &texId[0]);
	
    //enable texturing and zero slot
    glActiveTexture(GL_TEXTURE0);
    
    //don't use alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    vector<GLubyte> imgData;
    uint img_width, img_height;
    Leaf::texture_num = 0;
    Branch::texture_num = 1;
    string texture_fnames[] =
        { "../../Textures/LeafTex.png", "../../Textures/BranchTex.png" };
    assert(textures_c == sizeof(texture_fnames) / sizeof(*texture_fnames));
    
    for (uint i = 0; i < textures_c; ++i)
    {
        //bind texId[i] to 0 unit
        glBindTexture(GL_TEXTURE_2D, texId[i]);
        
        // Set nearest filtering mode for texture minification
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        imgData.clear();
        //Loading texture from file:
        if (lodepng::decode(imgData, img_width, img_height, texture_fnames[i]))
        {
            cerr << "Error on file " << texture_fnames[i] << endl;
        }
    
        //set Texture Data
        glTexImage2D(
            GL_TEXTURE_2D,      //target texture
            0,                  //image level-of-detail (0 = max detail)
            GL_RGBA,            //image format
            img_width,          //image width
            img_height,         //image height
            0,                  //image border
            GL_RGBA,            //internal format
            GL_UNSIGNED_BYTE,   //type (of image data element)
            &imgData[0]         //image data
        );
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
    //init light:
    lightSource.pos = glm::vec3(10.0f, 10.0f, 10.0f);
    lightSource.ambient = 0.2f;
    lightSource.diffuse = 0.4f;
    lightSource.specular = 0.4f;
}

/////////////////////////////////////////////////////////////////////
///is called when program starts
void
init()
{
	srand(time(NULL));
    
    //enable depth test
	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
	//initialize shader program
	shaderProgram.init(VertexShaderName,FragmentShaderName);
	//use this shader program
	glUseProgram(shaderProgram.programObject);
    
    
    //create tree branch
    uint tree_levels = 4;
	pTree = new Branch(
        tree_levels,
        glm::vec3(-0.5f, 1.0f, 0.0f), //root coordinates
        0.0f,                         //trunc inclination
        0.0f,                         //trunc rotation
        1.8f,                         //trunc height (affects branches' heights)
        0.1f,                         //trunc width (affects branches' widths)
        38,                           //initial children count
        9            //every level children count is reduced on this value
    );

    pTree->make_children();
    
    
	//initializa texture
	initTexture();
}


/////////////////////////////////////////////////////////////////////
///called when window size is changed
void
reshape(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    //set viewport to match window size
    glViewport(0, 0, width, height);
    
    float fieldOfView = 45.0f;
    float aspectRatio = float(width) / float(height);
    float zNear = 0.1f;
    float zFar = 100.0f;
    //set projection matrix
    projectionMatrix = glm::perspective(fieldOfView, aspectRatio, zNear, zFar);
}

////////////////////////////////////////////////////////////////////
///actions for single frame
void
display()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    //Draw triangle with shaders (in screen coordinates)
    glUseProgram(shaderProgram.programObject);
    
    //camera matrix. camera is placed in point "eye" and looks at certain angles
    auto viewMatrix = glm::mat4x4();
    static const glm::vec3 inclination_axis = glm::vec3(1, 0, 0);
    static const glm::vec3 rotation_axis = glm::vec3(0, 1, 0);
    viewMatrix = glm::rotate(viewMatrix, camera_inclination, inclination_axis);
    viewMatrix = glm::rotate(viewMatrix, camera_rotation, rotation_axis);
    viewMatrix = glm::translate(viewMatrix, -eye);

    //modelMatrix is connected with current object
    auto modelMatrix = glm::mat4();
    pTree->display(modelMatrix, viewMatrix, projectionMatrix, texId);
    
    //end frame visualization
    glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////
///IdleFunction
void
update()
{
	//make animation
	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////////
///is called when key on keyboard is pressed
///use SPACE to switch mode
///TODO: place camera transitions in this function
void
keyboard(unsigned char key, int mx, int my)
{
    auto viewMatrix = glm::mat4x4();
    static const glm::vec3 inclination_axis = glm::vec3(1, 0, 0);
    static const glm::vec3 rotation_axis = glm::vec3(0, 1, 0);
    viewMatrix = glm::rotate(viewMatrix, -camera_inclination, inclination_axis);
    viewMatrix = glm::rotate(viewMatrix, -camera_rotation, rotation_axis);
    
    glm::vec4 shift;
    float walk_step = 0.1f;
    if (key == 'w' || key == 'W')
    {
        shift[2] -= walk_step;
    }
    else if (key == 's' || key == 'S')
    {
        shift[2] += walk_step;
    }
    else if (key == 'a' || key == 'A')
    {
        shift[0] -= walk_step;
    } 
    else if (key == 'd' || key == 'D')
    {
        shift[0] += walk_step;
    }
    
    eye += glm::vec3(viewMatrix * shift);
}

/////////////////////////////////////////////////////////////////////////
///is called when mouse button is pressed
void
mouse(int button, int mode, int posx, int posy)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (mode == GLUT_DOWN)
        {
            mouseX = posx;
            mouseY = posy;
        }
        else
        {
            mouseX = -1;
            mouseY = -1;
        }
    }
}

static inline float
deperiodize(float x, float x0, float T)
{
    while (x < x0) {
        x += T;
    }
    while (x > T) {
        x -= T;
    }
    return x;
}

void
mousemotion(int posx, int posy)
{
    static const float mouse_sensitivity = 0.024f;
    camera_rotation = deperiodize(
        camera_rotation + mouse_sensitivity * (posx - mouseX),
        0, 360.0f
    );
    camera_inclination = deperiodize(
        camera_inclination + mouse_sensitivity * (posy - mouseY),
        0, 360.0f
    );
    glutWarpPointer(mouseX, mouseY);
}

////////////////////////////////////////////////////////////////////////
///this function is used in case of InitializationError
void
emptydisplay()
{
}

////////////////////////////////////////////////////////////////////////
///entry point
int
main (int argc, char *argv[])
{
    glutInit(&argc, argv);
    #ifdef __APPLE__
        glutInitDisplayMode(
            GLUT_3_2_CORE_PROFILE
            | GLUT_RGBA
            | GLUT_DOUBLE
            | GLUT_DEPTH
        );
    #else
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
        glutInitContextVersion(3, 2);  
        glutInitContextProfile(GLUT_CORE_PROFILE);
        glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
        glewExperimental = GL_TRUE;
    #endif
    
    glutCreateWindow("Test OpenGL application");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutReshapeWindow(windowWidth,windowHeight);
    glutIdleFunc(update);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mousemotion);
    
    glewInit();
    glGetError();
    const char *slVer = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    cout << "GLSL Version: " << slVer << endl;
    
    try
    {
        init();
    }
    catch (const char *str)
    {
        cout << "Error During Initialiation: " << str << endl;
        delete pTree;
        glDeleteTextures(1, texId);
        //start main loop with empty screen
        glutDisplayFunc(emptydisplay);
        glutMainLoop();
        return -1;
    }
    
    try
    {
        glutMainLoop();
    }
    catch (const char *str)
    {
        cout << "Error During Main Loop: " << str << endl;
    }
        
    //release memory
    delete pTree;
    
    glDeleteTextures(1, texId);
    return 0;
}
    
