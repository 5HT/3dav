#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

const char *helptext[] = {
    "Rotate: left mouse drag",
    " Scale: right mouse drag up/down",
    "   Pan: middle mouse drag",
    "",
    "Toggle fullscreen: f",
    "Toggle animation: space",
    "Quit: escape",
    0
};

double tm = 0.0f;
const char *helpprompt[] = {"Press F1 for help", 0};
int win_width = 800, win_height = 600;
float cam_theta, cam_phi = 25, cam_dist = 8;
float cam_pan[3];
int mouse_x, mouse_y;
int bnstate[8];
int anim, help;
double anim_start;
long nframes;
GLuint object;
float objectrot;
char ch='1';
const unsigned char* buttons;
const float* axes;
int buttonsCount;
int axisCount;
int fullscr;
int prev_xsz, prev_ysz;
GLFWwindow* window;
float lpos[] = {-1, 2, 3, 0};

void setPerspective(float fov, float aspect, float nearx, float farx)
{
    glFrustum(-aspect * nearx, aspect * nearx, -nearx, nearx, nearx, farx);
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error (%d): %s\n", error, description);
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
    int dx = x - mouse_x;
    int dy = y - mouse_y;
    mouse_x = x;
    mouse_y = y;

    if (!(dx | dy)) return;

    if (bnstate[GLFW_MOUSE_BUTTON_LEFT]) {
        cam_theta += dx * 0.5;
        cam_phi += dy * 0.5;
        if(cam_phi < -90) cam_phi = -90;
        if(cam_phi > 90) cam_phi = 90;
    }

    if (bnstate[GLFW_MOUSE_BUTTON_RIGHT]) {
        float up[3], right[3];
        float theta = cam_theta * M_PI / 180.0f;
        float phi = cam_phi * M_PI / 180.0f;

        up[0] = -sin(theta) * sin(phi);
        up[1] = -cos(phi);
        up[2] = cos(theta) * sin(phi);
        right[0] = cos(theta);
        right[1] = 0;
        right[2] = sin(theta);

        cam_pan[0] += (right[0] * dx + up[0] * dy) * 0.01;
        cam_pan[1] += up[1] * dy * 0.01;
        cam_pan[2] += (right[2] * dx + up[2] * dy) * 0.01;
    }

    if (bnstate[GLFW_MOUSE_BUTTON_MIDDLE]) {
        cam_dist += dy * 0.1;
        if(cam_dist < 0) cam_dist = 0;
    }

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (button) {
           case GLFW_MOUSE_BUTTON_LEFT:                break;
           case GLFW_MOUSE_BUTTON_RIGHT:               break;
           case GLFW_MOUSE_BUTTON_MIDDLE:              break;
        }
    }

    bnstate[button] = action == GLFW_PRESS;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                 glfwSetWindowShouldClose(window, 1);
                 break;
            case GLFW_KEY_F1:
                 help ^= 1;
                 glfwSwapBuffers(window);
                 break;
            case GLFW_KEY_F:        
                 if (fullscr)
                 {
                     glfwGetWindowSize(window, &prev_xsz, &prev_ysz);
                     const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                     glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
                 } else {
                     glfwSetWindowMonitor(window, NULL, 100, 100, prev_xsz, prev_ysz, GLFW_DONT_CARE);
                 }
                 break;
            case GLFW_KEY_SPACE:
                 anim ^= 1;
                 if(anim) {
                    anim_start = glfwGetTime() * 1000;
                    nframes = 0;
                 } else {
                    double tm = glfwGetTime() * 1000 - anim_start;
                    long fps = (nframes * 100000) / tm;
                    printf("framerate: %ld.%ld fps\n", fps / 100, fps % 100);
                 }
                 break;
        }
    }
}

void process_joystick_input() {
    if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
        int count;
        const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
        if (axes && count >= 2) {
            printf("Joystick Axes: X = %f, Y = %f\n", axes[0], axes[1]);
        }
        
        int button_count;
        const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &button_count);
        if (buttons) {
            for (int i = 0; i < button_count; i++) {
                if (buttons[i] == GLFW_PRESS) {
                    printf("Button %d pressed\n", i);
                }
            }
        }
    }
}

void joystick_callback(int joy, int event) {
    if (event == GLFW_CONNECTED) {
        printf("Joystick %d connected: %s\n", joy, glfwGetJoystickName(joy));
    } else if (event == GLFW_DISCONNECTED) {
        printf("Joystick %d disconnected\n", joy);
    }

    int buttonsCount;
    int axisCount;

    const unsigned char* buttons = glfwGetJoystickButtons(joy, &buttonsCount);
    printf("Joystick %d has %d buttons.\n", joy, buttonsCount);

    for (int i = 0; i < buttonsCount; i++)
    {
        printf("Button %d: %s\n", i, (buttons[i] == GLFW_PRESS) ? "Pressed" : "Released");
    }

    const float* axes = glfwGetJoystickAxes(joy, &axisCount);
    printf("Joystick %d has %d axes.\n", joy, axisCount);

    for (int i = 0; i < axisCount; i++)
    {
        printf("Axis %d: %f\n", i, axes[i]);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    win_width = width;
    win_height = height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void loadObj(char *fname) {
    FILE *fp;
    int read;
    GLfloat x, y, z;
    char ch;
    object = glGenLists(1);
    fp = fopen(fname, "r");
    if (!fp) {
        printf("can't open file %s\n", fname);
        exit(1);
    }

    GLfloat vertices[10000][3];
    int faces[10000][3];
    int vertexCount = 0;
    int faceCount = 0;

    while (!feof(fp)) {
        read = fscanf(fp, " %c", &ch);
        if (ch == 'v') {
            fscanf(fp, "%f %f %f", &vertices[vertexCount][0], &vertices[vertexCount][1], &vertices[vertexCount][2]);
            vertexCount++;
        } else if (ch == 'f') {
            char faceLine[256];
            fgets(faceLine, sizeof(faceLine), fp);
            int v[3];
            int count = 0;
            char *token = strtok(faceLine, " \t\n");
            while (token != NULL && count < 3) {
                sscanf(token, "%d", &v[count]);
                token = strtok(NULL, " \t\n");
                count++;
            }
            for (int i=0; i < count; i++) faces[faceCount][i] = v[i];
            faceCount++;
        }
    }
    fclose(fp);
    glNewList(object, GL_COMPILE);
    {
        glPushMatrix();

/*
        // Draw points
        glPointSize(3.0);
        glBegin(GL_POINTS);
        for (int i = 0; i < vertexCount; i++) {
            glVertex3f(vertices[i][0], vertices[i][1], vertices[i][2]);
        }
        glEnd();
*/
        // Draw triangles
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < faceCount; i++) {
            int v1 = faces[i][0] - 1; // OBJ indices start at 1
            int v2 = faces[i][1] - 1;
            int v3 = faces[i][2] - 1;
            glVertex3f(vertices[v1][0], vertices[v1][1], vertices[v1][2]);
            glVertex3f(vertices[v2][0], vertices[v2][1], vertices[v2][2]);
            glVertex3f(vertices[v3][0], vertices[v3][1], vertices[v3][2]);
        }
        glEnd();

        glPopMatrix();
    }
    glEndList();
}

void drawObj()
{
    glPushMatrix();
    glColor3f(1.0,0.23,0.27);
    glScalef(0.1,0.1,0.1);
    glCallList(object);
    glPopMatrix();

    glBegin(GL_QUADS);
    glNormal3f( 0,  1,  0);
    glVertex3f(-5, -1.7,  5);
    glVertex3f( 5, -1.7,  5);
    glVertex3f( 5, -1.7, -5);
    glVertex3f(-5, -1.7, -5);
    glEnd();
}

void print_help(void)
{
    int i;
    const char *s, **text;

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, win_width, 0, win_height, -1, 1);

    text = help ? helptext : helpprompt;

    for(i=0; text[i]; i++) {
        glColor3f(0, 0.1, 0);
        glRasterPos2f(7, win_height - (i + 1) * 20 - 2);
        s = text[i];
//      while(*s) { glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *s++); }
        glColor3f(0, 0.9, 0);
        glRasterPos2f(5, win_height - (i + 1) * 20);
        s = text[i];
//      while(*s) { glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *s++); }
    }

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}

void render(GLFWwindow* window)
{  
    glViewport(0, 0, win_width, win_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    setPerspective(45.0, (double)win_width / (double)win_height, 0.1, 100.0);

    glClearColor(0.0,0.0,0.0,1.0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -cam_dist);
    glRotatef(cam_phi, 1, 0, 0);
    glRotatef(cam_theta, 0, 1, 0);
    glTranslatef(cam_pan[0], cam_pan[1], cam_pan[2]);
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    glPushMatrix();
    if(anim) { glRotatef((glfwGetTime() * 1000 - anim_start) / 10.0f, 0, 1, 0); }

    drawObj();
    print_help();

    nframes++;
}

int main(int argc, char **argv)
{
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    window = glfwCreateWindow(win_width, win_height, "3DAV", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetJoystickCallback(joystick_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glClear(GL_DEPTH_BUFFER_BIT);

    loadObj("porsche.obj");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        render(window);
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
