#include "raylib.h"

// This define must appear before the include in exactly one C file
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <stdlib.h>
#include <time.h>
#include <math.h>

// --- Configuration ---
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CELL_SIZE 20
#define MAX_SNAKE_LENGTH 200
#define MAX_PARTICLES 100

// --- Game States ---
typedef enum { STATE_MENU, STATE_PLAYING, STATE_PAUSED, STATE_GAMEOVER } GameState;

// --- Structs ---
typedef struct {
    Vector2 body[MAX_SNAKE_LENGTH];
    int length;
    Vector2 direction;
} Snake;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float life;
    Color color;
    bool active;
} Particle;

// --- Globals ---
Particle particles[MAX_PARTICLES];
GameState currentState = STATE_MENU;
int score = 0;
float gameSpeed = 12.0f; // Initial speed, modified by raygui slider

// --- Logic Functions ---
Vector2 GetRandomPosition() {
    int gridX = rand() % (SCREEN_WIDTH / CELL_SIZE);
    int gridY = rand() % (SCREEN_HEIGHT / CELL_SIZE);
    return (Vector2){ (float)(gridX * CELL_SIZE), (float)(gridY * CELL_SIZE) };
}

void ResetGame(Snake *snake, Vector2 *food) {
    snake->length = 1;
    snake->body[0] = (Vector2){400, 300};
    snake->direction = (Vector2){CELL_SIZE, 0};
    *food = GetRandomPosition();
    score = 0;
}

void SpawnParticles(Vector2 pos) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = true;
        particles[i].position = pos;
        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
        float speed = (float)GetRandomValue(50, 150) / 10.0f;
        particles[i].velocity = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };
        particles[i].life = 1.0f;
        particles[i].color = (Color){255, GetRandomValue(100,255), 0, 255};
    }
}

int main() {
    // Standard initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Game + raygui");
    SetTargetFPS(60); // 60 FPS for smooth UI; game logic is on a timer
    srand(time(NULL));

    Snake snake;
    Vector2 food;
    ResetGame(&snake, &food);

    float moveTimer = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // 1. UPDATE LOGIC
        if (currentState == STATE_PLAYING) {
            // Controls
            if (IsKeyPressed(KEY_UP) && snake.direction.y == 0) snake.direction = (Vector2){0, -CELL_SIZE};
            if (IsKeyPressed(KEY_DOWN) && snake.direction.y == 0) snake.direction = (Vector2){0, CELL_SIZE};
            if (IsKeyPressed(KEY_LEFT) && snake.direction.x == 0) snake.direction = (Vector2){-CELL_SIZE, 0};
            if (IsKeyPressed(KEY_RIGHT) && snake.direction.x == 0) snake.direction = (Vector2){CELL_SIZE, 0};
            if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) currentState = STATE_PAUSED;

            moveTimer += dt;
            if (moveTimer >= (1.0f / gameSpeed)) {
                // Move body
                for (int i = snake.length - 1; i > 0; i--) snake.body[i] = snake.body[i - 1];
                
                // Move head
                snake.body[0].x += snake.direction.x;
                snake.body[0].y += snake.direction.y;
                moveTimer = 0;

                // Screen wrap
                if (snake.body[0].x < 0) snake.body[0].x = SCREEN_WIDTH - CELL_SIZE;
                if (snake.body[0].x >= SCREEN_WIDTH) snake.body[0].x = 0;
                if (snake.body[0].y < 0) snake.body[0].y = SCREEN_HEIGHT - CELL_SIZE;
                if (snake.body[0].y >= SCREEN_HEIGHT) snake.body[0].y = 0;

                // Self-collision check
                for (int i = 1; i < snake.length; i++) {
                    if (snake.body[0].x == snake.body[i].x && snake.body[0].y == snake.body[i].y) {
                        currentState = STATE_GAMEOVER;
                    }
                }

                // Food collision
                if (snake.body[0].x == food.x && snake.body[0].y == food.y) {
                    if (snake.length < MAX_SNAKE_LENGTH) snake.length++;
                    SpawnParticles((Vector2){ food.x + CELL_SIZE/2, food.y + CELL_SIZE/2 });
                    food = GetRandomPosition();
                    score++;
                }
            }
        }

        // Particle logic (Always active for visuals)
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (particles[i].active) {
                particles[i].life -= dt;
                if (particles[i].life <= 0) particles[i].active = false;
                else {
                    particles[i].position.x += particles[i].velocity.x;
                    particles[i].position.y += particles[i].velocity.y;
                    particles[i].velocity.x *= 0.95f;
                    particles[i].velocity.y *= 0.95f;
                }
            }
        }

        // 2. DRAWING
        BeginDrawing();
        ClearBackground(DARKGREEN);

        // Draw Snake
        for (int i = 0; i < snake.length; i++) {
            DrawRectangleV(snake.body[i], (Vector2){CELL_SIZE, CELL_SIZE}, (i == 0) ? SKYBLUE : BLUE);
        }

        // Draw Food (Your pulsing effect)
        float pulse = (sinf(GetTime() * 4.0f) + 1.0f) / 2.0f;
        DrawCircle(food.x + CELL_SIZE/2, food.y + CELL_SIZE/2, (CELL_SIZE/3) + pulse * 5, RED);

        // Draw Particles
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (particles[i].active) DrawCircleV(particles[i].position, 3, Fade(particles[i].color, particles[i].life));
        }

        // --- UI OVERLAYS ---
        if (currentState == STATE_MENU) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.6f));
            GuiPanel((Rectangle){ 250, 150, 300, 320 }, "MAIN MENU");
            
            if (GuiButton((Rectangle){ 300, 210, 200, 40 }, "START GAME")) currentState = STATE_PLAYING;
            
            // Difficulty Control
            GuiLabel((Rectangle){ 300, 270, 200, 20 }, "Snake Speed:");
            GuiSliderBar((Rectangle){ 300, 290, 200, 20 }, "Slow", "Insane", &gameSpeed, 5, 40);
            
            if (GuiButton((Rectangle){ 300, 390, 200, 40 }, "EXIT")) break;
        }

        if (currentState == STATE_PAUSED) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.4f));
            if (GuiButton((Rectangle){ 300, 250, 200, 50 }, "RESUME")) currentState = STATE_PLAYING;
            if (GuiButton((Rectangle){ 300, 320, 200, 50 }, "QUIT TO MENU")) currentState = STATE_MENU;
        }

        if (currentState == STATE_GAMEOVER) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(MAROON, 0.7f));
            DrawText("SNAKE DIED!", 320, 180, 30, RAYWHITE);
            DrawText(TextFormat("FINAL SCORE: %d", score), 335, 220, 20, YELLOW);
            
            if (GuiButton((Rectangle){ 300, 280, 200, 50 }, "TRY AGAIN")) {
                ResetGame(&snake, &food);
                currentState = STATE_PLAYING;
            }
            if (GuiButton((Rectangle){ 300, 350, 200, 50 }, "MENU")) currentState = STATE_MENU;
        }

        // Persistent HUD
        DrawText(TextFormat("Score: %d", score), 15, 15, 20, WHITE);
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}