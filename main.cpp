#include "chip8.hpp"
#include <GL/glut.h>

void display();
void keyboardUp(BYTE key, int x, int y);
void keyboardDown(BYTE key, int x, int y);

chip8 machine;

int main(int argc, char **argv)
{
    if (argc != 2)
        exit_with_errmsg();

    machine.initialize();
    machine.load_game(argv[1]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

    glutInitWindowSize(640, 320);
    glutInitWindowPosition(320, 320);
    glutCreateWindow("chip8 emulator");

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);

    glutMainLoop();

    return (0);
}

void draw_pixel(int x, int y)
{
    float xx = (float)x / 32.0f - 1;
    float yy = -((float)y / 16.0f) + 1;
    glBegin(GL_QUADS);
    glVertex2f(xx, yy);
    glVertex2f(xx + (1.0f / 32.0f), yy);
    glVertex2f(xx + (1.0f / 32.0f), yy - (1.0f / 16.0f));
    glVertex2f(xx, yy - (1.0f / 16.0f));
    glEnd();
}

void update()
{
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            if (machine.gfx[y][x] == 0)
                glColor3f(0, 0, 0);
            else
                glColor3f(1, 1, 1);

            draw_pixel(x, y);
        }
    }
}

void display()
{
    machine.emulate_cycle();

    if (machine.draw_flag)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        update();
        glFlush();

        machine.draw_flag = false;
    }
}

void keyboardDown(BYTE key, int x, int y)
{
    if (key == 27) // esc
        exit(0);
    machine.set_keys(key, 1);
}

void keyboardUp(BYTE key, int x, int y)
{
    machine.set_keys(key, 0);
}
