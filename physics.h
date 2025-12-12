#ifndef PHYSICS_H
#define PHYSICS_H

#include "types.h"

// ============================================================================
// PHYSICS.H - Game Physics Function Declarations
// ============================================================================
// Purpose: Declares all physics and collision functions
// Why separate: Isolates game logic from rendering and input
// ============================================================================

// Ball physics
void updateBallPosition(Ball &ball);
void checkWallCollision(Ball &ball, GameState &state);
void checkPaddleCollision(Ball &ball, const Paddle &paddle, GameState &state);

// Paddle physics
void updatePaddlePosition(Paddle &paddle, bool moveUp, bool moveDown);

// AI logic
void updateAI(Paddle &aiPaddle, const Ball &ball);

// Particle system
void createParticles(GameState &state, Vector2D position, float vx, float vy);
void updateParticles(GameState &state);

// Game state management
void resetBall(Ball &ball, bool leftScored);
void resetGame(GameState &state);

#endif
