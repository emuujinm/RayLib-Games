#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <raylib.h>

typedef struct Running {
    Texture2D tex;
    Vector2 pos;
    Rectangle srect[4];
    int frames;
    int currentFrame;
    int actualWidth;
    int actualHeight;
    long double time;
    float animationTime;
} Running;

typedef struct Cactus {
    Texture2D tex;
    Rectangle srect[60];
    Vector2 pos[60];
    int size;
    int actualWidth;
    int actualHeight;
    int speed;
} Cactus;

void sprite_animate(Running* s);
bool catbat_fly(Running* bat, int speed);
void GameInit(Running* cat, Cactus* cactus, Running* bat, int screenWidth, int screenHeight);
void ResetGame(Running* cat, Cactus* cactus, Running* bat, bool* gameOver, bool* soundPlayed, int* score, int screenWidth, int screenHeight);

bool batfly = false;

void UpdateDrawFrame(Running* cat, Running* bat, Cactus* cactus, bool* gameOver, Sound gameOverSound, bool* soundPlayed, int* score, int screenWidth, int screenHeight) {
    if (!(*gameOver)) {
        sprite_animate(cat);
        sprite_animate(bat);

        static float jump_v = 24;
        static float dv = jump_v;
        static float dg = 2.2;
        static bool jump = false;

        if (IsKeyPressed(KEY_SPACE)) jump = true;
        if (jump) {
            cat->pos.y -= dv;
            dv -= dg;
            if (cat->pos.y > GetScreenHeight() * 0.7 - cat->actualHeight) {
                cat->pos.y = GetScreenHeight() * 0.7 - cat->actualHeight;
                dv = jump_v;
                jump = false;
            }
        }

        for (int i = 0; i < cactus->size; i++) {
            if (cactus->pos[i].x >= -cactus->actualWidth * 2) {
                cactus->pos[i].x -= cactus->speed;
            }
        }

        static long double batFlyTime = 0;
        batFlyTime += GetFrameTime();
        if (batFlyTime > 6) {
            batfly = catbat_fly(bat, cactus->speed + 1);
            batFlyTime = 0;
        }

        for (int i = 0; i < cactus->size; i++) {
            if (CheckCollisionRecs((Rectangle){cat->pos.x, cat->pos.y, cat->actualWidth, cat->actualHeight},
                                   (Rectangle){cactus->pos[i].x, cactus->pos[i].y, cactus->actualWidth, cactus->actualHeight})) {
                *gameOver = true;
            }
        }

        if (batfly) {
            if (CheckCollisionRecs(
                (Rectangle){cat->pos.x, cat->pos.y, cat->actualWidth, cat->actualHeight},
                (Rectangle){bat->pos.x, bat->pos.y, bat->actualWidth, bat->actualHeight}
            )) {
                *gameOver = true;
            }
        }

        *score += 1;
    }

    if (*gameOver && !(*soundPlayed)) {
        PlaySound(gameOverSound);
        *soundPlayed = true;
    }

    if (*gameOver && IsKeyPressed(KEY_ENTER)) {
        ResetGame(cat, cactus, bat, gameOver, soundPlayed, score, screenWidth, screenHeight);
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawLineEx((Vector2){0, GetScreenHeight() * 0.7}, (Vector2){GetScreenWidth(), GetScreenHeight() * 0.7}, 5, BLACK);
    DrawTextureRec(cat->tex, cat->srect[cat->currentFrame], cat->pos, WHITE);
    if (batfly) {
        DrawTextureRec(bat->tex, bat->srect[bat->currentFrame], bat->pos, WHITE);
    }
    for (int i = 0; i < cactus->size; i++) {
        if (cactus->pos[i].x >= -cactus->actualWidth && cactus->pos[i].x <= GetScreenWidth()) {
            DrawTextureRec(cactus->tex, cactus->srect[i], cactus->pos[i], WHITE);
        }
    }
    if (*gameOver) {
        DrawText("Press Enter to restart", GetScreenWidth() * 0.25, GetScreenHeight() * 0.3, 40, LIGHTGRAY);
    }

    DrawText(TextFormat("Score: %d", *score), 10, 10, 20, BLACK);

    EndDrawing();
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    srand(time(NULL));

    InitWindow(screenWidth, screenHeight, "Catsaur game");
    SetTargetFPS(30);

    Running cat, bat;
    Cactus cactus;
    GameInit(&cat, &cactus, &bat, screenWidth, screenHeight);

    Sound gameOverSound = LoadSound("resources/gameOver.wav");

    bool gameOver = false;
    bool soundPlayed = false;
    int score = 0;

    while (!WindowShouldClose()) {
        UpdateDrawFrame(&cat, &bat, &cactus, &gameOver, gameOverSound, &soundPlayed, &score, screenWidth, screenHeight);
    }

    UnloadTexture(cat.tex);
    UnloadTexture(bat.tex);
    UnloadTexture(cactus.tex);
    UnloadSound(gameOverSound);
    CloseWindow();
    return 0;
}

void GameInit(Running* cat, Cactus* cactus, Running* bat, int screenWidth, int screenHeight) {
    cat->tex = LoadTexture("resources/cat.png");
    cat->frames = 3;
    cat->currentFrame = 0;
    cat->actualWidth = 62;
    cat->actualHeight = 53;
    cat->pos = (Vector2){150, screenHeight * 0.7 - cat->actualHeight};
    for (int i = 0; i < cat->frames; i++) {
        cat->srect[i] = (Rectangle){cat->actualWidth * i, 0, cat->actualWidth, cat->actualHeight};
    }
    cat->time = 0;
    cat->animationTime = 0.14;

    bat->tex = LoadTexture("resources/bat.png");
    bat->frames = 4;
    bat->currentFrame = 0;
    bat->actualWidth = 31;
    bat->actualHeight = 22;
    bat->pos = (Vector2){700, screenHeight * 0.6};
    for (int i = 0; i < bat->frames; i++) {
        bat->srect[i] = (Rectangle){bat->actualWidth * i, 0, bat->actualWidth, bat->actualHeight};
    }
    bat->time = 0;
    bat->animationTime = 0.07;

    cactus->tex = LoadTexture("resources/cactus.png");
    cactus->size = sizeof(cactus->srect) / sizeof(cactus->srect[0]);
    cactus->actualWidth = cactus->actualHeight = 64;
    cactus->speed = 9;
    cactus->pos[0] = (Vector2){screenWidth, screenHeight * 0.7 - cactus->actualHeight};
    cactus->srect[0] = (Rectangle){(rand() % 3) * cactus->actualWidth, 0, cactus->actualWidth, cactus->actualHeight};
    for (int i = 1; i < cactus->size; i++) {
        cactus->pos[i] = (Vector2){cactus->pos[i - 1].x + rand() % screenWidth + screenWidth / 2, screenHeight * 0.7 - cactus->actualHeight};
        cactus->srect[i] = (Rectangle){(rand() % 3) * cactus->actualWidth, 0, cactus->actualWidth, cactus->actualHeight};
    }
}

void ResetGame(Running* cat, Cactus* cactus, Running* bat, bool* gameOver, bool* soundPlayed, int* score, int screenWidth, int screenHeight) {
    *gameOver = false;
    *soundPlayed = false;
    *score = 0;
    batfly = false;
    GameInit(cat, cactus, bat, screenWidth, screenHeight);
}

void sprite_animate(Running* s) {
    s->time += GetFrameTime();
    if (s->time > s->animationTime) {
        s->currentFrame++;
        s->time = 0;
    }
    if (s->currentFrame >= s->frames) {
        s->currentFrame = 0;
    }
}

bool catbat_fly(Running* s, int speed) {
    s->pos.x -= speed;

    if (s->pos.x < -s->actualWidth) {
        int range = GetScreenHeight() * 0.7 - GetScreenHeight() * 0.5;
        int y = rand() % range + GetScreenHeight() * 0.45;
        s->pos = (Vector2){GetScreenWidth(), y};
        return false;
    }
    return true;
}