#include <iostream> 
#include <raylib.h>
#include <deque>
#include <raymath.h>
using namespace std;

Color cyan = {39, 245, 212, 255};
Color blue = {34, 93, 180, 255};
Color orange = {250, 187, 138, 255};

int cellSize = 30;
int cellCountX = 12;
int cellCountY = cellCountX * 2;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(deque[i], element)) {
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Cat {
public:
    deque<Vector2> body = {
        Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}
    };
    Vector2 direction = {1, 0};
    bool addSegment = false;

    Texture2D headR;
    Texture2D headU;
    Texture2D headD;
    Texture2D headL;

    Cat() {
        Image headRight = LoadImage("resources/headR.png");
        headR = LoadTextureFromImage(headRight);
        UnloadImage(headRight);

        Image headUp = LoadImage("resources/headU.png");
        headU = LoadTextureFromImage(headUp);
        UnloadImage(headUp);

        Image headDown = LoadImage("resources/headD.png");
        headD = LoadTextureFromImage(headDown);
        UnloadImage(headDown);

        Image headLeft = LoadImage("resources/headL.png");
        headL = LoadTextureFromImage(headLeft);
        UnloadImage(headLeft);
    }

    ~Cat() {
        UnloadTexture(headR);
        UnloadTexture(headU);
        UnloadTexture(headD);
        UnloadTexture(headL);
    }

    void DrawBodySegment(float x, float y, float cellSize, Color color) {
        float corner = 5.0f;
        
        Rectangle middle = {x + corner, y, cellSize - 2 * corner, cellSize};
        DrawRectangleRec(middle, color);

        middle = {x, y + corner, cellSize, cellSize - 2 * corner};
        DrawRectangleRec(middle, color);
        
        DrawCircle(x + corner, y + corner, corner, color);
        DrawCircle(x + cellSize - corner, y + corner, corner, color);
        DrawCircle(x + corner, y + cellSize - corner, corner, color);
        DrawCircle(x + cellSize - corner, y + cellSize - corner, corner, color);
    }

    void Draw() {
        float headX = offset + body[0].x * cellSize;
        float headY = offset + body[0].y * cellSize;

        if (direction.x == 1 && direction.y == 0) {
            DrawTexture(headR, (int)headX, (int)headY, WHITE);
        } else if (direction.x == -1 && direction.y == 0) {
            DrawTexture(headL, (int)headX, (int)headY, WHITE);
        } else if (direction.x == 0 && direction.y == 1) {
            DrawTexture(headU, (int)headX, (int)headY, WHITE);
        } else if (direction.x == 0 && direction.y == -1) {
            DrawTexture(headD, (int)headX, (int)headY, WHITE);
        }

        for (unsigned int i = 1; i < body.size(); i++) {
            float x = offset + body[i].x * cellSize;
            float y = offset + body[i].y * cellSize;
            DrawBodySegment(x, y, cellSize, orange);
        }
    }

    void Update() {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment) {
            addSegment = false;
        } else {
            body.pop_back();
        }
    }

    void Reset() {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    }
};

class Food {
    public:
    Vector2 position;
    Texture2D texture;

    Food(deque<Vector2> snakeBody) {
        position = {5, 6};
        Image image = LoadImage("resources/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food() {
        UnloadTexture(texture);
    }

    void Draw() {
        DrawTexture(texture, offset  + position.x * cellSize, offset  + position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell() {
        float x = GetRandomValue(0, cellCountX - 1);
        float y = GetRandomValue(0, cellCountY - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody) {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody)) {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game {
public:
    Cat cat = Cat();
    Food food = Food(cat.body);
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    Game() {
       InitAudioDevice(); 
       eatSound = LoadSound("resources/Sound/eat.mp3");
       wallSound = LoadSound("resources/Sound/wall.mp3");
    }

    ~Game() {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
    }

    void Draw() {
        food.Draw();
        cat.Draw();
    }

    void CheckCollisionWithFood() {
        if (Vector2Equals(cat.body[0], food.position)) {
            food.position = food.GenerateRandomPos(cat.body );
            cat.addSegment = true;
            score++;
            PlaySound(eatSound);
        }

    }

    void CheckCollisionWithEdges() {
        if (cat.body[0].x == cellCountX || cat.body[0].x == -1) {
            GameOver();
        } 
        if (cat.body[0].y == cellCountY || cat.body[0].y == -1) {
            GameOver();
        }
    }

    void Update() {
        if (running) {
            cat.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }

    void GameOver() {
        cat.Reset();
        food.position = food.GenerateRandomPos(cat.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }

    void CheckCollisionWithTail() {
        deque<Vector2> headlessBody = cat.body;
        headlessBody.pop_front();
        if (ElementInDeque(cat.body[0], headlessBody)) {
            GameOver();
        }
    }
};

int main() {
    InitWindow(2 * offset + cellSize * cellCountX, 2 * offset + cellSize * cellCountY, "Cat");
    SetTargetFPS(60);

    Game game = Game();

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (eventTriggered(0.25)) {
            game.Update();
        }

        if (IsKeyPressed(KEY_UP) && game.cat.direction.y != 1) {
            game.cat.direction = {0, -1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.cat.direction.y != -1) {
            game.cat.direction = {0, 1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.cat.direction.x != 1) {
            game.cat.direction = {-1, 0};
            game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.cat.direction.x != -1) {
            game.cat.direction = {1, 0};
            game.running = true;
        }

        ClearBackground(cyan);
        DrawRectangleLinesEx((Rectangle){offset - 5, offset - 5, cellSize * cellCountX + 10, cellSize * cellCountY + 10}, 5, BLUE);
        DrawText("Cat", offset - 5, 20, 40, blue);
        DrawText(TextFormat("Score: %i", game.score), offset - 5, offset + cellSize * cellCountY +10, 40, blue);
        game.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}