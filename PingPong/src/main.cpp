#include <raylib.h>
#include <iostream>

int player1_score = 0;
int player2_score = 0;
int maxScore = 15;
bool gameOverPlayed = false;

const char* player1Wins = "Player 1 won!";
const char* player2Wins = "Player 2 won!";
const char* gameOver = "GAME OVER";
const char* pressRestart = "Press ENTER to play again!";

class Ball {
public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    Texture2D headR;
    Texture2D headL;
    Texture2D current;
    Sound bounceSound;

    void Load() {
        headR = LoadTexture("resources/right.png");
        headL = LoadTexture("resources/left.png");
        current = headR;
        bounceSound = LoadSound("resources/score.wav");
    }

    void Unload() {
        UnloadTexture(headR);
        UnloadTexture(headL);
        UnloadSound(bounceSound);
    }

    void Draw() {
        DrawTexture(current, x - radius, y - radius, WHITE);
    }

    void Update() {
        x += speed_x;
        y += speed_y;

        if (y + radius >= GetScreenHeight() || y - radius <= 0) {
            speed_y *= -1;
        }

        if (x - radius <= 0) {
            player1_score++;
            ResetBall();
        }

        if (x + radius >= GetScreenWidth()) {
            player2_score++;
            ResetBall();
        }
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choices[2] = {-1, 1};
        speed_x *= speed_choices[GetRandomValue(0, 1)];
        speed_y *= speed_choices[GetRandomValue(0, 1)];
        current = (speed_x > 0) ? headR : headL;
    }
};

class Paddle {
protected:
    void LimitMovement() {
        if (y <= 0) {
            y = 0;
        }
        if (y + height >= GetScreenHeight()) {
            y = GetScreenHeight() - height;
        }
    }

public:
    float x, y;
    float width, height;
    int speed;

    void Draw() {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
    }

    void Update() {
        if (IsKeyDown(KEY_UP)) {
            y -= speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            y += speed;
        }
        LimitMovement();
    }
};

class LPaddle : public Paddle {
public:
    void Update() {
        if (IsKeyDown(KEY_W)) {
            y -= speed;
        }
        if (IsKeyDown(KEY_S)) {
            y += speed;
        }
        LimitMovement();
    }
};

Ball ball;
Paddle player1;
LPaddle player2;

void ResetGame() {
    player1_score = 0;
    player2_score = 0;
    gameOverPlayed = false;
    ball.ResetBall();
}

int main() {
    const int screen_width = 1280;
    const int screen_height = 800;

    InitWindow(screen_width, screen_height, "Ping Pong");
    InitAudioDevice();
    SetTargetFPS(70);

    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 7;
    ball.speed_y = 7;
    ball.Load();

    player1.width = 25;
    player1.height = 120;
    player1.x = screen_width - player1.width - 10;
    player1.y = screen_height / 2 - player1.height / 2;
    player1.speed = 12;

    player2.height = 120;
    player2.width = 25;
    player2.x = 10;
    player2.y = screen_height / 2 - player2.height / 2;
    player2.speed = 12;

    Sound gameOverSound = LoadSound("resources/gameOver.wav");

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (player1_score == maxScore || player2_score == maxScore) {
            ClearBackground(PURPLE);

            DrawText(gameOver, screen_width / 2 - MeasureText(gameOver, 60) / 2, 
                    screen_height / 2 - 140, 60, WHITE);

            if (player1_score == maxScore) {
                const char* result = player1Wins;
                DrawText(result, screen_width / 2 - MeasureText(result, 40) / 2, 
                        screen_height / 2 - 60, 40, WHITE);
            } else {
                const char* result = player2Wins;
                DrawText(result, screen_width / 2 - MeasureText(result, 40) / 2, 
                        screen_height / 2 - 60, 40, WHITE);
            }

            DrawText(pressRestart, screen_width / 2 - MeasureText(pressRestart, 20) / 2, 
                    screen_height / 2, 20, WHITE);

            if (!gameOverPlayed) {
                PlaySound(gameOverSound);
                gameOverPlayed = true;
            }

            if (IsKeyPressed(KEY_ENTER)) {
                ResetGame();
            }

            EndDrawing();
            continue;
        }

        ball.Update();
        player1.Update();
        player2.Update();

        if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius,
                                    {player1.x, player1.y, player1.width, player1.height})) {
            ball.speed_x *= -1;
            ball.current = ball.headL;
            PlaySound(ball.bounceSound);
        }

        if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius,
                                    {player2.x, player2.y, player2.width, player2.height})) {
            ball.speed_x *= -1;
            ball.current = ball.headR;
            PlaySound(ball.bounceSound);
        }

        ClearBackground(PINK);
        DrawRectangle(screen_width / 2, 0, screen_width, screen_height, GREEN);
        DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);

        ball.Draw();
        player1.Draw();
        player2.Draw();

        DrawText(TextFormat("%i", player2_score), screen_width / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i", player1_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);

        EndDrawing();
    }

    ball.Unload();
    UnloadSound(gameOverSound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}