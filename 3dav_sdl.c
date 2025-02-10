#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>
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

SDL_Renderer *renderer;
SDL_Window *window;
TTF_Font *font;
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
int fullscr;
int prev_xsz, prev_ysz;
float lpos[] = {-1, 2, 3, 0};

void render();
void setPerspective(float fov, float aspect, float nearx, float farx);
void loadObj(char *fname);
void drawObj();
void print_help(SDL_Renderer *renderer);

void setPerspective(float fov, float aspect, float nearx, float farx)
{
    glFrustum(-aspect * nearx, aspect * nearx, -nearx, nearx, nearx, farx);
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

void print_help(SDL_Renderer *renderer)
{
    int i;
    const char **text;
    text = help ? helptext : helpprompt;
    SDL_Color textColor = {255, 255, 255, 5};

    for (i = 0; text[i]; i++) {
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, text[i], textColor);
        if (!textSurface) {
            fprintf(stderr, "Failed to render text: %s\n", TTF_GetError());
            continue;
        }

        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (!textTexture) {
            fprintf(stderr, "Failed to create texture: %s\n", SDL_GetError());
            SDL_FreeSurface(textSurface);
            continue;
        }

        int textWidth = textSurface->w;
        int textHeight = textSurface->h;
        SDL_Rect renderQuad = {5, win_height - (i + 1) * 20, textWidth, textHeight};
        SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
}

void render(SDL_Renderer *renderer)
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
    if(anim) { glRotatef((SDL_GetTicks() - anim_start) / 10.0f, 0, 1, 0); }

    drawObj();
    print_help(renderer);

    nframes++;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
//int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "Failed to initialize SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    font = TTF_OpenFont("CascadiaCode.ttf", 24); // Replace with your font path and size
    if (!font) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    window = SDL_CreateWindow("3DAV", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_width, win_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "Failed to create SDL window\n");
        SDL_Quit();
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Failed to create SDL renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glClear(GL_DEPTH_BUFFER_BIT);

    loadObj("porsche.obj");

    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = 0;
                            break;
                        case SDLK_f:
                            fullscr ^= 1;
                            SDL_SetWindowFullscreen(window, fullscr ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                            break;
                        case SDLK_SPACE:
                            anim ^= 1;
                            if(anim) {
                                anim_start = SDL_GetTicks();
                                nframes = 0;
                            } else {
                                double tm = SDL_GetTicks() - anim_start;
                                long fps = (nframes * 100000) / tm;
                                printf("framerate: %ld.%ld fps\n", fps / 100, fps % 100);
                            }
                            break;
                        case SDLK_F1:
                            help ^= 1;
                            break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (bnstate[SDL_BUTTON_LEFT]) {
                        cam_theta += event.motion.xrel * 0.5;
                        cam_phi += event.motion.yrel * 0.5;
                        if(cam_phi < -90) cam_phi = -90;
                        if(cam_phi > 90) cam_phi = 90;
                    }
                    if (bnstate[SDL_BUTTON_RIGHT]) {
                        float up[3], right[3];
                        float theta = cam_theta * M_PI / 180.0f;
                        float phi = cam_phi * M_PI / 180.0f;

                        up[0] = -sin(theta) * sin(phi);
                        up[1] = -cos(phi);
                        up[2] = cos(theta) * sin(phi);
                        right[0] = cos(theta);
                        right[1] = 0;
                        right[2] = sin(theta);

                        cam_pan[0] += (right[0] * event.motion.xrel + up[0] * event.motion.yrel) * 0.01;
                        cam_pan[1] += up[1] * event.motion.yrel * 0.01;
                        cam_pan[2] += (right[2] * event.motion.xrel + up[2] * event.motion.yrel) * 0.01;
                    }
                    if (bnstate[SDL_BUTTON_MIDDLE]) {
                        cam_dist += event.motion.yrel * 0.1;
                        if(cam_dist < 0) cam_dist = 0;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    bnstate[event.button.button] = event.button.state == SDL_PRESSED;
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        win_width = event.window.data1;
                        win_height = event.window.data2;
                        glViewport(0, 0, win_width, win_height);
                    }
                    break;
            }
        }

        render(renderer);
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}