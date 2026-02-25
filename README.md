# ProjectArkanoid

A classic Arkanoid (breakout) game built from scratch in **C++20** using **SFML 2.5** for rendering, audio, and input handling. The project leverages C++20 modules (`.ixx` / `.cpp` partition files) for clean modular architecture.

---

## Features

- **Multiple block types** — normal (multi-hit), indestructible, and bonus blocks
- **Bonus system** — collectible power-ups that extend the paddle, spawn extra balls, or increase ball speed
- **Level loader** — levels are defined in plain-text files (`assets/level*.txt`) with configurable paddle width and ball speed
- **State machine architecture** — menu, gameplay, nickname input, and game-over screens managed through a `GameStateManager`
- **Pause menu** — resume, restart, return to main menu, or exit mid-game
- **Level selection** — choose any unlocked level from the main menu
- **Score and lives tracking** — HUD displays current score and remaining lives
- **High score persistence** — per-player score history saved to disk with nickname support
- **Audio** — background music and sound effects (block hit, block destroy)
- **Collision system** — dedicated module handling ball-paddle, ball-block, and ball-wall interactions

## Screenshots

_(Add gameplay screenshots here)_

---

## Project Structure

```
ProjectArkanoid/
├── src/                        # Source code (C++20 modules)
│   ├── game.cpp                # Entry point
│   ├── game_state.ixx          # GameState interface
│   ├── game_state_manager.*    # State machine
│   ├── menu_state.*            # Main menu & level select
│   ├── playing_state.*         # Core gameplay loop
│   ├── game_over_state.*       # Game-over screen
│   ├── nickname_input_state.*  # Nickname entry for high scores
│   ├── ball.*                  # Ball entity
│   ├── paddle.*                # Paddle entity
│   ├── block.*                 # Block types (Normal, Indestructible, Bonus)
│   ├── bonus.*                 # Bonus drop logic
│   ├── collision_system.*      # Collision detection & response
│   ├── game_object_manager.*   # Object lifecycle management
│   ├── audio.*                 # Audio playback
│   ├── highscore.*             # High score I/O
│   └── config.*                # Level file parser
├── assets/                     # Game resources
│   ├── level1.txt, level2.txt  # Level definitions
│   ├── highscores.txt          # Persistent high scores
│   ├── *.png                   # Textures (background, ball, bonus)
│   ├── *.wav                   # Sound effects & music
│   └── arial.ttf               # Font
├── players/                    # Per-player score history
├── External/                   # Third-party dependencies (SFML headers/libs)
├── CMake.txt                   # CMake build configuration
├── ProjectArkanoid.sln         # Visual Studio solution
└── ProjectArkanoid.vcxproj     # Visual Studio project
```

## Level File Format

Each level is a plain-text file in `assets/` following this format:

```
paddleWidth: 120
ballSpeed: 200
1111111111111
2222222221111
1111111111111
3333333333333
```

| Digit | Block Type     |
| ----- | -------------- |
| `0`   | Empty          |
| `1`   | Normal         |
| `2`   | Indestructible |
| `3`   | Bonus          |

Parameters `paddleWidth` and `ballSpeed` are optional and must appear before the layout grid.

---

## Requirements

- **Compiler**: MSVC with C++20 module support (Visual Studio 2022 recommended)
- **Library**: [SFML 2.5](https://www.sfml-dev.org/) (graphics, audio, window, system)
- **OS**: Windows (pre-built SFML DLLs are included in the repository)

## Building

### Visual Studio (recommended)

1. Open `ProjectArkanoid.sln` in Visual Studio 2022.
2. Make sure the platform is set to **x64**.
3. Build and run (**F5**).

### CMake

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

> **Note:** The SFML installation path may need to be specified via `-DSFML_DIR=<path>` if it is not found automatically.

## Controls

| Key / Input | Action                 |
| ----------- | ---------------------- |
| Arrow keys  | Move paddle left/right |
| Escape      | Pause / unpause        |
| Mouse click | Navigate menus         |

---

## License

This project was created for educational purposes.
