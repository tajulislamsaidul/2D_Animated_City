
//  2D Animated City Landscape - UPDATED
//  ALGORITHMS ADDED:
//    [1] Basic Shapes         - Triangle, Quad, Polygon (existing, kept)
//    [2] Loop & Condition     - Windows, trusses, rain, waves (existing, kept)
//    [3] DDA Line Algorithm   - River waves, lamp poles, road markings
//    [4] Bresenham's Line     - Bridge trusses, bird wings, building outlines
//    [5] Mid-Point Circle     - Car wheels, sun/moon outline, lamp glow
//    [6] 2D Transformation    - Rotating windmill (Translate + Rotate + Scale)
//    [7] Moving Objects       - Clouds, boat, car, birds (existing, kept)
//    [8] Key-Controlled Move  - Arrow Keys move the YELLOW player car
//  KEYBOARD CONTROLS:
//    D / N      - Toggle Day / Night
//    L          - Toggle street Lights
//    R          - Toggle Rain
//    Arrow Keys - Move the YELLOW player car (left / right)
//    ESC        - Quit

#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

const float PI = 3.14159265f;
//  Global Animation Variables
float cloudX = 0.0f;
float carX = -100.0f;
float boatX = 1100.0f;
float birdX = -50.0f;
float waveX = 0.0f;
float windAngle = 0.0f;   // Windmill rotation angle
float billboardAnim = 0.0f;

// State flags
bool isDay = true;
bool lightsOn = false;
bool isRaining = false;

// Key-controlled player car
float playerCarX = 400.0f;
float playerCarY = 220.0f;

// Rain system
#define MAX_RAIN 500
struct Drop { float x, y, speed; };
Drop rain[MAX_RAIN];

//Day/Night colour selector
void setDayNightColor(float r, float g, float b)
{
    if (isDay)
        glColor3f(r, g, b);
    else
        glColor3f(r * 0.3f, g * 0.3f, b * 0.4f);
}
//  FILLED CIRCLE – GL_TRIANGLE_FAN
void drawCircle(float cx, float cy, float r, int segs)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segs; i++)
    {
        float theta = 2.0f * PI * i / (float)segs;
        glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
    }
    glEnd();
}

//  DDA LINE DRAWING ALGORITHM
//  Used for: river wave shimmer, road lane markings, lamp poles
void ddaLine(int x1, int y1, int x2, int y2)
{
    float dx = (float)(x2 - x1);
    float dy = (float)(y2 - y1);
    int   steps = (int)((fabsf(dx) > fabsf(dy)) ? fabsf(dx) : fabsf(dy));
    if (steps == 0) return;
    float xInc = dx / steps;
    float yInc = dy / steps;
    float x = (float)x1;
    float y = (float)y1;

    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++)
    {
        glVertex2i((int)(x + 0.5f), (int)(y + 0.5f));
        x += xInc;
        y += yInc;
    }
    glEnd();
}
//  BRESENHAM'S LINE DRAWING ALGORITHM
//  Used for: bridge trusses, bird wings, building grid outlines
void bresenhamLine(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x2 >= x1) ? 1 : -1;
    int sy = (y2 >= y1) ? 1 : -1;
    int x = x1, y = y1;

    glBegin(GL_POINTS);
    glVertex2i(x, y);
    if (dx >= dy)
    {
        int pk = 2 * dy - dx;
        for (int i = 0; i < dx; i++)
        {
            x += sx;
            if (pk < 0)
                pk += 2 * dy;
            else { y += sy; pk += 2 * dy - 2 * dx; }
            glVertex2i(x, y);
        }
    }
    else
    {
        int pk = 2 * dx - dy;
        for (int i = 0; i < dy; i++)
        {
            y += sy;
            if (pk < 0)
                pk += 2 * dx;
            else { x += sx; pk += 2 * dx - 2 * dy; }
            glVertex2i(x, y);
        }
    }
    glEnd();
}
//  [5] MID-POINT CIRCLE DRAWING ALGORITHM  (outline)
//  Used for: car wheels outline, sun/moon ring, lamp glow ring
void plotCirclePoints(int xc, int yc, int x, int y)
{
    glBegin(GL_POINTS);
    glVertex2i(xc + x, yc + y); glVertex2i(xc - x, yc + y);
    glVertex2i(xc + x, yc - y); glVertex2i(xc - x, yc - y);
    glVertex2i(xc + y, yc + x); glVertex2i(xc - y, yc + x);
    glVertex2i(xc + y, yc - x); glVertex2i(xc - y, yc - x);
    glEnd();
}

void midpointCircle(int xc, int yc, int r)
{
    int x = 0, y = r;
    int pk = 1 - r;
    plotCirclePoints(xc, yc, x, y);
    while (x < y)
    {
        x++;
        if (pk < 0) pk += 2 * x + 1;
        else { y--; pk += 2 * x - 2 * y + 1; }
        plotCirclePoints(xc, yc, x, y);
    }
}
//  SKY  (unchanged logic, uses GL_QUADS)
void drawSky()
{
    glBegin(GL_QUADS);
    if (isDay)
    {
        glColor3f(0.4f, 0.7f, 1.0f);
        glVertex2f(0, 200); glVertex2f(1000, 200);
        glColor3f(0.1f, 0.4f, 0.8f);
        glVertex2f(1000, 700); glVertex2f(0, 700);
    }
    else
    {
        glColor3f(0.05f, 0.05f, 0.1f);
        glVertex2f(0, 200); glVertex2f(1000, 200);
        glColor3f(0.0f, 0.0f, 0.05f);
        glVertex2f(1000, 700); glVertex2f(0, 700);
    }
    glEnd();

    // Stars at night
    if (!isDay && !isRaining)
    {
        srand(12345);
        glColor3f(1.0f, 1.0f, 1.0f);
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        for (int i = 0; i < 150; i++)
            glVertex2f((float)(rand() % 1000), 300.0f + rand() % 400);
        glEnd();
    }
}

//SUN
void drawSun()
{
    if (!isDay) return;
    glColor3f(1.0f, 0.9f, 0.0f);
    drawCircle(250, 600, 45, 50);
}
//  MOON
void drawMoon()
{
    if (isDay) return;
    glColor3f(0.9f, 0.9f, 0.9f);
    drawCircle(250, 600, 40, 50);
}
//  CLOUD
void drawCloud(float x, float y)
{
    if (isDay) glColor3f(1.0f, 1.0f, 1.0f);
    else       glColor3f(0.3f, 0.3f, 0.4f);

    drawCircle(x, y, 30, 30);
    drawCircle(x + 25, y + 15, 35, 30);
    drawCircle(x + 50, y, 30, 30);
    drawCircle(x + 25, y - 10, 25, 30);
}
//  Billboard
void drawBillboard()
{
    //Animated color using smooth RGB change
    float r = (sinf(billboardAnim) + 1) / 2;
    float g = (sinf(billboardAnim + 2) + 1) / 2;
    float b = (sinf(billboardAnim + 4) + 1) / 2;
    setDayNightColor(r, g, b);

    // Billboard board
    glBegin(GL_QUADS);
    glVertex2f(780, 470);
    glVertex2f(960, 470);
    glVertex2f(960, 540);
    glVertex2f(780, 540);
    glEnd();

    //Glow effect only at night
    if (!isDay)
    {
        glColor4f(r, g, b, 0.3f);

        glBegin(GL_QUADS);
        glVertex2f(780, 470);
        glVertex2f(960, 470);
        glVertex2f(960, 540);
        glVertex2f(780, 540);
        glEnd();
    }
    //Border
    glColor3f(0, 0, 0);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    glVertex2f(780, 470);
    glVertex2f(960, 470);
    glVertex2f(960, 540);
    glVertex2f(780, 540);
    glEnd();

    //Text
    glColor3f(0, 0, 0);
    glRasterPos2f(800, 505);
    const char* text = "The River Town";
    for (int i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);

    //Stand
    setDayNightColor(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(810, 470);
    glVertex2f(825, 470);
    glVertex2f(825, 460);
    glVertex2f(810, 460);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(915, 470);
    glVertex2f(930, 470);
    glVertex2f(930, 460);
    glVertex2f(915, 460);
    glEnd();
}

//  BUILDING using Bresenham outlines [4], loop windows [2]

// layer 1
void drawBuilding1(float x, float y, float w, float h, int rows, int cols)
{
    setDayNightColor(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();

    // Outline via Bresenham
    glColor3f(0.2f, 0.2f, 0.2f);
    glPointSize(1.5f);
    bresenhamLine((int)x, (int)y, (int)(x + w), (int)y);
    bresenhamLine((int)(x + w), (int)y, (int)(x + w), (int)(y + h));
    bresenhamLine((int)(x + w), (int)(y + h), (int)x, (int)(y + h));
    bresenhamLine((int)x, (int)(y + h), (int)x, (int)y);

    // Windows via nested loop
    float winW = (w / cols) * 0.5f;
    float winH = (h / rows) * 0.5f;
    float padX = (w / cols) * 0.25f;
    float padY = (h / rows) * 0.25f;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            float wx = x + padX + c * (w / cols);
            float wy = y + padY + r * (h / rows);

            if (lightsOn) glColor3f(1.0f, 1.0f, 0.0f);
            else
                setDayNightColor(0.2f, 0.3f, 0.4f);

            glBegin(GL_QUADS);
            glVertex2f(wx, wy);
            glVertex2f(wx + winW, wy);
            glVertex2f(wx + winW, wy + winH);
            glVertex2f(wx, wy + winH);
            glEnd();
        }
    }
}

// layer 2
void drawBuilding2(float x, float y, float w, float h, int rows, int cols)
{
    setDayNightColor(0.7f, 0.7f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();

    // Outline via Bresenham
    glColor3f(0.2f, 0.2f, 0.2f);
    glPointSize(1.5f);
    bresenhamLine((int)x, (int)y, (int)(x + w), (int)y);
    bresenhamLine((int)(x + w), (int)y, (int)(x + w), (int)(y + h));
    bresenhamLine((int)(x + w), (int)(y + h), (int)x, (int)(y + h));
    bresenhamLine((int)x, (int)(y + h), (int)x, (int)y);

    // Windows via nested loop [2]
    float winW = (w / cols) * 0.5f;
    float winH = (h / rows) * 0.5f;
    float padX = (w / cols) * 0.25f;
    float padY = (h / rows) * 0.25f;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            float wx = x + padX + c * (w / cols);
            float wy = y + padY + r * (h / rows);

            if (lightsOn) glColor3f(2.0f, 2.0f, 0.0f);
            else
                setDayNightColor(0.2f, 0.3f, 0.4f);

            glBegin(GL_QUADS);
            glVertex2f(wx, wy);
            glVertex2f(wx + winW, wy);
            glVertex2f(wx + winW, wy + winH);
            glVertex2f(wx, wy + winH);
            glEnd();
        }
    }
}

// layer 3
void drawBuilding3(float x, float y, float w, float h, int rows, int cols)
{
    setDayNightColor(0.6f, 0.6f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();

    // Outline via Bresenham 
    glColor3f(0.2f, 0.2f, 0.2f);
    glPointSize(1.5f);
    bresenhamLine((int)x, (int)y, (int)(x + w), (int)y);
    bresenhamLine((int)(x + w), (int)y, (int)(x + w), (int)(y + h));
    bresenhamLine((int)(x + w), (int)(y + h), (int)x, (int)(y + h));
    bresenhamLine((int)x, (int)(y + h), (int)x, (int)y);

    // Windows via nested loop
    float winW = (w / cols) * 0.5f;
    float winH = (h / rows) * 0.5f;
    float padX = (w / cols) * 0.25f;
    float padY = (h / rows) * 0.25f;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            float wx = x + padX + c * (w / cols);
            float wy = y + padY + r * (h / rows);

            if (lightsOn) glColor3f(3.0f, 3.0f, 0.0f);
            else
                setDayNightColor(0.2f, 0.3f, 0.4f);

            glBegin(GL_QUADS);
            glVertex2f(wx, wy);
            glVertex2f(wx + winW, wy);
            glVertex2f(wx + winW, wy + winH);
            glVertex2f(wx, wy + winH);
            glEnd();
        }
    }
}

//  HOUSE (Triangle roof, Quad walls)
void drawHouse(float x, float y)
{
    setDayNightColor(0.8f, 0.6f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + 60, y);
    glVertex2f(x + 60, y + 50);
    glVertex2f(x, y + 50);
    glEnd();

    setDayNightColor(0.7f, 0.2f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 10, y + 50);
    glVertex2f(x + 70, y + 50);
    glVertex2f(x + 30, y + 80);
    glEnd();

    if (lightsOn) glColor3f(1.0f, 1.0f, 0.0f);
    else          setDayNightColor(0.2f, 0.3f, 0.4f);

    glBegin(GL_QUADS);
    glVertex2f(x + 20, y + 20);
    glVertex2f(x + 40, y + 20);
    glVertex2f(x + 40, y + 35);
    glVertex2f(x + 20, y + 35);
    glEnd();

}

//  RIVER DDA wave shimmer, Quad body
void drawRiver()
{
    glBegin(GL_QUADS);
    if (isDay)
    {
        glColor3f(0.2f, 0.6f, 0.9f);
        glVertex2f(0, 0);    glVertex2f(1000, 0);
        glColor3f(0.1f, 0.4f, 0.7f);
        glVertex2f(1000, 200); glVertex2f(0, 200);
    }
    else
    {
        glColor3f(0.05f, 0.1f, 0.2f);
        glVertex2f(0, 0);    glVertex2f(1000, 0);
        glColor3f(0.02f, 0.05f, 0.1f);
        glVertex2f(1000, 200); glVertex2f(0, 200);
    }
    glEnd();

    // Wave shimmer lines DDA + loop 
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(1.5f);
    for (int i = 0; i < 20; i++)
    {
        int wx1 = (int)fmod(i * 52 + waveX, 1050.0f);
        int wy = 18 + i * 9;
        if (wy > 195) break;
        // Alternate slope direction for ripple effect
        if (i % 2 == 0)
            ddaLine(wx1, wy, wx1 + 35, wy + 4);
        else
            ddaLine(wx1, wy, wx1 + 35, wy - 3);
    }
}

//  BRIDGE  Bresenham trusses , DDA lamp poles
//  Midpoint circle lamps, loop

void drawBridge()
{
    // Pillars Quads + loop
    for (int i = 250; i <= 750; i += 250)
    {
        setDayNightColor(0.2f, 0.2f, 0.2f);
        glBegin(GL_QUADS);
        glVertex2f(i - 15, 40);
        glVertex2f(i + 15, 40);
        glVertex2f(i + 15, 200);
        glVertex2f(i - 15, 200);
        glEnd();

        // Pillar outline  Bresenham
        glColor3f(0.1f, 0.1f, 0.1f);
        glPointSize(1.5f);
        bresenhamLine(i - 15, 40, i + 15, 40);
        bresenhamLine(i + 15, 40, i + 15, 200);
        bresenhamLine(i + 15, 200, i - 15, 200);
        bresenhamLine(i - 15, 200, i - 15, 40);
    }

    // Bridge deck Quad
    setDayNightColor(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0, 180);
    glVertex2f(1000, 180);
    glVertex2f(1000, 220);
    glVertex2f(0, 220);
    glEnd();

    // Road lane dashes DDA + loop
    glColor3f(0.95f, 0.90f, 0.20f);
    glPointSize(2.0f);
    for (int i = 0; i < 1000; i += 60)
    {
        // Only draw dash with condition
        if (i % 120 < 60)
            ddaLine(i, 210, i + 40, 210);
    }

    // Bridge trusses  Bresenham + loop
    glColor3f(0.2f, 0.2f, 0.2f);
    glPointSize(2.5f);
    // Top railing
    bresenhamLine(0, 240, 1000, 240);
    bresenhamLine(0, 220, 1000, 220);
    // bottom railing
    bresenhamLine(0, 200, 1000, 200);
    bresenhamLine(0, 180, 1000, 180);
    // Verticals & cross diagonals
    for (int i = 0; i < 1000; i += 50)
    {
        bresenhamLine(i, 220, i, 240);             // vertical
        bresenhamLine(i, 220, i + 50, 240);        // diagonal
        bresenhamLine(i + 50, 220, i, 240);        // diagonal back slash
        bresenhamLine(i, 180, i, 200);             // vertical
        bresenhamLine(i, 180, i + 50, 200);        // diagonal 
        bresenhamLine(i + 50, 180, i, 200);
    }

    // Lamp poles; lamp bulbs Midpoint Circle; loop
    for (int i = 100; i < 1000; i += 200)
    {
        // Pole via DDA
        if (lightsOn) glColor3f(0.6f, 0.6f, 0.6f);
        else          setDayNightColor(0.3f, 0.3f, 0.3f);
        glPointSize(3.0f);
        ddaLine(i, 240, i, 320);
        // Arm
        ddaLine(i, 320, i + 12, 330);
        // Bulb filled circle core
        if (lightsOn) glColor3f(1.0f, 1.0f, 0.4f);
        else          setDayNightColor(0.8f, 0.8f, 0.8f);
        drawCircle((float)(i + 12), 330, 8, 15);
        // Bulb outline  Midpoint Circle
        glColor3f(0.3f, 0.3f, 0.2f);
        glPointSize(1.5f);
        midpointCircle(i + 12, 330, 9);
    }
    for (int i = 0; i < 1000; i += 200)
    {
        // Pole via DDA
        if (lightsOn) glColor3f(0.6f, 0.6f, 0.6f);
        else          setDayNightColor(0.3f, 0.3f, 0.3f);
        glPointSize(3.0f);
        ddaLine(i, 200, i, 280);
        // Arm
        ddaLine(i, 280, i + 12, 290);
        // Bulb – filled circle core
        if (lightsOn) glColor3f(1.0f, 1.0f, 0.4f);
        else          setDayNightColor(0.8f, 0.8f, 0.8f);
        drawCircle((float)(i + 12), 290, 8, 15);
        // Bulb outline  Midpoint Circle
        glColor3f(0.3f, 0.3f, 0.2f);
        glPointSize(1.5f);
        midpointCircle(i + 12, 290, 9);
    }

}
//  FOREGROUND LAND Polygon
void drawForegroundLand()
{
    setDayNightColor(0.3f, 0.5f, 0.2f);
    glBegin(GL_POLYGON);
    glVertex2f(0, 0);
    glVertex2f(380, 0);
    glVertex2f(200, 60);
    glVertex2f(320, 40);
    glVertex2f(100, 80);
    glVertex2f(150, 90);
    glVertex2f(0, 120);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(1000, 0);
    glVertex2f(600, 0);
    glVertex2f(750, 50);
    glVertex2f(700, 50);
    glVertex2f(950, 100);
    glVertex2f(800, 90);
    glVertex2f(1000, 140);
    glEnd();
}
//  TREE glPushMatrix / glTranslatef / glScalef
void drawTree(float x, float y, float scale)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    setDayNightColor(0.4f, 0.2f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-5, 0);
    glVertex2f(5, 0);
    glVertex2f(5, 40);
    glVertex2f(-5, 40);
    glEnd();

    setDayNightColor(0.1f, 0.6f, 0.2f);
    drawCircle(0, 50, 25, 20);
    drawCircle(-15, 40, 20, 20);
    drawCircle(15, 40, 20, 20);
    drawCircle(0, 70, 20, 20);
    glPopMatrix();
}

void drawTree1(float x, float y, float scale)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    // Tree trunk
    setDayNightColor(0.4f, 0.2f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-5, 0);
    glVertex2f(5, 0);
    glVertex2f(5, 40);
    glVertex2f(-5, 40);
    glEnd();

    // Leaves (polygon instead of circle)
    setDayNightColor(0.1f, 0.6f, 0.2f);
    // Top leaf (bigger)
    glBegin(GL_POLYGON);
    glVertex2f(0, 85);
    glVertex2f(-15, 78);
    glVertex2f(-30, 65);
    glVertex2f(-15, 50);
    glVertex2f(0, 40);
    glVertex2f(15, 50);
    glVertex2f(30, 65);
    glVertex2f(15, 78);
    glEnd();

    // Left leaf (bigger)
    glBegin(GL_POLYGON);
    glVertex2f(-20, 65);
    glVertex2f(-35, 58);
    glVertex2f(-50, 45);
    glVertex2f(-35, 30);
    glVertex2f(-20, 20);
    glVertex2f(-5, 30);
    glVertex2f(5, 45);
    glVertex2f(-5, 58);
    glEnd();

    // Right leaf (bigger)
    glBegin(GL_POLYGON);
    glVertex2f(20, 65);
    glVertex2f(5, 58);
    glVertex2f(-5, 45);
    glVertex2f(5, 30);
    glVertex2f(20, 20);
    glVertex2f(35, 30);
    glVertex2f(50, 45);
    glVertex2f(35, 58);
    glEnd();

    // Middle leaf (bigger)
    glBegin(GL_POLYGON);
    glVertex2f(0, 70);
    glVertex2f(-15, 63);
    glVertex2f(-22, 50);
    glVertex2f(-15, 35);
    glVertex2f(0, 25);
    glVertex2f(15, 35);
    glVertex2f(22, 50);
    glVertex2f(15, 63);
    glEnd();

    glPopMatrix();
}

// WINDMILL 2D Transformation: Translate + Rotate + Scale
// Placed on top of the tallest building
void drawWindmill(float x, float y)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);          // Move to position

    // Static tower
    setDayNightColor(0.55f, 0.55f, 0.58f);
    glBegin(GL_QUADS);
    glVertex2f(-6, 0); glVertex2f(6, 0);
    glVertex2f(4, 35); glVertex2f(-4, 35);
    glEnd();
    // Hub
    setDayNightColor(0.7f, 0.7f, 0.7f);
    drawCircle(0, 35, 5, 12);

    // Rotating blades via glRotatef
    glTranslatef(0, 35, 0);             // Move to hub centre
    glRotatef(windAngle, 0, 0, 1);     // Rotate around Z axis

    setDayNightColor(0.82f, 0.82f, 0.82f);
    for (int blade = 0; blade < 4; blade++)
    {
        glPushMatrix();
        glRotatef(blade * 90.0f, 0, 0, 1);  // Space 4 blades 90° apart
        glBegin(GL_POLYGON);
        glVertex2f(0, 4);
        glVertex2f(2, 6);
        glVertex2f(3, 28);
        glVertex2f(-3, 28);
        glVertex2f(-2, 6);
        glVertex2f(0, 4);
        glEnd();
        glPopMatrix();
    }

    glPopMatrix();
}

//  TRAFFIC CAR  – auto-moving
//  Wheels use Midpoint Circle 
void drawCar(float x, float y)
{
    // Body
    setDayNightColor(0.8f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(x, y + 5);  glVertex2f(x + 60, y + 5);
    glVertex2f(x + 60, y + 20); glVertex2f(x, y + 20);
    glEnd();

    // Cabin
    setDayNightColor(0.6f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(x + 15, y + 20); glVertex2f(x + 45, y + 20);
    glVertex2f(x + 40, y + 35); glVertex2f(x + 20, y + 35);
    glEnd();

    // Wheel filled + midpoint circle outline
    glColor3f(0.1f, 0.1f, 0.1f);
    drawCircle(x + 15, y + 5, 8, 20);
    drawCircle(x + 45, y + 5, 8, 20);

    glColor3f(0.45f, 0.45f, 0.45f);
    glPointSize(2.0f);
    midpointCircle((int)(x + 15), (int)(y + 5), 8);
    midpointCircle((int)(x + 45), (int)(y + 5), 8);
    // Headlights
    if (lightsOn)
    {
        glColor3f(1.0f, 1.0f, 0.6f);
        drawCircle(x + 58, y + 12, 4, 10);
    }
}
//  PLAYER CAR KEY-CONTROLLED
void drawPlayerCar(float x, float y)
{
    // Body yellow
    setDayNightColor(0.95f, 0.85f, 0.10f);
    glBegin(GL_QUADS);
    glVertex2f(x, y + 5);  glVertex2f(x + 60, y + 5);
    glVertex2f(x + 60, y + 20); glVertex2f(x, y + 20);
    glEnd();

    // Cabin darker yellow
    setDayNightColor(0.75f, 0.65f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(x + 15, y + 20); glVertex2f(x + 45, y + 20);
    glVertex2f(x + 40, y + 35); glVertex2f(x + 20, y + 35);
    glEnd();

    // Wheels filled + midpoint circle outline
    glColor3f(0.1f, 0.1f, 0.1f);
    drawCircle(x + 15, y + 5, 8, 20);
    drawCircle(x + 45, y + 5, 8, 20);

    glColor3f(0.45f, 0.45f, 0.45f);
    glPointSize(2.0f);
    midpointCircle((int)(x + 15), (int)(y + 5), 8);
    midpointCircle((int)(x + 45), (int)(y + 5), 8);

    // Always on headlights for player
    glColor3f(1.0f, 1.0f, 0.6f);
    drawCircle(x + 58, y + 12, 4, 10);
    drawCircle(x + 2, y + 12, 4, 10);
}

//  BOAT
void drawBoat()
{
    float y = 80;
    setDayNightColor(0.4f, 0.2f, 0.1f);
    glBegin(GL_POLYGON);
    glVertex2f(boatX, y);
    glVertex2f(boatX + 80, y);
    glVertex2f(boatX + 100, y + 25);
    glVertex2f(boatX - 20, y + 25);
    glEnd();

    // Mast DDA
    glColor3f(0.0f, 0.0f, 0.0f);
    glPointSize(3.0f);
    ddaLine((int)(boatX + 40), (int)(y + 25), (int)(boatX + 40), (int)(y + 90));
    // Sail Triangle
    if (isDay) glColor3f(0.9f, 0.9f, 0.9f);
    else       glColor3f(0.5f, 0.5f, 0.5f);

    glBegin(GL_TRIANGLES);
    glVertex2f(boatX + 42, y + 30);
    glVertex2f(boatX + 90, y + 30);
    glVertex2f(boatX + 42, y + 85);
    glEnd();
}
//  BIRD Bresenham lines
void drawBird(float x, float y)
{
    if (!isDay) return;
    glColor3f(0.1f, 0.1f, 0.1f);
    glPointSize(2.0f);
    // Two wing strokes via Bresenham
    bresenhamLine((int)x, (int)(y + 10),
        (int)(x + 10), (int)y);
    bresenhamLine((int)(x + 10), (int)y,
        (int)(x + 20), (int)(y + 10));
}

//  RAIN
void drawRain()
{
    if (!isRaining) return;
    glColor3f(0.6f, 0.7f, 0.8f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    for (int i = 0; i < MAX_RAIN; i++)
    {
        glVertex2f(rain[i].x, rain[i].y);
        glVertex2f(rain[i].x - 6.0f, rain[i].y - 20.0f);
    }
    glEnd();
}

//  DISPLAY CALLBACK
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    //Background 
    drawSky();
    drawSun();
    drawMoon();

    drawCloud(fmod(100 + cloudX, 1100) - 50, 600);
    drawCloud(fmod(400 + cloudX * 0.8f, 1100) - 50, 520);
    drawCloud(fmod(750 + cloudX * 1.2f, 1100) - 50, 580);
    drawBird(fmod(birdX, 1100) - 20, 600);
    drawBird(fmod(birdX + 40, 1100) - 20, 620);
    drawBird(fmod(birdX + 20, 1100) - 20, 580);
    //City Skyline
    drawBuilding1(0, 200, 70, 220, 6, 3);
    drawBuilding1(80, 200, 60, 310, 8, 3);
    drawBuilding1(150, 200, 80, 300, 8, 3);
    drawBuilding1(240, 200, 60, 360, 12, 2);
    drawBuilding1(310, 200, 100, 400, 12, 4);
    drawBuilding1(420, 200, 70, 350, 10, 3);
    drawBuilding1(500, 200, 90, 340, 10, 4);
    drawBuilding1(600, 200, 80, 280, 8, 3);
    drawBuilding1(690, 200, 60, 300, 8, 2);
    drawBuilding1(760, 200, 240, 260, 8, 8);

    drawBuilding2(0, 200, 25, 190, 4, 1);
    drawBuilding2(40, 200, 75, 250, 6, 3);
    drawBuilding2(130, 200, 85, 260, 7, 4);
    drawBuilding2(230, 200, 95, 280, 7, 4);
    drawBuilding2(340, 200, 85, 320, 8, 3);
    drawBuilding2(440, 200, 85, 300, 8, 3);
    drawBuilding2(540, 200, 105, 250, 7, 4);
    drawBuilding2(660, 200, 65, 260, 8, 2);
    drawBuilding2(740, 200, 65, 240, 7, 2);
    drawBuilding2(820, 200, 105, 210, 6, 4);
    drawBuilding2(940, 200, 105, 220, 6, 5);

    drawBuilding3(0, 200, 85, 160, 5, 4);
    drawBuilding3(105, 200, 95, 200, 6, 4);
    drawBuilding3(220, 200, 85, 220, 7, 3);
    drawBuilding3(325, 200, 115, 250, 7, 5);
    drawBuilding3(460, 200, 85, 230, 7, 4);
    drawBuilding3(565, 200, 85, 210, 7, 4);
    drawBuilding3(670, 200, 105, 190, 7, 5);
    drawBuilding3(795, 200, 95, 170, 6, 4);
    drawBuilding3(910, 200, 95, 180, 6, 4);
    drawBillboard();

    // Windmill on tallest building (310, 200, h=400 , top = 600)
    drawWindmill(360, 600);
    drawWindmill(640, 480);

    drawHouse(800, 200);
    drawHouse(880, 200);
    drawHouse(140, 200);
    drawHouse(220, 200);
    drawHouse(300, 200);

    drawTree(760, 220, 1.0f);
    drawTree(860, 220, 0.9f);
    drawTree(960, 220, 1.1f);
    drawTree(80, 220, 1.0f);
    drawTree(130, 220, 1.0f);

    // Middleground
    drawRiver();
    drawBoat();
    drawBridge();
    // Foreground
    drawForegroundLand();

    drawTree1(70, 75, 1.4f);
    drawTree1(180, 45, 1.2f);
    drawTree1(720, 25, 1.3f);
    drawTree1(880, 70, 1.5f);

    // Auto car
    drawCar(carX, 220);
    drawCar(1000 - carX - 100, 220);
    // Key controlled player car
    drawPlayerCar(playerCarX, playerCarY);
    // Weather
    drawRain();
    glutSwapBuffers();
}

// TIMER for auto animation
void timer(int value)
{
    cloudX += 0.5f;
    birdX += 1.5f;
    billboardAnim += 0.05f;
    carX += 3.0f;
    if (carX > 1100) carX = -100;
    boatX -= 1.0f;
    if (boatX < -150) boatX = 1100;

    waveX += 0.5f;
    windAngle += 2.0f;    // Windmill spin
    if (windAngle >= 360.0f) windAngle -= 360.0f;

    if (isRaining)
    {
        for (int i = 0; i < MAX_RAIN; i++)
        {
            rain[i].y -= rain[i].speed;
            rain[i].x -= rain[i].speed * 0.3f;
            if (rain[i].y < 0)
            {
                rain[i].y = 700 + (rand() % 100);
                rain[i].x = rand() % 1300;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}
//  KEYBOARD Arrow keys move player car; D/N/L/R
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'd': case 'D': isDay = true;         break;
    case 'n': case 'N': isDay = false;        break;
    case 'l': case 'L': lightsOn = !lightsOn;    break;
    case 'r': case 'R': isRaining = !isRaining;  break;
    case 27: exit(0);                             break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y)
{
    // Arrow keys for move player car
    float step = 8.0f;
    switch (key)
    {
    case GLUT_KEY_LEFT:  playerCarX -= step; break;
    case GLUT_KEY_RIGHT: playerCarX += step; break;
    }
    // Clamp within window
    if (playerCarX < 0)    playerCarX = 0;
    if (playerCarX > 940)  playerCarX = 940;
    if (playerCarY < 0)    playerCarY = 0;
    if (playerCarY > 660)  playerCarY = 660;

    glutPostRedisplay();
}
//  INIT & MAIN
void init()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1000, 0, 700);

    for (int i = 0; i < MAX_RAIN; i++)
    {
        rain[i].x = (float)(rand() % 1300);
        rain[i].y = (float)(rand() % 700);
        rain[i].speed = 12.0f + (rand() % 10);
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 700);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("CG Projects 2D Animated City");
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
