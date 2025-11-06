# 🐍 Snake Game – Advanced Console Edition (C++)

🎮 **An upgraded version of the classic Snake game** — built in **C++** for Windows Console with emojis, animations, levels, bombs, power fruits, and high scores!  
Enjoy a colorful, fun, and challenging gameplay experience — all within your terminal window 💻✨

---

## 🚀 Features

✅ **Classic Snake Mechanics** – Eat fruits and grow longer  
💥 **Bombs** – Avoid touching them or you’ll explode!  
🍒 **Power Fruits** – Time-limited fruits with bonus +50 points  
🏆 **Level System** – Game gets faster and harder as you score  
🧱 **Obstacles** – Appear in Hard Mode for extra challenge  
💾 **High Score Saving** – Automatically saves best score in a file  
⏸️ **Pause / Resume** – Press `P` anytime  
🔁 **Retry Option** – Press `R` to play again after Game Over  
🌈 **Colorful Console UI** – Beautifully drawn borders and emojis  
⚙️ **3 Difficulty Levels** – Easy, Medium, and Hard  

---

## 🖼️ Preview

```text
Player: Nigam   |  Score: 120   |  Level: 4   |  Length: 20   |  High: 200
═══════════════════════════════════════════════
🐍🐍🐍🍎🧱🧱🧱💣
...
═══════════════════════════════════════════════
Power Fruit: [#####]   Bomb: [###  ]
🧩 How to Play
Key	Action
⬆️ / ⬇️ / ⬅️ / ➡️	Move the snake
P	Pause / Resume
X	Exit game
R	Retry after game over
Enter	Confirm selection

🎯 Goal: Eat as many fruits as possible without hitting walls, bombs, or yourself!

⚙️ Installation & Run Instructions
🪟 For Windows:
Clone this repository

bash
Copy code
git clone https://github.com/your-username/snake-game-cpp.git
Open the project folder

bash
Copy code
cd snake-game-cpp
Compile the program

bash
Copy code
g++ snake.cpp -o snake.exe
Run the game

bash
Copy code
snake.exe
💡 Make sure your terminal supports UTF-8 emojis and color output for best visuals!

📁 Project Structure
bash
Copy code
📂 snake-game-cpp
 ┣ 📜 snake.cpp            # Main game source file
 ┣ 📜 highscore.txt        # Auto-created to save best score
 ┣ 📜 README.md            # You are here :)
🧠 Game Logic Overview
The game follows a simple loop:

css
Copy code
Input ➜ Logic ➜ Draw ➜ Sleep ➜ Repeat
Input() handles keyboard controls

Logic() updates movement, collisions, scoring, etc.

Draw() redraws the console with current state

Sleep() controls frame speed (snake movement rate)

Speed increases as your snake grows and level advances.

🎨 Difficulty Levels
Mode	Description	Speed	Obstacles
🟢 Easy	For beginners	Slow	❌
🟡 Medium	Balanced mode	Normal	❌
🔴 Hard	For pros	Fast	✅

💾 High Score System
The game saves your best score in highscore.txt

If you beat your record, it updates automatically 🎉

🧠 Concepts Used
Object-Oriented Programming (Classes & Structs)

File Handling (fstream)

Console Manipulation (windows.h)

Real-time Input (_kbhit(), _getch())

Vectors & Dynamic Data Structures

Random Number Generation

Time-based Events (Power fruit timer)

Color and Cursor Control for UI

📈 Future Enhancements (Ideas 💡)
🎵 Add background music or sound effects

🌐 Online leaderboard

🧍 Multiplayer mode

🪄 Custom skins and themes

🧩 Save game state and resume later

👨‍💻 Developed By
Nigam Sanghvi
💻 Computer Science Student | Game & Software Developer
📧 [your-email@example.com]
🌐 [your-github-profile-link]

🏁 Conclusion
This project is more than just a game — it’s a fun way to explore:

Real-time logic, OOP, and console-based UI design in C++ 🎯

If you like it, please ⭐ star this repo and share your feedback! 😊
Let’s make console games cool again! 🕹️
