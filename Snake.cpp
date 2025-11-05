#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

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

    vector<Point> obstacles;
    bool useObstacles = false;

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

    // --- scoring feedback ---
    string lastPointsMsg;
    Point pointsMsgPos;
    time_t lastPointsTime;

    struct HighScoreEntry {
        string name;
        int score;
    } highScore;

    // ----- console utils -----
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

    // ----- high score -----
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

    // ----- obstacles -----
    void GenerateObstacles() {
        if (!useObstacles) return;
        obstacles.clear();
        int count = level + 3;
        for (int i = 0; i < count; i++) {
            Point o;
            bool valid;
            do {
                valid = true;
                o.x = rand() % WIDTH;
                o.y = rand() % HEIGHT;
                for (auto &s : snake) if (s == o) valid = false;
                if (fruit == o) valid = false;
                if (powerFruitActive && powerFruit == o) valid = false;
                if (bombActive && bomb == o) valid = false;
            } while (!valid);
            obstacles.push_back(o);
        }
    }

    // ----- fruit / power fruit / bomb -----
    void GenerateFruit() {
        bool valid;
        do {
            valid = true;
            fruit.x = rand() % WIDTH;
            fruit.y = rand() % HEIGHT;
            for (auto &s : snake) if (s == fruit) valid = false;
            if (powerFruitActive && fruit == powerFruit) valid = false;
            if (bombActive && fruit == bomb) valid = false;
            if (useObstacles) for (auto &o : obstacles) if (o == fruit) valid = false;
        } while (!valid);
        currentFruit = fruitSymbols[rand() % fruitSymbols.size()];
    }

    void GeneratePowerFruit() {
        bool valid;
        do {
            valid = true;
            powerFruit.x = rand() % WIDTH;
            powerFruit.y = rand() % HEIGHT;
            for (auto &s : snake) if (s == powerFruit) valid = false;
            if (fruit == powerFruit) valid = false;
            if (bombActive && bomb == powerFruit) valid = false;
            if (useObstacles) for (auto &o : obstacles) if (o == powerFruit) valid = false;
        } while (!valid);
        powerFruitActive = true;
        powerFruitStart = time(0);
    }

    void GenerateBomb() {
        bool valid;
        do {
            valid = true;
            bomb.x = rand() % WIDTH;
            bomb.y = rand() % HEIGHT;
            for (auto &s : snake) if (s == bomb) valid = false;
            if (fruit == bomb) valid = false;
            if (powerFruitActive && bomb == powerFruit) valid = false;
            if (useObstacles) for (auto &o : obstacles) if (o == bomb) valid = false;
        } while (!valid);
        bombActive = true;
        bombStart = time(0);
    }

    // ----- collisions / level -----
    bool CheckCollision(Point p) {
        if (p.x < 0 || p.x >= WIDTH || p.y < 0 || p.y >= HEIGHT) return true;
        for (size_t i = 1; i < snake.size(); i++) if (snake[i] == p) return true;
        if (useObstacles) for (auto &o : obstacles) if (o == p) return true;
        return false;
    }

    void LevelSystem() {
        int newLevel = score / 30 + 1;
        if (newLevel > level) {
            level = newLevel;
            if (useObstacles) GenerateObstacles();
        }
    }

    static bool IsOpposite(Direction a, Direction b) {
        return (a == LEFT && b == RIGHT) || (a == RIGHT && b == LEFT) ||
               (a == UP && b == DOWN) || (a == DOWN && b == UP);
    }

    // 📘 RULEBOOK SCREEN (kept private)
    void ShowRules() {
        system("cls");
        SetColor(11);
        cout << "\n\n📘  GAME RULEBOOK - ADVANCED SNAKE 🐍\n";
        SetColor(7);
        cout << "────────────────────────────────────────────\n\n";
        SetColor(14); cout << "🎯 OBJECTIVE:\n"; SetColor(7);
        cout << "Grow your snake by eating fruits and survive as long as possible!\n\n";

        SetColor(14); cout << "🍎 FRUITS:\n"; SetColor(7);
        cout << "  - Regular fruits give +10 points.\n";
        cout << "  - Power fruits ⭐ appear randomly and give +50 points.\n\n";

        SetColor(14); cout << "💣 BOMBS:\n"; SetColor(7);
        cout << "  - Avoid them! Touching a bomb ends the game immediately.\n\n";

        SetColor(14); cout << "🧱 OBSTACLES (Hard Mode only):\n"; SetColor(7);
        cout << "  - Static blocks appear as you level up.\n";
        cout << "  - Colliding with one causes instant death.\n\n";

        SetColor(14); cout << "🕹️ CONTROLS:\n"; SetColor(7);
        cout << "  → Arrow Keys  - Move the snake\n";
        cout << "  → P           - Pause/Resume\n";
        cout << "  → X           - Quit the game\n\n";

        SetColor(14); cout << "💡 TIPS:\n"; SetColor(7);
        cout << "  - The snake moves faster as it grows and levels up.\n";
        cout << "  - You can't reverse direction instantly (for safety).\n";
        cout << "  - High scores are saved automatically.\n\n";

        SetColor(10);
        cout << "Press any key to continue...";
        _getch();
        system("cls");
    }

public:
    SnakeGame() {
        SetConsoleOutputCP(65001);
        srand(time(0));

        cout << "Enter your name: ";
        cin >> playerName;

        // Blue Y/N prompt with validation
        char choice;
        while (true) {
            SetColor(9);
            cout << "\nHello " << playerName << ", before starting to play the game,\n";
            cout << "do you want to read the rules for this game? (Y/N): ";
            SetColor(7);
            cin >> choice;

            if (choice == 'Y' || choice == 'y') { ShowRules(); break; }
            else if (choice == 'N' || choice == 'n') { system("cls"); break; }
            else {
                SetColor(12);
                cout << "\n⚠️  Invalid choice! Please enter Y or N.\n";
                SetColor(7);
            }
        }

        int d;
        while (true) {
            cout << "\nSelect Difficulty:\n";
            cout << "1. Easy\n2. Medium\n3. Hard (with Obstacles)\nChoice: ";
            cin >> d;

            if (cin.fail()) {
                cin.clear(); cin.ignore(1000, '\n');
                SetColor(12); cout << "\n⚠️  Invalid input! Please enter 1, 2, or 3.\n\n"; SetColor(7);
                continue;
            }

            if (d == 1) { baseSpeed = 150; break; }
            else if (d == 2) { baseSpeed = 110; break; }
            else if (d == 3) { baseSpeed = 80; useObstacles = true; break; }
            else {
                SetColor(12); cout << "\n⚠️  Invalid choice! Please enter 1, 2, or 3.\n\n"; SetColor(7);
            }
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
        lastPointsMsg = "";
        lastPointsTime = 0;
        pointsMsgPos = {-1, -1};

        LoadHighScore();
        GenerateFruit();
        GenerateBomb();
        GenerateObstacles();
        HideCursor();
    }

    void ShowStartScreen() {
        system("cls");
        SetColor(10);
        cout << "\n\n        🐍 WELCOME TO ADVANCED SNAKE GAME 🐍\n\n";
        SetColor(14); cout << "Controls:\n"; SetColor(7);
        cout << " Arrow Keys - Move\n";
        cout << " P - Pause/Resume\n";
        cout << " X - Quit Game\n\n";
        SetColor(11);
        cout << "High Score: " << highScore.name << " - " << highScore.score << "\n\n";
        SetColor(13);
        cout << "Press any key to start...";
        _getch();
        system("cls");
    }

    void Input() {
        if (_kbhit()) {
            int k = _getch();
            if (!paused && (k == 224 || k == 0)) {
                int arrow = _getch();
                Direction next = dir;
                switch (arrow) {
                    case 72: next = UP; break;
                    case 80: next = DOWN; break;
                    case 75: next = LEFT; break;
                    case 77: next = RIGHT; break;
                }
                if (!(snake.size() > 1 && IsOpposite(dir, next))) dir = next;
            }
            if (k == 'p' || k == 'P') paused = !paused;
            if (k == 'x' || k == 'X') gameOver = true;
        }
    }

    void Logic() {
        if (paused || dir == STOP) return;

        Point newHead = snake[0];
        switch (dir) {
            case LEFT:  newHead.x--; break;
            case RIGHT: newHead.x++; break;
            case UP:    newHead.y--; break;
            case DOWN:  newHead.y++; break;
        }

        if (CheckCollision(newHead)) { gameOver = true; return; }

        snake.insert(snake.begin(), newHead);

        if (newHead == fruit) {
            score += 10;
            lastPointsMsg = "+10"; lastPointsTime = time(0);
            pointsMsgPos = fruit; if (pointsMsgPos.x < WIDTH - 1) pointsMsgPos.x++;
            GenerateFruit();
        } else if (powerFruitActive && newHead == powerFruit) {
            score += 50;
            lastPointsMsg = "+50"; lastPointsTime = time(0);
            pointsMsgPos = powerFruit; if (pointsMsgPos.x < WIDTH - 1) pointsMsgPos.x++;
            powerFruitActive = false;
        } else {
            snake.pop_back();
        }

        if (!powerFruitActive && rand() % 70 == 0) GeneratePowerFruit();
        if (powerFruitActive && difftime(time(0), powerFruitStart) >= 5) powerFruitActive = false;

        if (bombActive && newHead == bomb) { gameOver = true; return; }
        if (bombActive && difftime(time(0), bombStart) >= 5) { bombActive = false; GenerateBomb(); }

        LevelSystem();
    }

    void Draw() {
        SetCursorPosition(0, 0);
        SetColor(11);
        cout << "Player: " << playerName
             << "  |  Score: " << score
             << "  |  Length: " << snake.size()
             << "  |  Level: " << level
             << "  |  High Score: " << highScore.score << "\n";

        SetColor(14);
        cout << "╔"; for (int i = 0; i < WIDTH; i++) cout << "══"; cout << "╗\n";

        for (int y = 0; y < HEIGHT; y++) {
            SetColor(14); cout << "║";
            for (int x = 0; x < WIDTH; x++) {
                Point current = {x, y};
                bool isHead = (snake[0] == current);
                bool isBody = false;
                for (size_t i = 1; i < snake.size(); i++) if (snake[i] == current) isBody = true;

                bool isObs = false;
                if (useObstacles) for (auto &o : obstacles) if (o == current) isObs = true;

                bool isMsg = (!lastPointsMsg.empty() && difftime(time(0), lastPointsTime) < 2 &&
                              current == pointsMsgPos);

                if (isHead) { SetColor(10); cout << "🟩"; }
                else if (isBody) { SetColor(2); cout << "🟢"; }
                else if (isObs) { SetColor(5); cout << "▩ "; }
                else if (current == fruit) { SetColor(12); cout << currentFruit; }
                else if (powerFruitActive && current == powerFruit) { SetColor(11); cout << "⭐"; }
                else if (bombActive && current == bomb) { SetColor(4); cout << "💣"; }
                else if (isMsg) { SetColor(9); cout << lastPointsMsg; }
                else { SetColor(7); cout << "  "; }
            }
            SetColor(14); cout << "║\n";
        }

        SetColor(14);
        cout << "╚"; for (int i = 0; i < WIDTH; i++) cout << "══"; cout << "╝\n";

        // Progress bars (⭐ blue, 💣 red)
        double powerLeft = powerFruitActive ? 5 - difftime(time(0), powerFruitStart) : 0;
        double bombLeft  = bombActive ? 5 - difftime(time(0), bombStart) : 0;
        int barWidth = 20;

        if (powerFruitActive) {
            SetColor(11);
            cout << "⭐ Power Fruit: [";
            int fill = max(0, (int)((powerLeft / 5.0) * barWidth));
            for (int i = 0; i < barWidth; i++) cout << (i < fill ? "█" : " ");
            cout << "] " << fixed << setprecision(1) << max(0.0, powerLeft) << "s\n";
        }
        if (bombActive) {
            SetColor(4);
            cout << "💣 Bomb Timer : [";
            int fill = max(0, (int)((bombLeft / 5.0) * barWidth));
            for (int i = 0; i < barWidth; i++) cout << (i < fill ? "█" : " ");
            cout << "] " << fixed << setprecision(1) << max(0.0, bombLeft) << "s\n";
        }

        SetColor(8);
        cout << "Controls: Arrow Keys | P Pause | X Quit\n";

        if (difftime(time(0), lastPointsTime) >= 2) lastPointsMsg = "";
    }

    bool IsGameOver() { return gameOver; }
    bool ShouldRetry() { return retry; }

    void GameOverScreen() {
        system("cls");
        SetColor(12);
        cout << "\n\n       💀 GAME OVER 💀\n\n";
        SetColor(11);
        cout << "Final Score: " << score << endl;
        cout << "Snake Length: " << snake.size() << endl;

        if (score > highScore.score) {
            SaveHighScore();
            SetColor(10);
            cout << "\n🎉 New High Score by " << playerName
                 << "! 🎉 (" << score << " points)\n";
        }

        SetColor(13);
        cout << "\nPress R to Retry or any other key to Exit...\n";
        char choice = _getch();
        retry = (choice == 'r' || choice == 'R');
    }

    int GetSpeedDelay() {
        int speed = baseSpeed - (int)snake.size() * 2 - level * 5;
        return max(50, speed);
    }
};

int main() {
    while (true) {
        SnakeGame game;           // constructor handles rule prompt
        game.ShowStartScreen();   // nice welcome before gameplay

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
