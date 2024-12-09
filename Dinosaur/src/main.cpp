#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <raylib.h>

const char* gameOverText = "GAME OVER!";
const char* restartText = "Press ENTER to restart";

const int screenWidth = 800;
const int screenHeight = 450;

int score = 0;
float scoreTime = 0;

typedef struct Cat {
    Texture2D tex;
    Vector2 pos;
    Rectangle cut[4];

    int frame;
    int cframe;
    int w;
    int h;
    long double time;
    float fps;

    Sound jumpSound;
    Sound gameOverSound;

    void Load() {
        tex = LoadTexture("resources/cat.png");
        jumpSound = LoadSound("resources/jumpSound.wav");
        gameOverSound = LoadSound("resources/gameOverSound.wav");
    }

    void Unload() {
        UnloadTexture(tex);
        UnloadSound(jumpSound);
        UnloadSound(gameOverSound);
    }
} Cat;

class Bat: public Cat {
public:
    void Load() {
        tex = LoadTexture("resources/bat.png");
    }

    void Unload() {
        UnloadTexture(tex);
    }
};

typedef struct Cactus {
    Texture2D tex;

    Rectangle cut[60];
    Vector2 pos[60];

    int size;
    int w;
    int h;
    int speed;

    void Load() {
        tex = LoadTexture("resources/cactus.png");
    }

    void Unload() {
        UnloadTexture(tex);
    }
} Cactus;

void animate(Cat* cat);
bool bat_fly(Cat* cat, int speed);
void gameReset(Cat* cat, Cat* bat, Cactus* cactus, 
                bool* jump, bool* batfly, bool* gameOver, 
                float* jumpHeight, long double* speedUpTime, 
                long double* batTime, int* cactusSpeed);

int main(void) {
    srand(time(NULL));

    InitWindow(screenWidth, screenHeight, "Catsaur game");
    InitAudioDevice();
    SetTargetFPS(30);

    Cat cat; 
    cat.Load();
    cat.frame = 3;
    cat.cframe = 0;
    cat.w = 62;
    cat.h = 53;
    cat.pos = (Vector2) {screenWidth / 4, screenHeight * 0.7 - cat.h };
    for (int i = 0; i < cat.frame; i++) {
        cat.cut[i] = (Rectangle) { cat.w * i, 0, 
                        cat.w, cat.h };
    }
    cat.time = 0;
    cat.fps = 0.16;

    Bat bat; 
    bat.Load();
    bat.frame = 4;
    bat.cframe = 0;
    bat.w = 31;
    bat.h = 22;
    bat.pos = (Vector2) {(screenWidth * 3) / 4, screenHeight * 0.6 };
    for (int i = 0; i < bat.frame; i++) {
        bat.cut[i] = (Rectangle) { bat.w * i, 0, 
                            bat.w, bat.h };
    }
    bat.time = 0;
    bat.fps = 0.07;

    Cactus cactus; 
    cactus.Load();  
    
    cactus.size = sizeof(cactus.cut) / sizeof(cactus.cut[0]);
    cactus.w = 52;
    cactus.h = 64;
    cactus.pos[0] = (Vector2) { screenWidth, screenHeight * 0.7 - cactus.h };
    cactus.cut[0] = (Rectangle) { (rand() % 3) * cactus.w, 0, 
                        cactus.w, cactus.h };
    for (int i = 1; i < cactus.size; i++) {
        cactus.pos[i] = (Vector2) { cactus.pos[i - 1].x + rand() % screenWidth + screenWidth / 2, 
                        screenHeight * 0.7 - cactus.h };
        cactus.cut[i] = (Rectangle) { (rand() % 3) * cactus.w, 0, 
                        cactus.w, cactus.h };
    }
    cactus.speed = 9;

    float jumpInit = 24;
    float jumpHeight = jumpInit;
    float jumpChange = 2.2;

    int batSpeed = cactus.speed;
    long double speedUpTime = 0;
    long double batTime = 0;

    bool jump = false;
    bool batfly = false;
    bool gameOver = false;

    while (!WindowShouldClose()) {
        if (gameOver && IsKeyPressed(KEY_ENTER)) {
            gameReset(&cat, &bat, &cactus, &jump, &batfly, &gameOver, 
                        &jumpHeight, &speedUpTime, &batTime, &cactus.speed);
            score = 0;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            jump = true;
            PlaySound(cat.jumpSound);
        }

        if (!gameOver) {
            animate(&cat);
            animate(&bat);

            if (jump) {
                cat.pos.y -= jumpHeight;
                jumpHeight -= jumpChange;

                if (cat.pos.y > screenHeight * 0.7 - cat.h) {
                    cat.pos.y = screenHeight * 0.7 - cat.h;
                    jumpHeight = jumpInit;
                    jump = false;
                }
            }

            if (batfly) {
                batfly = bat_fly(&bat, batSpeed);
                batTime = 0;
            }

            for (int i = 0; i < cactus.size; i++) {
                if (cactus.pos[i].x < -cactus.w * 2) {
                    continue;
                }
                cactus.pos[i].x -= cactus.speed;

                if (cactus.pos[i].x + cactus.w < 0) {
                    score++;
                }
            }

            speedUpTime += GetFrameTime() / 5;
            if (speedUpTime > 1.0) {
                cactus.speed += 1;
                speedUpTime = 0;
            } 

            batTime += GetFrameTime();
            if (batTime > 6) {
                batSpeed = cactus.speed + 1;
                batfly = true;
            }

            scoreTime += GetFrameTime();
            if (scoreTime >= 1.0f) {
                score++;
                scoreTime = 0;
            }

            for (int i = 0; i < cactus.size; i++) {
                if (cactus.pos[i].x > 0 && cactus.pos[i].x < screenWidth) {
                    if (CheckCollisionRecs((Rectangle) { cat.pos.x, cat.pos.y, cat.w, cat.h },
                                           (Rectangle) { cactus.pos[i].x, cactus.pos[i].y, cactus.w, cactus.h })) {
                        gameOver = true;
                    }
                }
            }

            if (batfly) {
                if (CheckCollisionRecs((Rectangle) { cat.pos.x, cat.pos.y, cat.w, cat.h },
                                       (Rectangle) { bat.pos.x, bat.pos.y, bat.w, bat.h })) {
                    gameOver = true;
                    PlaySound(cat.gameOverSound);                
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLineEx((Vector2) { 0, screenHeight * 0.7 }, (Vector2) { screenWidth, screenHeight * 0.7 }, 5, BLACK);

        DrawTextureRec(cat.tex, cat.cut[cat.cframe], cat.pos, WHITE);

        if (batfly) {
            DrawTextureRec(bat.tex, bat.cut[bat.cframe], bat.pos, WHITE);
        }

        for (int i = 0; i < cactus.size; i++) {
            if (cactus.pos[i].x > -cactus.w && cactus.pos[i].x < screenWidth) {
                DrawTextureRec(cactus.tex, cactus.cut[i], cactus.pos[i], WHITE);
            }
        }

        DrawText(TextFormat("Score: %d", score), 10, 10, 20, DARKGRAY);

        if (gameOver) {
            const char* result = gameOverText;
            DrawText(result, screenWidth / 2 - MeasureText(result, 60) / 2, 
                        screenHeight / 2 - 30, 60, DARKGRAY);

            result = restartText;
            DrawText(result, screenWidth / 2 - MeasureText(result, 30) / 2, 
                        screenHeight / 2 - 60, 30, DARKGRAY);
        }

        EndDrawing();
    }

    cat.Unload();
    bat.Unload();
    cactus.Unload();
    CloseAudioDevice();
    
    CloseWindow();
    return 0;
}

void animate(Cat* s) {
    s->time += GetFrameTime();
    if (s->time > s->fps) {
        s->cframe++;
        s->time = 0;
    }

    if (s->cframe >= s->frame) {
        s->cframe = 0;
    }
}

bool bat_fly(Cat* cat, int speed) {
    cat->pos.x -= speed;

    if (cat->pos.x < -cat->w) {
        int range = GetScreenHeight() * 0.7 - GetScreenHeight() * 0.5;
        int y = rand() % range + GetScreenHeight() * 0.45;
        cat->pos = (Vector2) { GetScreenWidth(), y };
        return false;
    }

    return true;
}

void gameReset(Cat* cat, Cat* bat, Cactus* cactus, bool* jump, 
                bool* batfly, bool* gameOver, float* jumpHeight, long double* speedUpTime, 
                long double* batTime, int* cactusSpeed) {
    cat->pos = (Vector2) { 150, GetScreenHeight() * 0.7 - cat->h };
    bat->pos = (Vector2) { 700, GetScreenHeight() * 0.6 };

    cactus->pos[0] = (Vector2) { GetScreenWidth(), GetScreenHeight() * 0.7 - cactus->h };
    for (int i = 1; i < cactus->size; i++) {
        cactus->pos[i] = (Vector2) { cactus->pos[i - 1].x + rand() % GetScreenWidth() + GetScreenWidth() / 2, 
                            GetScreenHeight() * 0.7 - cactus->h };
    }

    *jump = false;
    *batfly = false;
    *gameOver = false;

    *jumpHeight = 24;
    *speedUpTime = 0;
    *batTime = 0;
    *cactusSpeed = 9;
}