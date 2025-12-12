#include "physics.h"
#include "audio.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

// ============================================================================
// PHYSICS.CPP - All Game Logic and Physics
// ============================================================================
// Purpose: Handles movement, collisions, AI, and game rules
// Why separate: Clean separation between logic and visuals
// Physics concepts: Velocity, collision detection, vector math
// ============================================================================

// ============================================================================
// updateBallPosition() - Move ball based on velocity
// ============================================================================
// Updates ball position every frame
// Also updates trail positions for visual effect
// Why: Core game mechanic - ball must move continuously
// ============================================================================
void updateBallPosition(Ball &ball)
{
    // Update position based on velocity and speed multiplier
    ball.position.x += ball.velocity.x * ball.speedMultiplier;
    ball.position.y += ball.velocity.y * ball.speedMultiplier;

    // Update spin effect (rotates ball visually)
    ball.spinEffect += 0.1f * ball.speedMultiplier;

    // Update trail (store current position in circular buffer)
    ball.trailIndex = (ball.trailIndex + 1) % 10;
    ball.trailPositions[ball.trailIndex] = ball.position;
}

// ============================================================================
// checkWallCollision() - Detect ball hitting top/bottom walls
// ============================================================================
// Bounces ball off top and bottom walls
// Also creates particle effect at impact point
// Why: Keeps ball in play area, adds visual feedback
// Physics: Velocity reflection (reverse Y component)
// ============================================================================
void checkWallCollision(Ball &ball, GameState &state)
{
    // Top wall collision
    if (ball.position.y + ball.radius >= WINDOW_HEIGHT)
    {
        ball.position.y = WINDOW_HEIGHT - ball.radius; // Correct position
        ball.velocity.y = -ball.velocity.y;            // Reverse direction

        // Create particle burst at impact point
        createParticles(state, ball.position, ball.velocity.x, -ball.velocity.y);
        playWallHitSound(); // Play sound effect
    }

    // Bottom wall collision
    if (ball.position.y - ball.radius <= 0)
    {
        ball.position.y = ball.radius;
        ball.velocity.y = -ball.velocity.y;

        createParticles(state, ball.position, ball.velocity.x, -ball.velocity.y);
        playWallHitSound(); // Play sound effect
    }
}

// ============================================================================
// checkPaddleCollision() - Detect ball hitting paddle
// ============================================================================
// Complex collision detection with angle-based bounce
// Key features:
// 1. AABB collision detection (rectangle overlap test)
// 2. Bounce angle varies based on where ball hits paddle
// 3. Speed increases slightly on each hit (difficulty scaling)
// 4. Particle effects at impact
// Why: Core gameplay mechanic, creates skill-based gameplay
// ============================================================================
void checkPaddleCollision(Ball &ball, const Paddle &paddle, GameState &state)
{
    // AABB (Axis-Aligned Bounding Box) collision detection
    // Check if ball's bounding box overlaps with paddle's bounding box
    bool collision =
        ball.position.x + ball.radius >= paddle.position.x - paddle.width / 2 &&
        ball.position.x - ball.radius <= paddle.position.x + paddle.width / 2 &&
        ball.position.y + ball.radius >= paddle.position.y - paddle.height / 2 &&
        ball.position.y - ball.radius <= paddle.position.y + paddle.height / 2;

    if (collision)
    {
        // Calculate where on paddle the ball hit (-1 = top, 0 = center, 1 = bottom)
        float hitPosition = (ball.position.y - paddle.position.y) / (paddle.height / 2);

        // Reverse horizontal direction
        ball.velocity.x = -ball.velocity.x;

        // Adjust vertical angle based on hit position
        // Hitting near edges creates sharper angles (more interesting gameplay)
        ball.velocity.y = hitPosition * 8; // Max 8 pixels/frame vertical speed

        // Gradually increase speed (difficulty scaling)
        ball.speedMultiplier += 0.05f;
        if (ball.speedMultiplier > 2.0f)
        { // Cap at 2x speed
            ball.speedMultiplier = 2.0f;
        }

        // Move ball outside paddle to prevent multiple collisions
        if (ball.velocity.x > 0)
        {
            ball.position.x = paddle.position.x + paddle.width / 2 + ball.radius + 1;
        }
        else
        {
            ball.position.x = paddle.position.x - paddle.width / 2 - ball.radius - 1;
        }

        // Create particle burst at paddle impact
        createParticles(state, ball.position, ball.velocity.x, ball.velocity.y);
        playPaddleHitSound(); // Play sound effect
    }
}

// ============================================================================
// updatePaddlePosition() - Move paddle based on input
// ============================================================================
// Updates paddle position with boundary checking
// Why: Handles player/AI movement commands
// Includes trail update for motion blur effect
// ============================================================================
void updatePaddlePosition(Paddle &paddle, bool moveUp, bool moveDown)
{
    // Update trail positions (circular buffer)
    paddle.trailIndex = (paddle.trailIndex + 1) % 5;
    paddle.trailPositions[paddle.trailIndex] = paddle.position;

    // Move up
    if (moveUp && paddle.position.y + paddle.height / 2 < WINDOW_HEIGHT)
    {
        paddle.position.y += paddle.speed;
    }

    // Move down
    if (moveDown && paddle.position.y - paddle.height / 2 > 0)
    {
        paddle.position.y -= paddle.speed;
    }
}

// ============================================================================
// updateAI() - Simple AI for computer opponent
// ============================================================================
// AI Strategy:
// 1. Track ball's Y position
// 2. Move paddle towards ball
// 3. Add slight delay/error for fairness (not perfect)
// Why: Provides single-player mode
// Balance: Fast enough to be challenging, slow enough to be beatable
// ============================================================================
void updateAI(Paddle &aiPaddle, const Ball &ball)
{
    // Only react if ball is moving towards AI paddle
    bool ballMovingTowardAI =
        (aiPaddle.position.x > WINDOW_WIDTH / 2 && ball.velocity.x > 0) ||
        (aiPaddle.position.x < WINDOW_WIDTH / 2 && ball.velocity.x < 0);

    if (!ballMovingTowardAI)
        return; // Don't move if ball going away

    // Calculate desired position (with slight offset for imperfection)
    float targetY = ball.position.y;
    float deadZone = 15; // AI won't react to small differences (makes it beatable)

    // Move towards ball position
    if (aiPaddle.position.y < targetY - deadZone)
    {
        updatePaddlePosition(aiPaddle, true, false); // Move up
    }
    else if (aiPaddle.position.y > targetY + deadZone)
    {
        updatePaddlePosition(aiPaddle, false, true); // Move down
    }
}

// ============================================================================
// createParticles() - Spawn particle effect
// ============================================================================
// Creates burst of particles at specified position
// Particles fly outward in random directions
// Why: Visual feedback for impacts (juice/polish)
// Parameters: position, base velocity for direction hint
// ============================================================================
void createParticles(GameState &state, Vector2D position, float vx, float vy)
{
    // Create 15 particles per impact
    for (int i = 0; i < 15; i++)
    {
        // Find next available particle slot
        int index = (state.activeParticles + i) % 100;
        Particle &p = state.particles[index];

        // Set particle properties
        p.position = position;
        p.life = 1.0f;             // Full life
        p.size = 2 + (rand() % 3); // Random size 2-4

        // Random velocity (scatter effect)
        float angle = (rand() % 360) * M_PI / 180.0f;
        float speed = 2 + (rand() % 4);
        p.velocity.x = cos(angle) * speed;
        p.velocity.y = sin(angle) * speed;

        // Color based on impact velocity (yellow-orange palette)
        p.color[0] = 1.0f;                          // Red
        p.color[1] = 0.7f + (rand() % 30) / 100.0f; // Green (0.7-1.0)
        p.color[2] = 0.2f + (rand() % 30) / 100.0f; // Blue (0.2-0.5)
    }

    state.activeParticles = (state.activeParticles + 15) % 100;
}

// ============================================================================
// updateParticles() - Update all active particles
// ============================================================================
// Each frame:
// 1. Move particles based on velocity
// 2. Apply gravity (particles fall)
// 3. Decrease life (particles fade out)
// Why: Animate particle effects over time
// ============================================================================
void updateParticles(GameState &state)
{
    for (int i = 0; i < 100; i++)
    {
        Particle &p = state.particles[i];

        if (p.life > 0)
        {
            // Update position
            p.position.x += p.velocity.x;
            p.position.y += p.velocity.y;

            // Apply gravity (particles fall down)
            p.velocity.y -= 0.2f;

            // Fade out over time
            p.life -= 0.02f;

            // Shrink as they fade
            p.size -= 0.05f;
            if (p.size < 0.5f)
                p.size = 0.5f;
        }
    }
}

// ============================================================================
// resetBall() - Reset ball after scoring
// ============================================================================
// Returns ball to center with new random direction
// Why: Called when a point is scored
// Direction: Towards player who was scored on (fair restart)
// ============================================================================
void resetBall(Ball &ball, bool leftScored)
{
    ball.position = Vector2D(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
    ball.speedMultiplier = 1.0f; // Reset speed
    ball.spinEffect = 0;

    // Random vertical direction
    float randomY = (rand() % 100 - 50) / 10.0f; // -5 to +5

    // Move towards player who was scored on
    if (leftScored)
    {
        ball.velocity = Vector2D(-6, randomY); // Move left
    }
    else
    {
        ball.velocity = Vector2D(6, randomY); // Move right
    }

    // Reset trail
    for (int i = 0; i < 10; i++)
    {
        ball.trailPositions[i] = ball.position;
    }
}

// ============================================================================
// resetGame() - Complete game reset
// ============================================================================
// Resets all game state to starting conditions
// Called when: Game starts, player requests restart
// Why: Provides clean slate for new game
// ============================================================================
void resetGame(GameState &state)
{
    // Reset scores
    state.leftPaddle.score = 0;
    state.rightPaddle.score = 0;

    // Reset paddle positions
    state.leftPaddle.position = Vector2D(50, WINDOW_HEIGHT / 2.0f);
    state.rightPaddle.position = Vector2D(WINDOW_WIDTH - 50, WINDOW_HEIGHT / 2.0f);

    // Reset paddle trails
    for (int i = 0; i < 5; i++)
    {
        state.leftPaddle.trailPositions[i] = state.leftPaddle.position;
        state.rightPaddle.trailPositions[i] = state.rightPaddle.position;
    }

    // Reset ball
    resetBall(state.ball, false);

    // Clear particles
    state.activeParticles = 0;
    for (int i = 0; i < 100; i++)
    {
        state.particles[i].life = 0;
    }

    state.isPaused = false;

    // Seed random number generator
    srand(static_cast<unsigned>(time(0)));
}
