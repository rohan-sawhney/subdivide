#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Subdivider.h"

int gridX = 600;
int gridY = 600;
int gridZ = 600;

const double fovy = 50.;
const double clipNear = .01;
const double clipFar = 1000.;
double x = 0;
double y = 0;
double z = -2.5;

std::vector<std::string> paths = {"/Users/rohansawhney/Desktop/developer/C++/subdivide/box.obj",
                                  "/Users/rohansawhney/Desktop/developer/C++/subdivide/icosahedron.obj",
                                  "/Users/rohansawhney/Desktop/developer/C++/subdivide/gourd.obj",
                                  "/Users/rohansawhney/Desktop/developer/C++/subdivide/teddy.obj"};
Subdivider subdivider;
Mesh mesh;
bool success = true;

void printInstructions()
{
    std::cerr << "space: toggle between meshes\n"
              << "↑/↓: move in/out\n"
              << "w/s: move up/down\n"
              << "a/d: move left/right\n"
              << "q: subdivide\n"
              << "escape: exit program\n"
              << std::endl;
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
}

void drawFaces()
{
    glColor4f(0.0, 0.0, 1.0, 0.6);
    for (FaceCIter f = mesh.faces.begin(); f != mesh.faces.end(); f++) {
        
        if (f->isBoundary()) continue;
        
        glBegin(GL_LINE_LOOP);
        HalfEdgeCIter he = f->he;
        do {
            glVertex3d(he->vertex->position.x(), he->vertex->position.y(), he->vertex->position.z());
            
            he = he->next;
            
        } while (he != f->he);
        
        glEnd();
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    double aspect = (double)viewport[2] / (double)viewport[3];
    gluPerspective(fovy, aspect, clipNear, clipFar);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(0, 0, z, x, y, 0, 0, 1, 0);
    
    if (success) {
        drawFaces();
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x0, int y0)
{
    switch (key) {
        case 27 :
            exit(0);
        case ' ':
            static int i = 0;
            i++;
            if (i == 4) i = 0;
            mesh.read(paths[i]);
            break;
        case 'a':
            x -= 0.03;
            break;
        case 'd':
            x += 0.03;
            break;
        case 'w':
            y += 0.03;
            break;
        case 's':
            y -= 0.03;
            break;
        case 'q':
            subdivider.process(mesh);
            break;
    }
    
    glutPostRedisplay();
}

void special(int i, int x0, int y0)
{
    switch (i) {
        case GLUT_KEY_UP:
            z += 0.03;
            break;
        case GLUT_KEY_DOWN:
            z -= 0.03;
            break;
    }
    
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    
    success = mesh.read(paths[0]);
    
    printInstructions();
    glutInitWindowSize(gridX, gridY);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInit(&argc, argv);
    glutCreateWindow("HalfEdge Mesh");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    
    return 0;
}
