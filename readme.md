# Raylib Snake Game with Raygui Integration

A classic Arcade Snake Game written in C using the **Raylib** multimedia library and **Raygui** for immediate-mode graphical user interface elements. This project features enhanced visual feedback including custom particle bursts and dynamic game-speed configuration.

## Features

- **Interactive Main Menu:** Powered by `raygui`, allowing players to customize settings before launch.
- **Dynamic Difficulty:** Adjust the snake's step velocity smoothly via an interactive slider bar ranging from "Slow" to "Insane".
- **Visual Special Effects:**
  - **Pulsing Food:** The target fruit dynamically resizes using a sine-wave mathematical scale over time.
  - **Juicy Particle Bursts:** Exploding fire/spark particles disperse with randomized velocity vectors and linear alpha-fading whenever food is consumed.
- **Seamless Screen Wrapping:** Passing through boundaries safely teleports the snake to the opposing wall layout instead of causing instant death.
- **State Machine Management:** Fully managed game cycles handling states for `MENU`, `PLAYING`, `PAUSED`, and `GAMEOVER`.

---

## Controls

| Key | Action |
|---|---|
| **Arrow Up** | Move Snake Up |
| **Arrow Down** | Move Snake Down |
| **Arrow Left** | Move Snake Left |
| **Arrow Right** | Move Snake Right |
| **P / Escape** | Pause / Unpause Game |

---

## Prerequisites & Dependencies

To compile and execute this game, your environment must have:
1. A valid C compiler (GCC, Clang, or MSVC).
2. [Raylib Development Library](https://www.raylib.com/) (v4.0 or newer recommended).
3. [Raygui Header](https://github.com/raysan5/raygui) (Included alongside source files or universally accessible via header paths).

---

## Building and Running

### Linux / macOS
Ensure you have the required development build tools installed via your local package manager, then link raylib directly during compilation:

```bash
gcc main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o snake_game
./snake_game

SnakeGame/
├── .vscode/
│   └── tasks.json    
├── snake_game.c             
├── raygui.h           
└── README.md           
