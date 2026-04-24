#include <stdio.h>
#include <raylib.h>
#include <string.h>
#include <time.h>

#define WIDTH 900
#define HEIGHT 600

#define RADIUS 20
#define LINETHICK 4

#define L1 250
#define L2 150

#define g 1500

#define TRACE_LEN 100
#define TRACE_THICKNESS 4

// trace pendulums
int trace_length = 0;
Vector2 trace[TRACE_LEN];

// variables required for solver
float l1, l2, phi1, phi2, phi1_d, phi2_d, phi1_dd, phi2_dd, m1, m2;

// start pos
Vector2 start = (Vector2) {WIDTH/2, 0};

// get end of the pendulum by passing start pos, length and phi
Vector2 get_end(Vector2 start, float l1, float phi1) 
{
    return (Vector2){start.x + l1 * sinf(phi1), start.y + l1 * cosf(phi1)};
}


void draw(float l, Vector2 start, float phi) 
{
    Vector2 end = get_end(start, l, phi);
    DrawLineEx(start, end, LINETHICK, WHITE);
    DrawCircleV(end, RADIUS, RED); 
}

void draw_double(Vector2 start, float phi1, float phi2, float l1, float l2) 
{
    Vector2 end_l1 = get_end(start, l1, phi1);
    Vector2 end_l2 = get_end(end_l1, l2, phi2);

    // draw pendulums
    draw(l2, end_l1, phi2);
    draw(l1, start, phi1);
    
    // draw trace
    if (trace_length < TRACE_LEN) trace_length++;
    Vector2 trace_copy[TRACE_LEN];

    memcpy(trace_copy, trace, TRACE_LEN*sizeof(Vector2));

    for (int i = 1; i < trace_length; i++) {
        trace[i] = trace_copy[i - 1]; 
    }
    trace[0] = end_l2;

    // visualize trace
    Color red = RED;
    for (int i = 0; i < trace_length; i++) {
        red.a = (unsigned char) ((float) 0xFF * ( 1 - (float) i / (float) trace_length));
        DrawRectangleV(trace[i], (Vector2){TRACE_THICKNESS, TRACE_THICKNESS}, red);
    }
}


void step(float dt) 
{
    phi1_dd = (-g*(2*m1+m2)*sinf(phi1) - m2*g*sinf(phi1-2*phi2) - 2*sinf(phi1-phi2)*m2*(phi2_d*phi2_d*l2 + phi1_d*phi1_d*l1*cosf(phi1-phi2)))
        / (l1*(2*m1+m2-m2*cosf(2*phi1-2*phi2)));

    phi2_dd = (2*sinf(phi1-phi2)*(phi1_d*phi1_d*l1*(m1+m2) + g*(m1+m2)*cosf(phi1) + phi2_d*phi2_d*l2*m2*cosf(phi1-phi2)))
        / (l2*(2*m1+m2-m2*cosf(2*phi1 - 2*phi2)));

    phi1_d += phi1_dd * dt;
    phi2_d += phi2_dd * dt;

    phi1 += phi1_d * dt;
    phi2 += phi2_d * dt;
}
void init_solver() {
    SetRandomSeed(time(NULL));
    l1 = L1;
    l2 = L2;

    phi1 = GetRandomValue(0, 360)*DEG2RAD;
    phi2 = GetRandomValue(0, 360)*DEG2RAD;

    phi1_d = 0;
    phi2_d = 0;

    m1 = 1;
    m2 = 1;
}

int main(int argc, char *argv[]) {
    InitWindow(WIDTH, HEIGHT, "Double Pendulum");
    
    SetTargetFPS(60);
    
    int frame = 0;

    init_solver();
    while (!WindowShouldClose()) {
        step(GetFrameTime() * 0.80f);
        
        if (IsKeyPressed(KEY_SPACE)) {
            init_solver();
            trace_length = 0;
        }
        BeginDrawing();
        ClearBackground(BLACK);
        draw_double(start, phi1, phi2, l1, l2);
        EndDrawing();

        Image img = LoadImageFromScreen();
        char filename[64];
        sprintf(filename, "frames/frame_%05d.png", frame++);
        ExportImage(img, filename);
        UnloadImage(img);
    }
    return 0;
}
