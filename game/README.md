# Chessmate - Chess Game

A modern chess game built with C++ and SFML, featuring local play, AI opponents, and online multiplayer.

## Architecture

### Class Responsibilities

#### Game

- **Owns the board, the players, the game state, and all game logic.**
- Responsible for move validation, turn management, game result calculation, and sound effects.
- Exposes methods to set players, play turns, and query game state.

#### GameController

- **Orchestrates the game loop, state transitions, and delegates to domain logic and UI.**
- Responsible for creating the Game instance, instantiating the correct player types, and setting them in the Game.
- Manages the main window, event loop, and user interactions.
- Does not own player objects directly; all player management is done via the Game instance.

#### BoardView / GUI

- **Responsible for rendering the chess board, pieces, and UI elements.**
- Receives board state from the Game and draws it to the screen.
- Handles user input and forwards relevant actions to the GameController.

## Features

- **Local Game**: Play chess against another human player on the same machine
- **AI Game**: Play against an AI opponent with configurable difficulty
- **Online Game**: Play against other players over the network (requires server)
- **Sound Effects**: Move sounds, capture sounds, check/checkmate announcements
- **Modern UI**: Clean, responsive interface with hover effects and visual feedback

## Building

### Prerequisites

- CMake 3.10 or higher
- SFML 2.5 or higher
- nlohmann/json
- websocketpp
- C++17 compatible compiler

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

### Running

```bash
./chessmate
```

## Game Modes

1. **Local Game**: Two human players take turns on the same machine
2. **AI Game**: Choose your color and play against the computer
3. **Online Game**: Connect to a chess server to play against other players

## Controls

- **Mouse**: Click to select pieces and make moves
- **Menu**: Use the main menu to select game modes
- **Game Over**: Automatic detection of checkmate, stalemate, and insufficient material

## File Structure

```
game/
├── include/           # Header files
│   ├── application/   # Game controller and application logic
│   ├── domain/        # Core game logic (board, pieces, game state)
│   ├── gui/          # User interface components
│   └── infrastructure/ # External integrations (AI, network, players)
├── src/              # Implementation files
├── assets/           # Images, sounds, fonts
└── tests/            # Unit tests
```

## Development

The project follows clean architecture principles with clear separation of concerns:

- **Domain Layer**: Core chess logic independent of UI or external systems
- **Application Layer**: Orchestrates the game flow and manages state
- **Infrastructure Layer**: Handles external dependencies (AI, networking)
- **GUI Layer**: User interface and rendering

For more details, see the code comments in each class header.
