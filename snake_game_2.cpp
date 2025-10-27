#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

const int WIDTH = 40;
const int HEIGHT = 20;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

struct Point {
    int x, y;
};

bool operator==(const Point& p1, const Point& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

class SnakeGame {
private:
    vector<Point> snake;
    Point fruit;
    Direction dir;
    int score;
    bool gameOver;

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

    void GenerateFruit() {
        bool validPosition;
        do {
            validPosition = true;
            fruit.x = rand() % WIDTH;
            fruit.y = rand() % HEIGHT;
            
            for (size_t i = 0; i < snake.size(); i++) {
                if (snake[i] == fruit) {
                    validPosition = false;
                    break;
                }
            }
        } while (!validPosition);
    }

    bool CheckCollision(Point p) {
        if (p.x < 0 || p.x >= WIDTH || p.y < 0 || p.y >= HEIGHT)
            return true;
        
        for (size_t i = 1; i < snake.size(); i++) {
            if (snake[i] == p)
                return true;
        }
        return false;
    }

public:
    SnakeGame() {
        srand(time(0));
        Point start;
        start.x = WIDTH / 2;
        start.y = HEIGHT / 2;
        snake.push_back(start);
        dir = STOP;
        score = 0;
        gameOver = false;
        GenerateFruit();
        HideCursor();
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
            case LEFT: newHead.x--; break;
            case RIGHT: newHead.x++; break;
            case UP: newHead.y--; break;
            case DOWN: newHead.y++; break;
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
        
        // Top border
        SetColor(14);
        cout << "+";
        for (int i = 0; i < WIDTH; i++) cout << "-";
        cout << "+" << endl;

        // Game area
        for (int y = 0; y < HEIGHT; y++) {
            SetColor(14);
            cout << "|";
            
            for (int x = 0; x < WIDTH; x++) {
                Point current;
                current.x = x;
                current.y = y;
                
                bool isSnakePart = false;
                bool isHead = (snake[0] == current);
                
                if (!isHead) {
                    for (size_t i = 1; i < snake.size(); i++) {
                        if (snake[i] == current) {
                            isSnakePart = true;
                            break;
                        }
                    }
                }
                
                if (isHead) {
                    SetColor(10);
                    cout << "O";
                } else if (isSnakePart) {
                    SetColor(2);
                    cout << "o";
                } else if (fruit == current) {
                    SetColor(12);
                    cout << "@";
                } else {
                    SetColor(7);
                    cout << " ";
                }
            }
            
            SetColor(14);
            cout << "|" << endl;
        }

        // Bottom border
        SetColor(14);
        cout << "+";
        for (int i = 0; i < WIDTH; i++) cout << "-";
        cout << "+" << endl;

        // Score and controls
        SetColor(11);
        cout << "Score: " << score << "  |  ";
        SetColor(13);
        cout << "Length: " << snake.size() << endl;
        SetColor(7);
        cout << "Controls: W/A/S/D to move  |  X to quit" << endl;
    }

    bool IsGameOver() { return gameOver; }

    void GameOverScreen() {
        system("cls");
        SetColor(12);
        cout << "\n\n\n";
        cout << "        +================================+\n";
        cout << "        |        GAME OVER!              |\n";
        cout << "        +================================+\n\n";
        SetColor(11);
        cout << "              Final Score: " << score << endl;
        cout << "              Snake Length: " << snake.size() << endl;
        SetColor(7);
        cout << "\n\n        Press any key to exit...\n";
        _getch();
    }

    void ShowStartScreen() {
        system("cls");
        SetColor(10);
        cout << "\n\n";
        cout << "        +================================+\n";
        cout << "        |      SNAKE GAME - C++          |\n";
        cout << "        +================================+\n\n";
        SetColor(14);
        cout << "        Controls:\n";
        SetColor(7);
        cout << "        W - Move Up\n";
        cout << "        S - Move Down\n";
        cout << "        A - Move Left\n";
        cout << "        D - Move Right\n";
        cout << "        X - Quit Game\n\n";
        SetColor(13);
        cout << "        Eat the ";
        SetColor(12);
        cout << "@";
        SetColor(13);
        cout << " to grow and score!\n\n";
        SetColor(11);
        cout << "        Press any key to start...\n";
        SetColor(7);
        _getch();
        system("cls");
    }
};

int main() {
    SnakeGame game;
    game.ShowStartScreen();

    while (!game.IsGameOver()) {
        game.Input();
        game.Logic();
        game.Draw();
        Sleep(100);
    }

    game.GameOverScreen();
    return 0;
}
