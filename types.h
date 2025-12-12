#ifndef TYPES_H
#define TYPES_H

// ============================================================================
// TYPES.H - Core Data Structures
// ============================================================================
// Purpose: Defines all the game objects and their properties
// Why separate: Keeps data structures centralized for easy modification
// ============================================================================

// Window configuration
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

// Game speed configuration
const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS;

// ============================================================================
// Vector2D - 2D position/velocity representation
// ============================================================================
// Why: Encapsulates x,y coordinates for cleaner code
// Used for: Position, velocity, direction calculations
// ============================================================================
struct Vector2D
{
    float x, y;

    Vector2D() : x(0), y(0) {}
    Vector2D(float x, float y) : x(x), y(y) {}
};

// ============================================================================
// Paddle - Player/AI controlled paddle
// ============================================================================
// Properties explained:
// - position: Center point of paddle
// - width/height: Paddle dimensions (tall and narrow for vertical movement)
// - speed: Movement speed in pixels per frame
// - score: Player's current score
// - isAI: Determines if this paddle is controlled by AI or player
// ============================================================================
struct Paddle
{
    Vector2D position;
    float width;
    float height;
    float speed;
    int score;
    bool isAI;

    // Trail effect for visual enhancement
    Vector2D trailPositions[5]; // Stores last 5 positions for motion blur effect
    int trailIndex;             // Current index in circular buffer

    Paddle() : width(15), height(100), speed(8), score(0), isAI(false), trailIndex(0)
    {
        for (int i = 0; i < 5; i++)
        {
            trailPositions[i] = Vector2D(0, 0);
        }
    }
};

// ============================================================================
// Ball - The pong ball with enhanced physics
// ============================================================================
// Properties explained:
// - position: Current center position
// - velocity: Movement vector (x,y speed and direction)
// - radius: Ball size
// - speedMultiplier: Increases over time for difficulty scaling
// - trailPositions: Creates glowing trail effect behind ball
// - spinEffect: Visual rotation effect when ball bounces
// ============================================================================
struct Ball
{
    Vector2D position;
    Vector2D velocity;
    float radius;
    float speedMultiplier; // Gradually increases ball speed

    // Visual effects
    Vector2D trailPositions[10]; // Longer trail for ball (10 positions)
    int trailIndex;
    float spinEffect; // Rotation angle for visual spinning

    Ball() : radius(8), speedMultiplier(1.0f), trailIndex(0), spinEffect(0)
    {
        position = Vector2D(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
        velocity = Vector2D(6, 4); // Initial velocity
        for (int i = 0; i < 10; i++)
        {
            trailPositions[i] = position;
        }
    }
};

// ============================================================================
// Particle - For impact effects and visual feedback
// ============================================================================
// Why: Creates satisfying visual feedback when ball hits paddle/wall
// Properties:
// - position/velocity: Particle physics
// - life: Countdown timer (particles fade out)
// - size: Visual size of particle
// - color: RGB values that can vary per particle
// ============================================================================
struct Particle
{
    Vector2D position;
    Vector2D velocity;
    float life; // 1.0 = full life, 0.0 = dead (controls fade out)
    float size;
    float color[3]; // RGB color values

    Particle() : life(0), size(3)
    {
        color[0] = color[1] = color[2] = 1.0f;
    }
};

// ============================================================================
// GameState - Complete game state container
// ============================================================================
// Why: Encapsulates all game objects in one structure
// Benefits: Easy to pass around, clear game state, simple reset functionality
// ============================================================================
struct GameState
{
    Paddle leftPaddle;
    Paddle rightPaddle;
    Ball ball;
    Particle particles[100]; // Pool of particles for effects
    int activeParticles;     // How many particles are currently active
    bool isPaused;
    int winningScore; // Score needed to win (typically 11)

    GameState() : activeParticles(0), isPaused(false), winningScore(3) {}
};

#endif
