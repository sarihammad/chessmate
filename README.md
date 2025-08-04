# Chessmate

A full-featured C++ chess application with a clean architecture, supporting local multiplayer, online multiplayer using WebSocket, and AI play using minimax algorithm with alpha-beta pruning. Built with SFML for the GUI and designed for maintainability, extensibility, and testability.

## Demo

### Local Demo

![188940128-b0916b22-a747-4e29-83c7-4596eb01ab9a-ezgif com-crop](https://github.com/user-attachments/assets/52098b11-f8b6-4262-ac85-01cacff6bdb1)

### Multiplayer Preview

<img width="1121" height="579" alt="Screenshot 2025-08-03 at 7 42 47 pm" src="https://github.com/user-attachments/assets/195bd620-d7d6-4495-bf55-45bb6ffc00f9" />

---

## Architecture

```mermaid
graph TD
    %% Entry point
    Start(["User Launches App"]) --> A1

    %% GUI Layer
    subgraph "GUI Layer (SFML)"
        A1["Main Menu<br>(runMainMenu)"] -->|"User Selection"| A2["Game GUI<br>(runGUI)"]
        A2 -->|"Mouse Events"| A3["Board View<br>(BoardView)"]
        A3 -->|"Rendering"| A4["SFML Window<br>& Sprites"]
        A2 -->|"UI Events"| A5["Button Widgets<br>& Overlays"]
    end

    %% Application Layer
    subgraph "Application Layer"
        B1["Game Controller<br>(GameController)"] -->|"Orchestrates"| B2["Game State<br>Manager"]
        B2 -->|"Turn Control"| B3["Player Interface"]
        B3 -->|"Move Validation"| B4["Game Engine<br>(Game)"]
    end

    %% Domain Layer
    subgraph "Domain Layer"
        C1["Board<br>(Board)"] -->|"Contains"| C2["Pieces<br>(Piece Hierarchy)"]
        C2 -->|"Move Generation"| C3["Move Logic"]
        C3 -->|"State Updates"| C4["Game State<br>(GameState)"]
        C4 -->|"Applies Rules"| C5["Rules Engine<br>(Check, Mate, Draw)"]

        %% Piece Hierarchy
        C2 --> P1["Piece<br>(Abstract)"]
        P1 --> P2["Pawn"]
        P1 --> P3["Rook"]
        P1 --> P4["Knight"]
        P1 --> P5["Bishop"]
        P1 --> P6["Queen"]
        P1 --> P7["King"]
    end

    %% Infrastructure Layer
    subgraph "Infrastructure Layer"
        D1["AI Player<br>(AIPlayer)"] -->|"Uses"| D2["AI Evaluator<br>(Minimax + Alpha-Beta)"]
        D3["Network Player<br>(NetworkPlayer)"] -->|"Communicates"| D4["WebSocket Client"]
        D4 -->|"JSON Messages"| D5["Spring Boot Server"]
        D6["Human Player<br>(HumanPlayer)"] -->|"Input"| D7["Mouse/Keyboard<br>Event Handling"]
    end

    %% Cross-layer connections
    A2 -->|"Game Actions"| B1
    B1 -->|"Board Updates"| C1
    B3 -->|"Player Type"| D1
    B3 -->|"Player Type"| D3
    B3 -->|"Player Type"| D6
```

---

## System Overview

Chessmate implements a Clean Architecture pattern with clear separation of concerns across four main layers:

### **GUI Layer**

- **Main Menu System**: Handles game mode selection (Local, AI, Multiplayer)
- **Game Interface**: Real-time board rendering with click-and-drag piece movement
- **Event Processing**: Mouse and keyboard input handling with visual feedback
- **UI Components**: Buttons, overlays, and confirmation dialogs

### **Application Layer**

- **Game Controller**: Central orchestrator managing game flow and state transitions
- **Player Management**: Creates and manages different player types (Human, AI, Network)
- **Game Loop**: Coordinates between GUI events and domain logic

### **Domain Layer**

- **Board Representation**: 8x8 grid with piece placement and movement tracking
- **Piece Hierarchy**: Polymorphic piece system (King, Queen, Rook, Bishop, Knight, Pawn)
- **Move Validation**: Legal move generation and chess rule enforcement
- **Game State**: Turn management, check/checkmate detection, and game termination

### **Infrastructure Layer**

- **AI Engine**: Minimax algorithm with alpha-beta pruning for intelligent gameplay
- **Network Communication**: WebSocket client for real-time multiplayer
- **Player Implementations**: Human input processing, AI decision making, network synchronization

The system supports three game modes:

1. **Local Multiplayer**: Two players on the same device
2. **AI Mode**: Human vs AI engine
3. **Online Multiplayer**: Real-time play over WebSocket with Spring Boot backend

---

## Features

- **Local Multiplayer:** Two players on the same device with intuitive click-and-drag interface
- **AI Opponent:** Minimax algorithm with alpha-beta pruning for challenging gameplay
- **Online Multiplayer:** Real-time play over WebSocket with Spring Boot backend
- **Modern UI:** Gradient menus, polished buttons, and in-game overlays
- **Game Over Menu:** Restart, Main Menu, and Quit options after each game
- **Clean Architecture:** Clear separation of domain, application, GUI, and infrastructure layers
- **Sound Effects:** Audio feedback for moves, captures, and game events
- **Visual Feedback:** Valid move indicators and capture square highlighting

---

## Tech Stack

- **Frontend:** SFML (Graphics, Window, System, Audio), C++17
- **Backend:** Spring Boot (Java), WebSocket, Maven
- **AI:** Minimax algorithm with alpha-beta pruning
- **Networking:** WebSocket++, nlohmann/json
- **Build System:** CMake, Make

---

## Build & Run

### Build Instructions

#### 1. Clone and Setup

```bash
git clone https://github.com/sarihammad/chessmate.git
cd chessmate
```

#### 2. Build Game Client

```bash
cd game
mkdir build && cd build
cmake ..
make
```

#### 3. Build Server (Optional for multiplayer)

```bash
cd ../../server
mvn spring-boot:run
```

### Run the Application

#### Game Client

```bash
cd game/build
./chessmate
```

#### Multiplayer Server

```bash
cd server
java -jar target/chessmate-server-1.0.0.jar
```

---

### Performance Considerations

- **AI Optimization**: Alpha-beta pruning reduces search space
- **Rendering**: SFML hardware acceleration for smooth graphics
- **Network**: WebSocket for low-latency multiplayer
