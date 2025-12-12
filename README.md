# Enhanced Pong Game

A modern take on the classic Pong game with smooth animations, particle effects, and AI opponent.

## Features

### Visual Enhancements

- **Animated gradient background** - Slowly pulsing colors
- **Motion trails** - Paddles and ball leave glowing trails
- **Particle effects** - Bursts on impacts with physics (gravity, fade out)
- **Ball spin effect** - Visual rotation and speed lines
- **Glowing elements** - Ball and paddles have soft glow

### Gameplay Features

- **Two-player mode** - W/S vs Arrow Keys
- **AI opponent** - Toggle with 'A' key
- **Dynamic difficulty** - Ball speeds up with each hit
- **Angle-based bounces** - Hit position affects ball direction
- **Score tracking** - First to 11 wins
- **Pause system** - Press SPACE to pause

## Controls

### Player 1 (Left Paddle)

- `W` - Move up
- `S` - Move down

### Player 2 (Right Paddle)

- `↑` - Move up
- `↓` - Move down

### Game Controls

- `SPACE` - Pause/Unpause
- `R` - Restart game
- `A` - Toggle AI for right paddle
- `ESC` - Quit game

## Building and Running

### Using Make (Recommended)

```bash
cd pong_game
make run
```

### Manual Compilation

```bash
clang++ -std=c++11 main.cpp renderer.cpp physics.cpp input.cpp -framework OpenGL -framework GLUT -o pong_game
./pong_game
```

### Clean Build Files

```bash
make clean
```

## File Structure

```
pong_game/
├── types.h           # Data structures (Paddle, Ball, Particle, GameState)
├── renderer.h/.cpp   # All graphics rendering (background, objects, UI)
├── physics.h/.cpp    # Game logic (movement, collisions, AI, particles)
├── input.h/.cpp      # Keyboard input handling
├── main.cpp          # Entry point, GLUT setup, game loop
├── Makefile          # Build automation
└── README.md         # This file
```

## Code Architecture

### Separation of Concerns

Each file handles a specific responsibility:

- **types.h**: Defines all data structures
- **renderer**: Pure visual rendering (no logic)
- **physics**: Game logic and calculations (no rendering)
- **input**: Keyboard state management
- **main**: Orchestration and game loop

### Why This Structure?

- **Maintainability**: Easy to find and modify specific features
- **Scalability**: Add new features without affecting existing code
- **Readability**: Clear separation makes code self-documenting
- **Debugging**: Isolate issues to specific modules

## Technical Highlights

### Graphics (OpenGL)

- Immediate mode rendering (glBegin/glEnd)
- Alpha blending for transparency effects
- Smooth line anti-aliasing
- Double buffering for flicker-free animation

### Physics

- AABB collision detection (rectangle overlap)
- Vector-based velocity system
- Angle-dependent bounce mechanics
- Speed scaling for difficulty progression

### Particle System

- Object pooling (100 particle buffer)
- Physics simulation (velocity, gravity)
- Lifetime management (fade out)
- Random scatter on impact

### AI Behavior

- Ball tracking with prediction
- Dead-zone for human-like imperfection
- Only reacts when ball approaches
- Balanced for fair gameplay

## Performance

- Runs at 60 FPS
- Efficient particle pooling (no dynamic allocation)
- Optimized collision detection
- Smooth animations via double buffering

## Future Enhancement Ideas

- Power-ups (speed boost, paddle size change)
- Multiple ball mode
- Sound effects
- Network multiplayer
- More advanced AI with prediction
- Customizable colors/themes
- Tournament mode with multiple rounds

## Requirements

- macOS (uses Apple's GLUT framework)
- C++11 compiler (clang++ or g++)
- OpenGL and GLUT frameworks

## License

Free to use and modify for educational purposes.
