#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include "graphics.h"
#include "common.h"

int team1_effort = 0;
int team2_effort = 0;


float rope_position = 0.0f;


void draw_text(float x, float y, char *string) {
    glRasterPos2f(x, y);
    for (char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void draw_stickman(float x, float y, float scale, float r, float g, float b, int effort) {
    glColor3f(r, g, b);

    float head_radius = 0.02f * scale;
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y + 0.08f * scale);
        for (int i = 0; i <= 20; i++) {
            float angle = i * 2.0f * 3.14159f / 20;
            glVertex2f(x + cos(angle) * head_radius, y + 0.08f * scale + sin(angle) * head_radius);
        }
    glEnd();
glPopMatrix();

    glBegin(GL_LINES);
        glVertex2f(x, y + 0.08f * scale - head_radius);
        glVertex2f(x, y);
        glVertex2f(x, y + 0.06f * scale);
        glVertex2f(x - 0.03f * scale, y + 0.03f * scale);
        glVertex2f(x, y + 0.06f * scale);
        glVertex2f(x + 0.03f * scale, y + 0.03f * scale);
        glVertex2f(x, y);
        glVertex2f(x - 0.02f * scale, y - 0.04f * scale);
        glVertex2f(x, y);
        glVertex2f(x + 0.02f * scale, y - 0.04f * scale);
    glEnd();

    // عرض الجهد فوق الرأس
    char buffer[32];
    sprintf(buffer, "Effort: %d", effort);
    glColor3f(0, 0, 0);
    draw_text(x - 0.04f, y + 0.12f * scale, buffer);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.9, 0.9, 0.9);
    glPushMatrix();
glTranslatef(rope_position, 0.0f, 0.0f);
glBegin(GL_QUADS);
        glVertex2f(-1.0f, -0.2f);
        glVertex2f(1.0f, -0.2f);
        glVertex2f(1.0f, 0.2f);
        glVertex2f(-1.0f, 0.2f);
    glEnd();

    for (int i = 0; i < 4; i++) {
        draw_stickman(-0.6f + i * 0.1f, -0.15f, 1.0f, 0.2f, 0.2f, 1.0f, team1_player_efforts[i]);
        draw_stickman(0.6f - i * 0.1f, -0.15f, 1.0f, 1.0f, 0.2f, 0.2f, team2_player_efforts[i]);
    }

    glPushMatrix();
        glTranslatef(rope_position, 0.0f, 0.0f);
        glColor3f(0.8, 0.4, 0.0);
        glPushMatrix();
glTranslatef(rope_position, 0.0f, 0.0f);
glBegin(GL_QUADS);
            glVertex2f(-0.1f, -0.05f);
            glVertex2f(0.1f, -0.05f);
            glVertex2f(0.1f, 0.05f);
            glVertex2f(-0.1f, 0.05f);
        glEnd();
    glPopMatrix();

    glutSwapBuffers();
}

void timer(int value) {
    glutPostRedisplay();  // يرسم الشاشة

    float diff = (float)(team1_effort - team2_effort) / 100.0f;
    rope_position += diff;

    if (rope_position > 0.9f) rope_position = 0.9f;
    if (rope_position < -0.9f) rope_position = -0.9f;

    glutTimerFunc(1000, timer, 0);
}


void init_graphics(int *argc, char **argv) {
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 400);
    glutCreateWindow("🏋️ Rope Pulling Game Visualization");

    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);

    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
}

void start_graphics() {
    glutMainLoop();
}

void update_effort(int t1, int t2) {
    team1_effort = t1;
    team2_effort = t2;
}
void update_single_player_effort(int player_id, int effort) {
    if (player_id < 4) {
        team1_player_efforts[player_id] = effort;
    } else if (player_id < 8) {
        team2_player_efforts[player_id - 4] = effort;
    }


    glutPostRedisplay();
}




void update_player_efforts(int team1[4], int team2[4]) {
    for (int i = 0; i < 4; i++) {
        team1_player_efforts[i] = team1[i];
        team2_player_efforts[i] = team2[i];
    }
}


