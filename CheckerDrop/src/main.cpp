#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define cellCount 10
#define maxScore 4

const int screenWidth = 800;
const int screenHeight = 800;
const int cellWidth = screenWidth / cellCount;
const int cellHeight = screenHeight / cellCount;

typedef enum CellValue
{
    EMPTY,
    X,
    O
} CellValue;

typedef struct Cell
{
    int i;
    int j;
    CellValue value;
} Cell;

Cell grid[cellCount][cellCount];

int tilesOccupied;
typedef enum GameState
{
    PLAYERX,
    PLAYERO,
    END
} GameState;

GameState state;
CellValue winner;

void CellDraw(Cell);
void GridInit(void);
void GameInit(void);
bool IndexIsValid(int, int);
bool CellMark(int, CellValue);
bool GameIsOver(CellValue*);
void UpdateDrawFrame(void);

void DrawTable()
{
    for (int j = 1; j < cellCount; ++j)
    {
        for (int i = 0; i < cellCount; ++i)
        {
            DrawRectangleLines( i * cellWidth, j * cellHeight, 
                                cellWidth, cellHeight, BLACK);
        }
    }
}

void CellDraw(Cell cell)
{
    switch (cell.value)
    {
        case X:
            DrawCircle(cell.i * cellWidth + cellWidth / 2, 
                       cell.j * cellHeight + cellHeight / 2, 
                       cellWidth / 2 - 5, RED);
            break;
        case O:
            DrawCircle(cell.i * cellWidth + cellWidth / 2, 
                       cell.j * cellHeight + cellHeight / 2, 
                       cellWidth / 2 - 5, YELLOW);
            break;
        default:
            break;
    }
}

void GridInit(void)
{
    for (int i = 0; i < cellCount; i++)
    {
        for (int j = 0; j < cellCount; j++)
        {
            grid[j][i] = (Cell)
            {
                .i = i,
                .j = j,
                .value = EMPTY
            };
        }
    }
}

void GameInit(void)
{
    GridInit();
    state = PLAYERX;
    tilesOccupied = 0;
}

bool IndexIsValid(int i, int j)
{
    return i >= 0 && i < cellCount && j >= 0 && j < cellCount;
}

bool CellMark(int column, CellValue value)
{
    for (int row = cellCount - 1; row >= 0; row--)
    {
        if (grid[row][column].value == EMPTY)
        {
            grid[row][column].value = value;
            tilesOccupied++;
            return true;
        }
    }
    return false;
}

bool GameIsOver(CellValue* winner)
{
    for (int i = 0; i < cellCount; i++)
    {
        for (int j = 0; j < cellCount; j++)
        {
            if (grid[i][j].value != EMPTY)
            {
                if (j <= cellCount - maxScore &&
                    grid[i][j].value == grid[i][j + 1].value &&
                    grid[i][j].value == grid[i][j + 2].value &&
                    grid[i][j].value == grid[i][j + 3].value)
                {
                    *winner = grid[i][j].value;
                    return true;
                }

                if (i <= cellCount - maxScore &&
                    grid[i][j].value == grid[i + 1][j].value &&
                    grid[i][j].value == grid[i + 2][j].value &&
                    grid[i][j].value == grid[i + 3][j].value)
                {
                    *winner = grid[i][j].value;
                    return true;
                }

                if (i <= cellCount - maxScore && 
                    j <= cellCount - maxScore &&
                    grid[i][j].value == grid[i + 1][j + 1].value &&
                    grid[i][j].value == grid[i + 2][j + 2].value &&
                    grid[i][j].value == grid[i + 3][j + 3].value)
                {
                    *winner = grid[i][j].value;
                    return true;
                }

                if (i <= cellCount - maxScore && j >= 3 &&
                    grid[i][j].value == grid[i + 1][j - 1].value &&
                    grid[i][j].value == grid[i + 2][j - 2].value &&
                    grid[i][j].value == grid[i + 3][j - 3].value)
                {
                    *winner = grid[i][j].value;
                    return true;
                }
            }
        }
    }

    *winner = EMPTY;
    return false;
}

const char* playerXWins = "RED PLAYER WON!";
const char* playerOWins = "YELLOW PLAYER WON!";
const char* pressRestart = "Press ENTER to play again!";
const char* playerXTurnPrompt = "RED GO";
const char* playerOTurnPrompt = "YELLOW GO";


void UpdateDrawFrame(Sound gameOverSound)
{
    static bool soundPlayed = false;

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && state != END)
    {
        Vector2 mPos = GetMousePosition();
        int column = mPos.x / cellWidth;

        if (IndexIsValid(0, column)) {
            if (CellMark(column, state == PLAYERX ? X : O)) {
                if (!GameIsOver(&winner)) {
                    state = (state == PLAYERX) ? PLAYERO : PLAYERX;
                }
                else {
                    state = END;
                    soundPlayed = false;
                }
            }
        }
    }

    if (state == END && !soundPlayed)
    {
        PlaySound(gameOverSound);
        soundPlayed = true;
    }

    if (state == END && IsKeyPressed(KEY_ENTER))
    {
        GameInit();
        soundPlayed = false;
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawTable();
    for (int i = 0; i < cellCount; i++)
    {
        for (int j = 0; j < cellCount; j++)
        {
            CellDraw(grid[i][j]);
        }
    }

    if (state == END)
    {
        const char* result = (winner == X) ? playerXWins : playerOWins;
        DrawText(result, screenWidth / 2 - MeasureText(result, 40) / 2, 
                 screenHeight / 2 - 60, 40, DARKGRAY);
        DrawText(   pressRestart, screenWidth / 2 - MeasureText(pressRestart, 20) / 2, 
                    screenHeight / 2, 20, GRAY);
    }
    else
    {
        const char* prompt = (state == PLAYERX) ? playerXTurnPrompt : playerOTurnPrompt;
        DrawText(prompt, screenWidth / 2 - MeasureText(prompt, 20) / 2, 10, 20, DARKGRAY);
    }

    EndDrawing();
}

int main()
{
    srand(time(0));
    
    InitWindow(screenWidth, screenHeight, "Checker Drop");
    InitAudioDevice();

    Sound gameOverSound = LoadSound("resources/gameOver.wav");
    GameInit();

    while (!WindowShouldClose())
    {
        UpdateDrawFrame(gameOverSound);
    }

    UnloadSound(gameOverSound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}