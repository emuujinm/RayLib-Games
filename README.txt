This project is a collection of simple games developed using C/C++ and the Raylib graphics library as part of the Data Structures course. The games include both single-player and multiplayer modes.

Games:
- Ping Pong (2 players)
- Snake (1 player, runner style)
- Checkdrop (2 players, similar to Connect Four)

Library: [Raylib](https://www.raylib.com/)

1. Installation: [https://github.com/raysan5/raylib/wiki](https://github.com/raysan5/raylib/wiki)

2. Clone: git clone https://github.com/your-username/your-repo-name.git 
          cd your-repo-name

3. Compile:
   On Linux/macOS:
   gcc -o game main.c -lraylib -lm -ldl -lpthread -lGL -lrt -lX11
   ./game
   
   On Windows (using MinGW):
   gcc -o game.exe main.c -lraylib -lopengl32 -lgdi32 -lwinmm
   game.exe
