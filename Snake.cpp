// ===== Advanced Snake (Neon Theme — Final Stable Version) =====
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
#include <limits>
using namespace std;

int WIDTH = 30, HEIGHT = 15;

enum Direction { STOP=0, LEFT, RIGHT, UP, DOWN };
struct Point { int x,y; };
bool operator==(const Point&a,const Point&b){ return a.x==b.x && a.y==b.y; }

class SnakeGame {
private:
    vector<Point> snake;
    Point fruit, powerFruit, bomb;
    bool powerFruitActive=false, bombActive=false;
    time_t powerFruitStart=0, bombStart=0;

    vector<Point> obstacles;
    bool useObstacles=false;

    string playerName;
    string currentFruit;
    vector<string> fruitSymbols = {"🍎","🍒","🍉","🥭","🍓"};

    Direction dir=STOP;
    int score=0;
    bool gameOver=false, paused=false, retry=false;
    int baseSpeed=120, level=1;

    string lastPointsMsg="";
    time_t lastPointsTime=0;

    struct HighScoreEntry { string name; int score; } highScore;

    // === Console Utilities ===
    void SetColor(int c){ SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c); }
    void SetCursorPosition(int x,int y){ COORD cpos{(SHORT)x,(SHORT)y}; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cpos); }
    void HideCursor(){
        CONSOLE_CURSOR_INFO ci; GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
        ci.bVisible = FALSE; SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
    }

    // === High Score ===
    void LoadHighScore(){
        ifstream f("highscore.txt");
        if (f.is_open()) { f >> highScore.name >> highScore.score; f.close(); }
        else { highScore.name="None"; highScore.score=0; }
    }
    void SaveHighScore(){
        ofstream f("highscore.txt");
        if (f.is_open()) { f << playerName << " " << score; f.close(); }
    }

    // === Object Generators ===
    void GenerateObstacles(){
        if (!useObstacles) return;
        obstacles.clear();
        int count = level + 3;
        for(int i=0;i<count;i++){
            Point o; bool ok;
            do{
                ok=true;
                o.x = rand()%WIDTH; o.y=rand()%HEIGHT;
                for (auto&s:snake) if (s==o) ok=false;
                if (fruit==o) ok=false;
                if (powerFruitActive && powerFruit==o) ok=false;
                if (bombActive && bomb==o) ok=false;
            }while(!ok);
            obstacles.push_back(o);
        }
    }
    void GenerateFruit(){
        bool ok;
        do{
            ok=true;
            fruit.x=rand()%WIDTH; fruit.y=rand()%HEIGHT;
            for (auto&s:snake) if (s==fruit) ok=false;
            if (powerFruitActive && fruit==powerFruit) ok=false;
            if (bombActive && fruit==bomb) ok=false;
            if (useObstacles) for (auto&o:obstacles) if (o==fruit) ok=false;
        }while(!ok);
        currentFruit = fruitSymbols[rand()%fruitSymbols.size()];
    }
    void GeneratePowerFruit(){
        bool ok;
        do{
            ok=true;
            powerFruit.x=rand()%WIDTH; powerFruit.y=rand()%HEIGHT;
            for (auto&s:snake) if (s==powerFruit) ok=false;
            if (fruit==powerFruit) ok=false;
            if (bombActive && bomb==powerFruit) ok=false;
            if (useObstacles) for (auto&o:obstacles) if (o==powerFruit) ok=false;
        }while(!ok);
        powerFruitActive=true; powerFruitStart=time(0);
    }
    void GenerateBomb(){
        bool ok;
        do{
            ok=true;
            bomb.x=rand()%WIDTH; bomb.y=rand()%HEIGHT;
            for (auto&s:snake) if (s==bomb) ok=false;
            if (fruit==bomb) ok=false;
            if (powerFruitActive && bomb==powerFruit) ok=false;
            if (useObstacles) for (auto&o:obstacles) if (o==bomb) ok=false;
        }while(!ok);
        bombActive=true; bombStart=time(0);
    }

    // === Mechanics ===
    bool CheckCollision(Point p){
        if (p.x<0 || p.x>=WIDTH || p.y<0 || p.y>=HEIGHT) return true;
        for (size_t i=1;i<snake.size();++i) if (snake[i]==p) return true;
        if (useObstacles) for (auto&o:obstacles) if (o==p) return true;
        return false;
    }
    void LevelSystem(){
        int newLvl = score/30 + 1;
        if (newLvl>level){ level=newLvl; if (useObstacles) GenerateObstacles(); }
    }
    static bool IsOpposite(Direction a, Direction b){
        return (a==LEFT&&b==RIGHT)||(a==RIGHT&&b==LEFT)||(a==UP&&b==DOWN)||(a==DOWN&&b==UP);
    }

    // === UI Sections ===
    void ShowRules(){
        system("cls");
        SetColor(13); cout << "\n\n      📘 ADVANCED SNAKE — RULEBOOK\n\n";
        SetColor(7);
        cout << "───────────────────────────────────────────\n";
        cout << "🎯 Eat fruits to grow, avoid bombs & walls.\n";
        cout << "⭐ Power Fruits: +50 pts  |  🍎 Normal: +10\n";
        cout << "💣 Bomb = Instant Game Over\n";
        cout << "🕹 Arrows to Move, P to Pause, X to Quit\n";
        cout << "───────────────────────────────────────────\n\n";
        SetColor(10); cout << "Press any key to continue..."; _getch();
        system("cls");
    }

    void Header(){
        SetColor(11);
        cout << "Player: " << playerName
             << "  |  Score: " << score
             << "  |  Length: " << snake.size()
             << "  |  Level: "  << level
             << "  |  High: "   << highScore.score << "\n";
    }

    void DrawBars(){
        double pLeft = powerFruitActive ? 5 - difftime(time(0), powerFruitStart) : 0;
        double bLeft = bombActive  ? 5 - difftime(time(0), bombStart) : 0;
        int W = 24;

        if (powerFruitActive){
            SetColor(11); cout << "⭐ Power: [";
            int fill = max(0,(int)((pLeft/5.0)*W));
            for (int i=0;i<W;i++) cout << (i<fill? "█":" ");
            cout << "] " << fixed << setprecision(1) << max(0.0,pLeft) << "s\n";
        }
        if (bombActive){
            SetColor(12); cout << "💣 Bomb : [";
            int fill = max(0,(int)((bLeft/5.0)*W));
            for (int i=0;i<W;i++) cout << (i<fill? "█":" ");
            cout << "] " << fixed << setprecision(1) << max(0.0,bLeft) << "s\n";
        }
    }

    void NeonBanner(){
        SetColor(13); cout << "╔"; for(int i=0;i<WIDTH;i++) cout<<"══"; cout<<"╗\n";
    }
    void NeonFooter(){
        SetColor(13); cout << "╚"; for(int i=0;i<WIDTH;i++) cout<<"══"; cout<<"╝\n";
    }

public:
    SnakeGame(){
        SetConsoleOutputCP(65001);
        srand((unsigned)time(0));
        HideCursor();

        cout << "Enter your name: ";
        cin.clear();
        if (cin.peek() == '\n') cin.ignore();
        getline(cin, playerName);

        char ch;
        while(true){
            SetColor(9);
            cout << "\nHello " << playerName << ", do you want to read the rules? (Y/N): ";
            SetColor(7);
            cin >> ch;
            if (ch=='Y'||ch=='y'){ ShowRules(); break; }
            else if (ch=='N'||ch=='n'){ system("cls"); break; }
            else { SetColor(12); cout << "Invalid! Try again.\n"; SetColor(7); }
        }

        int d;
        while(true){
            cout << "\nSelect Difficulty:\n";
            cout << " 1) Easy   2) Medium   3) Hard (with Obstacles)\nChoice: ";
            cin >> d;
            if (cin.fail()){ cin.clear(); cin.ignore(1000,'\n'); continue; }
            if (d==1){ baseSpeed=150; break; }
            if (d==2){ baseSpeed=110; break; }
            if (d==3){ baseSpeed=80;  useObstacles=true; break; }
        }

        snake.clear(); snake.push_back({WIDTH/2, HEIGHT/2});
        dir=STOP; score=0; level=1; bombActive=false; powerFruitActive=false;
        lastPointsMsg=""; lastPointsTime=0;

        LoadHighScore();
        GenerateFruit(); GenerateBomb(); GenerateObstacles();
        system("cls");
    }

    void ShowStartScreen(){
        system("cls");
        SetColor(11);
        cout << "\n   Arrow Keys = Move  |  P Pause  |  X Quit\n\n";
        cout << "   High Score: " << highScore.name << " - " << highScore.score << "\n\n";
        SetColor(10); cout << "   Press any key to start..."; _getch();
        system("cls");
    }

    void Input(){
        if (_kbhit()){
            int k=_getch();
            if (!paused && (k==224 || k==0)){
                int a=_getch(); Direction next=dir;
                if (a==72) next=UP; if (a==80) next=DOWN; if (a==75) next=LEFT; if (a==77) next=RIGHT;
                if (!(snake.size()>1 && IsOpposite(dir,next))) dir=next;
            }
            if (k=='p'||k=='P') paused=!paused;
            if (k=='x'||k=='X') gameOver=true;
        }
    }

    void Logic(){
        if (paused || dir==STOP) return;
        Point nh=snake[0];
        if (dir==LEFT) nh.x--; else if (dir==RIGHT) nh.x++; else if (dir==UP) nh.y--; else if (dir==DOWN) nh.y++;

        if (CheckCollision(nh)){ gameOver=true; return; }
        snake.insert(snake.begin(), nh);

        if (nh==fruit){
            score+=10; lastPointsMsg="+10"; lastPointsTime=time(0);
            GenerateFruit();
        } else if (powerFruitActive && nh==powerFruit){
            score+=50; lastPointsMsg="+50"; lastPointsTime=time(0);
            powerFruitActive=false;
        } else {
            snake.pop_back();
        }

        if (!powerFruitActive && rand()%70==0) GeneratePowerFruit();
        if (powerFruitActive && difftime(time(0),powerFruitStart)>=5) powerFruitActive=false;

        if (bombActive && nh==bomb){ gameOver=true; return; }
        if (bombActive && difftime(time(0),bombStart)>=5){ bombActive=false; GenerateBomb(); }

        LevelSystem();
    }

    void Draw(){
        SetCursorPosition(0,0); // overwrite previous frame, prevents stacking
        Header();

        // Popup score above board
        if (!lastPointsMsg.empty() && difftime(time(0), lastPointsTime) < 2) {
            SetColor(10);
            cout << "\n   " << lastPointsMsg << "!\n";
            SetColor(7);
        } else cout << "\n\n";

        NeonBanner();
        for (int y=0;y<HEIGHT;y++){
            SetColor(13); cout<<"║";
            for (int x=0;x<WIDTH;x++){
                Point c{x,y};
                bool isHead = (snake[0]==c);
                bool isBody=false; for(size_t i=1;i<snake.size();++i) if(snake[i]==c) isBody=true;
                bool isObs=false; if(useObstacles) for(auto&o:obstacles) if(o==c) isObs=true;

                if (isHead){ SetColor(10); cout<<"🟩"; }
                else if (isBody){ SetColor(2); cout<<"🟢"; }
                else if (isObs){ SetColor(5); cout<<"▩ "; }
                else if (c==fruit){ SetColor(12); cout<<currentFruit; }
                else if (powerFruitActive && c==powerFruit){ SetColor(11); cout<<"⭐"; }
                else if (bombActive && c==bomb){ SetColor(12); cout<<"💣"; }
                else { SetColor(7); cout<<"  "; }
            }
            SetColor(13); cout<<"║\n";
        }
        NeonFooter();

        // Move the cursor below the board (fixed to prevent repetition)
        cout << "\033[K"; 
        DrawBars();
        SetColor(8); cout << "Controls: Arrow Keys | P Pause | X Quit\n";
        cout << "\033[K"; // clear line
        if (difftime(time(0),lastPointsTime)>=2) lastPointsMsg="";
    }

    bool IsGameOver(){ return gameOver; }
    bool ShouldRetry(){ return retry; }

    void GameOverScreen(){
        system("cls");
        SetColor(12); cout << "\n\n       💀 GAME OVER 💀\n\n";
        SetColor(11); cout << "Final Score: " << score << "\nSnake Length: " << snake.size() << "\n";
        if (score>highScore.score){ SaveHighScore(); SetColor(10); cout << "\n🎉 New High Score by " << playerName << " ("<<score<<")\n"; }
        SetColor(13); cout << "\nPress R to Retry or any other key to Exit...\n";
        char c=_getch(); retry=(c=='r'||c=='R');
    }

    int GetSpeedDelay(){
        int sp = baseSpeed - (int)snake.size()*2 - level*5;
        return max(50, sp);
    }
};

int main(){
    while (true){
        SnakeGame game;
        game.ShowStartScreen();
        while (!game.IsGameOver()){
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
