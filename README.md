# 🐍 SNAKE GAME – Advanced Console Edition (C++)

> 🎮 A modern twist on the **classic Snake Game**, built entirely in **C++** for the Windows Console.  
> Featuring **colorful visuals**, **emoji-based UI**, **levels**, **power-ups**, **bombs**, and **a live high-score system**!  
> It’s fun, fast, and completely runs in your terminal 💻✨  

---

## 🌟 Highlights

| Feature | Description |
|----------|--------------|
| 🕹️ **Smooth Gameplay** | Real-time movement and keyboard control |
| 🍎 **Multiple Fruits** | Random emojis for fruits |
| ⚡ **Power Fruits** | Limited-time bonus items (+50 points!) |
| 💣 **Bombs** | Avoid or explode instantly 💀 |
| 🧱 **Obstacles** | Appear in hard mode for extra challenge |
| 🏆 **High Score System** | Saves and updates your top score automatically |
| 🕓 **Dynamic Speed** | Snake gets faster as you grow |
| 🎨 **Colorful Console UI** | Built with Windows console graphics |
| ⏸️ **Pause / Resume** | Press **P** anytime |
| 🔁 **Retry Option** | Play again instantly after Game Over |

---

## 🧩 Game Preview

```text
Player: Player_name | Score: 90 | Length: 15 | Level: 3 | High: 200
═════════════════════════════════════════════════════════
🐍🐍🐍🍒🧱🧱🧱💣
...
═════════════════════════════════════════════════════════
Power Fruit: [#####]     Bomb: [###  ]
🐍 Tip: The longer your snake grows, the faster it moves!


🎯 Objective
The goal is simple:
Eat fruits, grow longer, and survive as long as you can!
But beware of bombs, walls, and your own tail 👀


🧠 How to Play
Key	Action
⬆️ / ⬇️ / ⬅️ / ➡️	Move snake
P	Pause / Resume game
X	Exit game
R	Retry after Game Over
Enter	Confirm selection


⚙️ Setup & Run
🪟 For Windows Users:
Clone the Repository
--------------------------------------------------------------
bash
Copy code
git clone https://github.com/your-username/snake-game-cpp.git
Open the Project Folder
--------------------------------------------------------------
bash
Copy code
cd snake-game-cpp
Compile the Code
--------------------------------------------------------------
bash
Copy code
g++ snake.cpp -o snake.exe
Run the Game
--------------------------------------------------------------
bash
Copy code
snake.exe
✅ Make sure your console supports UTF-8 encoding and color output for best visuals!
--------------------------------------------------------------


🏗️ Project Structure
css
Copy code
📦 snake-game-cpp/
 ┣ 📜 snake.cpp          → Main source file
 ┣ 📜 highscore.txt      → Stores your highest score (auto-created)
 ┣ 📜 README.md          → You’re reading it right now 😄


🎮 Difficulty Levels
Mode	            Description	                 Speed	    Obstacles
 1     Easy	Relaxed and beginner-friendly	  🐢 Slow	     ❌ No
 2     Medium	Balanced gameplay	            ⚙️ Normal	   ❌ No
 3     Hard	For pros only	                  🚀 Fast	     ✅ Yes
💡 Higher levels unlock automatically as your score increases.


🧠 Game Mechanics
The core game loop is simple but efficient:
INPUT → LOGIC → DRAW → SLEEP → REPEAT
Input() – Handles real-time keyboard input
Logic() – Updates snake, collisions, and scoring
Draw() – Renders the board and stats
Sleep() – Controls game speed


🎯 The speed is dynamic — it increases as:
Your snake grows
Your level increases


💾 High Score System
🏆 Your best performance never gets lost!
Stored automatically in highscore.txt
Updates instantly when you beat your previous score
Displayed on the home screen every time you launch the game


🧩 Concepts Used :
Concept	Description
🧱 Object-Oriented Programming	Encapsulates all game logic inside a class
🕹️ Real-Time Input Handling	Using _kbhit() and _getch()
🎨 Console Manipulation	Controlled cursor, colors, and layout
🔢 Dynamic Data Structures	Snake body implemented with std::vector
🧠 Logic & Collision Detection	Handles fruits, bombs, and obstacles
⏱️ Timers & Randomization	For spawning power fruits and bombs
💾 File Handling	Saves high scores using fstream


💡 Future Enhancements : 
🚀 Planned or suggested improvements:
🎵 Add sound effects or music
🧑‍🤝‍🧑 Multiplayer mode
🌐 Online leaderboards
🧩 Custom skins and emojis


👨‍💻 Developers :
    202512024 - Heer Shah 
    202512025 - Diya Shah
    202512031 - Nigam Sanghvi
    202512059 - Jay Shah


🏁 Final Words
This Snake Game is not just about coding — it’s about creativity, logic, and fun 🐍💡
Experience how a few lines of C++ can bring life to your console screen 🎨

If you enjoyed this project, don’t forget to ⭐ Star the repository and share it with your friends!
