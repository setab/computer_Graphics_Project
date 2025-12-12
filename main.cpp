#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <cstdlib>
#include <cctype>
#include "types.h"
#include "renderer.h"
#include "physics.h"
#include "input.h"
#include "audio.h"

// ============================================================================
// MAIN.CPP - Game Loop and GLUT Integration
// ============================================================================
// Purpose: Main entry point, GLUT setup, game loop orchestration
// Why separate: Coordinates all systems without implementing details
// Architecture: Calls functions from other modules (renderer, physics, input)
// ============================================================================

// Global game state
GameState gameState;

// ============================================================================
// gameLoop() - Main update function called every frame
// ============================================================================
// This is the heart of the game - called 60 times per second
// Order of operations:
// 1. Check input
// 2. Update physics
// 3. Check collisions
// 4. Update particles
// 5. Trigger display refresh
// Why: Separates update logic from rendering (standard game architecture)
// ============================================================================
void gameLoop(int value)
{
    (void)value; // Suppress unused parameter warning

    // Skip updates if paused or game is over
    if (gameState.isPaused ||
        gameState.leftPaddle.score >= gameState.winningScore ||
        gameState.rightPaddle.score >= gameState.winningScore)
    {
        glutTimerFunc(FRAME_DELAY, gameLoop, 0);
        return;
    }

    // ========================================
    // INPUT HANDLING
    // ========================================
    // Left paddle (W/S keys)
    bool leftUp = isKeyPressed('w');
    bool leftDown = isKeyPressed('s');
    updatePaddlePosition(gameState.leftPaddle, leftUp, leftDown);

    // Right paddle (Arrow keys or AI)
    if (gameState.rightPaddle.isAI)
    {
        // AI controls right paddle
        updateAI(gameState.rightPaddle, gameState.ball);
    }
    else
    {
        // Player controls right paddle
        bool rightUp = isUpPressed();
        bool rightDown = isDownPressed();
        updatePaddlePosition(gameState.rightPaddle, rightUp, rightDown);
    }

    // ========================================
    // BALL PHYSICS
    // ========================================
    updateBallPosition(gameState.ball);
    checkWallCollision(gameState.ball, gameState);
    checkPaddleCollision(gameState.ball, gameState.leftPaddle, gameState);
    checkPaddleCollision(gameState.ball, gameState.rightPaddle, gameState);

    // ========================================
    // SCORING
    // ========================================
    // Check if ball went off left side (right player scores)
    if (gameState.ball.position.x < 0)
    {
        gameState.rightPaddle.score++;
        playScoreSound();                 // Play score sound
        resetBall(gameState.ball, false); // Reset towards left

        // Check for win
        if (gameState.rightPaddle.score >= gameState.winningScore)
        {
            playWinSound();
            stopBackgroundMusic();
        }
    }

    // Check if ball went off right side (left player scores)
    if (gameState.ball.position.x > WINDOW_WIDTH)
    {
        gameState.leftPaddle.score++;
        playScoreSound();                // Play score sound
        resetBall(gameState.ball, true); // Reset towards right

        // Check for win
        if (gameState.leftPaddle.score >= gameState.winningScore)
        {
            playWinSound();
            stopBackgroundMusic();
        }
    }

    // ========================================
    // PARTICLE SYSTEM
    // ========================================
    updateParticles(gameState);

    // Trigger redraw
    glutPostRedisplay();

    // Schedule next frame (60 FPS)
    glutTimerFunc(FRAME_DELAY, gameLoop, 0);
}

// ============================================================================
// displayWrapper() - GLUT display callback
// ============================================================================
// GLUT requires a function with specific signature
// This wrapper calls our display function with game state
// ============================================================================
void displayWrapper()
{
    display(gameState);
}

// ============================================================================
// keyboardWrapper() - Handle gameplay keys
// ============================================================================
// Processes game control keys (pause, restart, quit)
// Movement keys are handled by input system
// Why: Separates game commands from continuous input
// ============================================================================
void keyboardWrapper(unsigned char key, int x, int y)
{
    handleKeyPress(key, x, y); // Update input state

    // Handle special commands
    switch (tolower(key))
    {
    case ' ': // Spacebar - pause/unpause
        gameState.isPaused = !gameState.isPaused;
        if (gameState.isPaused)
        {
            pauseBackgroundMusic();
        }
        else
        {
            resumeBackgroundMusic();
        }
        break;

    case 'r': // R - restart game
        resetGame(gameState);
        playBackgroundMusic(); // Restart music
        break;

    case 27: // ESC - quit
        cleanupAudio();
        exit(0);
        break;

    case 'a': // A - toggle AI for right paddle
        gameState.rightPaddle.isAI = !gameState.rightPaddle.isAI;
        break;
    }
}

// ============================================================================
// keyboardUpWrapper() - Handle key releases
// ============================================================================
void keyboardUpWrapper(unsigned char key, int x, int y)
{
    handleKeyRelease(key, x, y);
}

// ============================================================================
// specialWrapper() - Handle arrow key presses
// ============================================================================
void specialWrapper(int key, int x, int y)
{
    handleSpecialKeyPress(key, x, y);
}

// ============================================================================
// specialUpWrapper() - Handle arrow key releases
// ============================================================================
void specialUpWrapper(int key, int x, int y)
{
    handleSpecialKeyRelease(key, x, y);
}

// ============================================================================
// main() - Program entry point
// ============================================================================
// Setup sequence:
// 1. Initialize GLUT (windowing system)
// 2. Configure window
// 3. Initialize game systems
// 4. Register callbacks
// 5. Start game loop
// 6. Enter GLUT main loop (runs forever)
// ============================================================================
int main(int argc, char **argv)
{
    // ========================================
    // GLUT INITIALIZATION
    // ========================================
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Double buffering for smooth animation
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Enhanced Pong - W/S and Arrow Keys - SPACE: Pause - R: Restart - A: Toggle AI");

    // ========================================
    // GAME INITIALIZATION
    // ========================================
    initGraphics();       // Setup OpenGL
    initInput();          // Setup input system
    initAudio();          // Setup audio system
    resetGame(gameState); // Initialize game state

    // Set right paddle to AI by default
    gameState.rightPaddle.isAI = true;

    // Start background music
    playBackgroundMusic();

    // ========================================
    // REGISTER CALLBACKS
    // ========================================
    // Display callback - renders frame
    glutDisplayFunc(displayWrapper);

    // Keyboard callbacks - handle input
    glutKeyboardFunc(keyboardWrapper);
    glutKeyboardUpFunc(keyboardUpWrapper);
    glutSpecialFunc(specialWrapper);
    glutSpecialUpFunc(specialUpWrapper);

    // Timer callback - game loop at 60 FPS
    glutTimerFunc(0, gameLoop, 0);

    // ========================================
    // START GAME
    // ========================================
    glutMainLoop(); // Never returns - GLUT takes over

    return 0;
}
