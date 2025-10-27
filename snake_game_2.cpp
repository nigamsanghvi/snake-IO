#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

const int WIDTH = 20;   // smaller size since emojis take more space
const int HEIGHT = 10;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

struct Point {
    int x, y;
};

bool operator==(const Point &p1, const Point &p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

class SnakeGame {
private:
    vector<Point> snake;
    Point fruit;
    string currentFruit;
    vector<string> fruitSymbols = {"🍎", "🍒", "🍉", "🥭", "🍓"};
    Direction dir;
    int score;
    bool gameOver;

    // ---------- Console Helpers ----------
    void SetColor(int color) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }

    void SetCursorPosition(int x, int y) {
        COORD coord;
        coord.X = (SHORT)x;
        coord.Y = (SHORT)y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    void HideCursor() {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    }

    // ---------- Game Logic ----------
    void GenerateFruit() {
        bool valid;
        do {
            valid = true;
            fruit.x = rand() % WIDTH;
            fruit.y = rand() % HEIGHT;
            for (auto &s : snake)
                if (s == fruit) valid = false;
        } while (!valid);

        currentFruit = fruitSymbols[rand() % fruitSymbols.size()];
    }

    bool CheckCollision(Point p) {
        if (p.x < 0 || p.x >= WIDTH || p.y < 0 || p.y >= HEIGHT)
            return true;

        for (size_t i = 1; i < snake.size(); i++)
            if (snake[i] == p)
                return true;

        return false;
    }

public:
    SnakeGame() {
        SetConsoleOutputCP(65001); // Enable emoji support
        srand(time(0));
        Point start = {WIDTH / 2, HEIGHT / 2};
        snake.push_back(start);
        dir = STOP;
        score = 0;
        gameOver = false;
        GenerateFruit();
        HideCursor();
    }

    void ShowStartScreen() {
        system("cls");
        SetColor(10);
        cout << "\n\n";
        cout << "        ╔════════════════════════════╗\n";
        cout << "        ║      🐍 SNAKE GAME 🐍       ║\n";
        cout << "        ╚════════════════════════════╝\n\n";

        SetColor(14);
        cout << "Controls:\n";
        SetColor(7);
        cout << "  W - Move Up\n";
        cout << "  S - Move Down\n";
        cout << "  A - Move Left\n";
        cout << "  D - Move Right\n";
        cout << "  X - Quit Game\n\n";
        SetColor(13);
        cout << "Eat the fruits ";
        SetColor(12);
        cout << "🍎 🍉 🍓";
        SetColor(13);
        cout << " to grow and score!\n\n";
        SetColor(11);
        cout << "Press any key to start...\n";
        SetColor(7);
        _getch();
        system("cls");
    }

    void Input() {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
                case 'w': case 'W': if (dir != DOWN) dir = UP; break;
                case 's': case 'S': if (dir != UP) dir = DOWN; break;
                case 'a': case 'A': if (dir != RIGHT) dir = LEFT; break;
                case 'd': case 'D': if (dir != LEFT) dir = RIGHT; break;
                case 'x': case 'X': gameOver = true; break;
            }
        }
    }

    void Logic() {
        if (dir == STOP) return;

        Point newHead = snake[0];

        switch (dir) {
            case LEFT:  newHead.x--; break;
            case RIGHT: newHead.x++; break;
            case UP:    newHead.y--; break;
            case DOWN:  newHead.y++; break;
        }

        if (CheckCollision(newHead)) {
            gameOver = true;
            return;
        }

        snake.insert(snake.begin(), newHead);

        if (newHead == fruit) {
            score += 10;
            GenerateFruit();
        } else {
            snake.pop_back();
        }
    }

    void Draw() {
        SetCursorPosition(0, 0);

        // Top Border
        SetColor(14);
        cout << "╔";
        for (int i = 0; i < WIDTH; i++) cout << "══";
        cout << "╗\n";

        // Game Grid
        for (int y = 0; y < HEIGHT; y++) {
            SetColor(14);
            cout << "║";

            for (int x = 0; x < WIDTH; x++) {
                Point current = {x, y};
                bool isHead = (snake[0] == current);
                bool isBody = false;

                for (size_t i = 1; i < snake.size(); i++) {
                    if (snake[i] == current) {
                        isBody = true;
                        break;
                    }
                }

                if (isHead) {
                    SetColor(10);
                    cout << "🟩";
                } else if (isBody) {
                    SetColor(2);
                    cout << "🟢";
                } else if (fruit == current) {
                    SetColor(12);
                    cout << currentFruit;
                } else {
                    SetColor(7);
                    cout << "  ";
                }
            }

            SetColor(14);
            cout << "║\n";
        }

        // Bottom Border
        SetColor(14);
        cout << "╚";
        for (int i = 0; i < WIDTH; i++) cout << "══";
        cout << "╝\n";

        // Info bar
        SetColor(11);
        cout << "Score: " << score << "   ";
        SetColor(13);
        cout << "Length: " << snake.size() << "\n";
        SetColor(7);
        cout << "Controls: W/A/S/D to move | X to quit\n";
    }

    bool IsGameOver() { return gameOver; }

    void GameOverScreen() {
        system("cls");
        SetColor(12);
        cout << "\n\n";
        cout << "        ╔════════════════════════════╗\n";
        cout << "        ║        💀 GAME OVER 💀      ║\n";
        cout << "        ╚════════════════════════════╝\n\n";
        SetColor(11);
        cout << "         Final Score: " << score << endl;
        cout << "         Snake Length: " << snake.size() << endl;
        SetColor(7);
        cout << "\nPress any key to exit...\n";
        _getch();
    }
};

int main() {
    SnakeGame game;
    game.ShowStartScreen();

    while (!game.IsGameOver()) {
        game.Input();
        game.Logic();
        game.Draw();
        Sleep(150);
    }

    game.GameOverScreen();
    return 0;
}
