#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

int WIDTH = 30;
int HEIGHT = 15;

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
    Point powerFruit;
    Point bomb;
    bool powerFruitActive;
    bool bombActive;
    time_t powerFruitStart;
    time_t bombStart;

    vector<Point> obstacles;      // ✅ Hard mode obstacles
    bool useObstacles = false;    // ✅ Enabled only in HARD

    string playerName;
    string currentFruit;
    vector<string> fruitSymbols = {"🍎", "🍒", "🍉", "🥭", "🍓"};

    Direction dir;
    int score;
    bool gameOver;
    bool paused;
    bool retry;

    int baseSpeed;
    int level;

    struct HighScoreEntry {
        string name;
        int score;
    } highScore;

    // ---------- Console Utilities ----------
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

    // ---------- High Score ----------
    void LoadHighScore() {
        ifstream file("highscore.txt");
        if (file.is_open()) {
            file >> highScore.name >> highScore.score;
            file.close();
        } else {
            highScore.name = "None";
            highScore.score = 0;
        }
    }

    void SaveHighScore() {
        ofstream file("highscore.txt");
        if (file.is_open()) {
            file << playerName << " " << score;
            file.close();
        }
    }

    // ---------- Obstacles (Hard mode only) ----------
    void GenerateObstacles() {
        if (!useObstacles) return;

        obstacles.clear();
        int count = level + 3; // More obstacles each level

        for (int i = 0; i < count; i++) {
            Point o;
            bool valid;
            do {
                valid = true;
                o.x = rand() % WIDTH;
                o.y = rand() % HEIGHT;

                for (auto &s : snake)
                    if (s == o) valid = false;
                if (fruit == o) valid = false;
                if (powerFruitActive && powerFruit == o) valid = false;
                if (bombActive && bomb == o) valid = false;

            } while (!valid);

            obstacles.push_back(o);
        }
    }

    // ---------- Fruit ----------
    void GenerateFruit() {
        bool valid;
        do {
            valid = true;
            fruit.x = rand() % WIDTH;
            fruit.y = rand() % HEIGHT;

            for (auto &s : snake)
                if (s == fruit) valid = false;

            if (powerFruitActive && fruit == powerFruit) valid = false;
            if (bombActive && fruit == bomb) valid = false;

            if (useObstacles)
                for (auto &o : obstacles)
                    if (o == fruit) valid = false;

        } while (!valid);

        currentFruit = fruitSymbols[rand() % fruitSymbols.size()];
    }

    void GeneratePowerFruit() {
        bool valid;
        do {
            valid = true;
            powerFruit.x = rand() % WIDTH;
            powerFruit.y = rand() % HEIGHT;

            for (auto &s : snake)
                if (s == powerFruit) valid = false;

            if (fruit == powerFruit) valid = false;
            if (bombActive && bomb == powerFruit) valid = false;

            if (useObstacles)
                for (auto &o : obstacles)
                    if (o == powerFruit) valid = false;

        } while (!valid);

        powerFruitActive = true;
        powerFruitStart = time(0);
        Beep(1700, 150);
    }

    // ---------- Bomb ----------
    void GenerateBomb() {
        bool valid;
        do {
            valid = true;
            bomb.x = rand() % WIDTH;
            bomb.y = rand() % HEIGHT;

            for (auto &s : snake)
                if (s == bomb) valid = false;
            if (fruit == bomb) valid = false;
            if (powerFruitActive && bomb == powerFruit) valid = false;

            if (useObstacles)
                for (auto &o : obstacles)
                    if (o == bomb) valid = false;

        } while (!valid);

        bombActive = true;
        bombStart = time(0);
    }

    // ---------- Collision ----------
    bool CheckCollision(Point p) {
        if (p.x < 0 || p.x >= WIDTH || p.y < 0 || p.y >= HEIGHT)
            return true;

        for (size_t i = 1; i < snake.size(); i++)
            if (snake[i] == p)
                return true;

        if (useObstacles)
            for (auto &o : obstacles)
                if (o == p) return true;

        return false;
    }

    // ---------- Level Progress ----------
    void LevelSystem() {
        int newLevel = score / 30 + 1;
        if (newLevel > level) {
            level = newLevel;
            Beep(1200, 120);
            Beep(1500, 120);

            if (useObstacles)
                GenerateObstacles();
        }
    }

public:
    SnakeGame() {
        SetConsoleOutputCP(65001);
        srand(time(0));

        cout << "Enter your name: ";
        cin >> playerName;

        cout << "\nSelect Difficulty:\n";
        cout << "1. Easy\n2. Medium\n3. Hard (with Obstacles)\nChoice: ";
        int d;
        cin >> d;

        if (d == 1) baseSpeed = 150;
        else if (d == 2) baseSpeed = 110;
        else {
            baseSpeed = 80;
            useObstacles = true; // ✅ Only HARD gets obstacles
        }

        snake.clear();
        snake.push_back({WIDTH / 2, HEIGHT / 2});

        dir = STOP;
        score = 0;
        gameOver = false;
        paused = false;
        retry = false;
        level = 1;
        bombActive = false;
        powerFruitActive = false;

        LoadHighScore();
        GenerateFruit();
        GenerateBomb();
        GenerateObstacles(); // ✅ Only active in Hard
        HideCursor();
    }

    // ---------- Start Screen ----------
    void ShowStartScreen() {
        system("cls");
        SetColor(10);
        cout << "\n\n        🐍 WELCOME TO ADVANCED SNAKE GAME 🐍\n\n";
        SetColor(14);
        cout << "Controls:\n";
        SetColor(7);
        cout << " W/A/S/D - Move\n";
        cout << " P - Pause/Resume\n";
        cout << " X - Quit Game\n\n";
        SetColor(11);
        cout << "High Score: " << highScore.name << " - " << highScore.score << "\n\n";
        SetColor(13);
        cout << "Press any key to start...";
        _getch();
        system("cls");
    }

    // ---------- Input ----------
    void Input() {
        if (_kbhit()) {
            char key = _getch();
            if (!paused) {
                if (key == 'w' || key == 'W') dir = UP;
                if (key == 's' || key == 'S') dir = DOWN;
                if (key == 'a' || key == 'A') dir = LEFT;
                if (key == 'd' || key == 'D') dir = RIGHT;
            }
            if (key == 'p' || key == 'P') paused = !paused;
            if (key == 'x' || key == 'X') gameOver = true;
        }
    }

    // ---------- Logic ----------
    void Logic() {
        if (paused || dir == STOP) return;

        Point newHead = snake[0];
        switch (dir) {
            case LEFT: newHead.x--; break;
            case RIGHT: newHead.x++; break;
            case UP: newHead.y--; break;
            case DOWN: newHead.y++; break;
        }

        if (CheckCollision(newHead)) {
            Beep(600, 200);
            Beep(400, 200);
            Beep(250, 300);
            gameOver = true;
            return;
        }

        snake.insert(snake.begin(), newHead);

        if (newHead == fruit) {
            score += 10;
            Beep(1000, 80);
            Beep(1200, 80);
            GenerateFruit();
        }
        else if (powerFruitActive && newHead == powerFruit) {
            score += 50;
            powerFruitActive = false;
            Beep(1600, 200);
            Beep(1900, 200);
        }
        else {
            snake.pop_back();
        }

        if (!powerFruitActive && rand() % 70 == 0)
            GeneratePowerFruit();

        if (powerFruitActive && difftime(time(0), powerFruitStart) >= 5)
            powerFruitActive = false;

        if (bombActive && newHead == bomb) {
            Beep(300, 200);
            Beep(200, 200);
            Beep(100, 300);
            gameOver = true;
            return;
        }

        if (bombActive && difftime(time(0), bombStart) >= 5) {
            bombActive = false;
            GenerateBomb();
        }

        LevelSystem();
    }

    // ---------- Draw ----------
    void Draw() {
        SetCursorPosition(0, 0);
        SetColor(14);
        cout << "╔";
        for (int i = 0; i < WIDTH; i++) cout << "══";
        cout << "╗\n";

        for (int y = 0; y < HEIGHT; y++) {
            SetColor(14);
            cout << "║";
            for (int x = 0; x < WIDTH; x++) {
                Point current = {x, y};

                bool isHead = (snake[0] == current);
                bool isBody = false;
                for (size_t i = 1; i < snake.size(); i++)
                    if (snake[i] == current)
                        isBody = true;

                bool isObs = false;
                if (useObstacles)
                    for (auto &o : obstacles)
                        if (o == current) isObs = true;

                if (isHead) { SetColor(10); cout << "🟩"; }
                else if (isBody) { SetColor(2); cout << "🟢"; }
                else if (isObs) { SetColor(5); cout << "▩ "; }
                else if (current == fruit) { SetColor(12); cout << currentFruit; }
                else if (powerFruitActive && current == powerFruit) { SetColor(11); cout << "⭐"; }
                else if (bombActive && current == bomb) { SetColor(4); cout << "💣"; }
                else { SetColor(7); cout << "  "; }
            }
            SetColor(14);
            cout << "║\n";
        }

        SetColor(14);
        cout << "╚";
        for (int i = 0; i < WIDTH; i++) cout << "══";
        cout << "╝\n";

        SetColor(11);
        cout << "Player: " << playerName
             << "   Score: " << score
             << "   Length: " << snake.size()
             << "   Level: " << level << "\n";
        cout << "High Score: " << highScore.name << " - " << highScore.score << "\n";
        cout << "Controls: W/A/S/D Move | P Pause | X Quit\n";
    }

    bool IsGameOver() { return gameOver; }
    bool ShouldRetry() { return retry; }

    // ---------- Game Over ----------
    void GameOverScreen() {
        system("cls");
        SetColor(12);
        cout << "\n\n       💀 GAME OVER 💀\n\n";
        SetColor(11);
        cout << "Final Score: " << score << endl;
        cout << "Snake Length: " << snake.size() << endl;

        if (score > highScore.score) {
            cout << "🎉 New High Score! 🎉\n";
            SaveHighScore();
            Beep(1000, 120);
            Beep(1300, 120);
            Beep(1600, 200);
        }

        SetColor(13);
        cout << "\nPress R to Retry or any other key to Exit...\n";
        char choice = _getch();
        retry = (choice == 'r' || choice == 'R');
    }

    int GetSpeedDelay() {
        int speed = baseSpeed - (snake.size() * 2) - (level * 5);
        return max(50, speed);
    }
};

// ---------- Main ----------
int main() {
    while (true) {
        SnakeGame game;
        game.ShowStartScreen();

        while (!game.IsGameOver()) {
            game.Input();
            game.Logic();
            game.Draw();
            Sleep(game.GetSpeedDelay());
        }

        game.GameOverScreen();
        if (!game.ShouldRetry()) break;
    }

    return 0;
}
