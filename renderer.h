#ifndef RENDERER_H
#define RENDERER_H

#include "types.h"

// ============================================================================
// RENDERER.H - Graphics Function Declarations
// ============================================================================
// Purpose: Declares all rendering functions
// Why separate: Separates interface from implementation
// ============================================================================

// Initialize OpenGL settings (called once at startup)
void initGraphics();

// Background rendering
void drawBackground(); // Animated gradient background
void drawCenterLine(); // Dashed center divider line

// Game object rendering
void drawPaddle(const Paddle &paddle);
void drawBall(const Ball &ball);
void drawParticles(const GameState &state);

// UI rendering
void drawScore(const GameState &state);
void drawPauseScreen();
void drawWinScreen(const GameState &state);

// Main display function (orchestrates all rendering)
void display(const GameState &state);

#endif
