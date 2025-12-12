#ifndef INPUT_H
#define INPUT_H

#include "types.h"

// ============================================================================
// INPUT.H - Input Handling Function Declarations
// ============================================================================
// Purpose: Declares keyboard input functions
// Why separate: Isolates user input from game logic
// ============================================================================

// Initialize input state
void initInput();

// GLUT callback handlers
void handleKeyPress(unsigned char key, int x, int y);
void handleKeyRelease(unsigned char key, int x, int y);
void handleSpecialKeyPress(int key, int x, int y);
void handleSpecialKeyRelease(int key, int x, int y);

// Get current input state
bool isKeyPressed(char key);
bool isUpPressed();
bool isDownPressed();

#endif
