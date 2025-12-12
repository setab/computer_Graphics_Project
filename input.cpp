#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include "input.h"
#include <cctype>

// ============================================================================
// INPUT.CPP - Keyboard Input Management
// ============================================================================
// Purpose: Tracks keyboard state for smooth game controls
// Why separate: Clean input handling without cluttering game logic
// Technique: Key state array (true = pressed, false = released)
// ============================================================================

// Key state storage
// Using array for regular keys and bools for special keys (arrows)
bool keyStates[256]; // ASCII keys
bool upArrowPressed = false;
bool downArrowPressed = false;

// ============================================================================
// initInput() - Initialize input system
// ============================================================================
// Sets all keys to unpressed state
// Called once at startup
// ============================================================================
void initInput()
{
    for (int i = 0; i < 256; i++)
    {
        keyStates[i] = false;
    }
}

// ============================================================================
// handleKeyPress() - Regular key press event
// ============================================================================
// Called by GLUT when user presses a key
// Stores key state for smooth continuous movement
// Why: Allows checking "is key currently held?" rather than just press events
// ============================================================================
void handleKeyPress(unsigned char key, int x, int y)
{
    keyStates[tolower(key)] = true; // Store lowercase for consistency
}

// ============================================================================
// handleKeyRelease() - Regular key release event
// ============================================================================
// Called by GLUT when user releases a key
// Clears key state
// ============================================================================
void handleKeyRelease(unsigned char key, int x, int y)
{
    keyStates[tolower(key)] = false;
}

// ============================================================================
// handleSpecialKeyPress() - Arrow key press event
// ============================================================================
// GLUT separates arrow keys from regular keys
// We track up/down arrows for paddle control
// Why separate: GLUT's design separates special keys (arrows, function keys)
// ============================================================================
void handleSpecialKeyPress(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        upArrowPressed = true;
        break;
    case GLUT_KEY_DOWN:
        downArrowPressed = true;
        break;
    }
}

// ============================================================================
// handleSpecialKeyRelease() - Arrow key release event
// ============================================================================
void handleSpecialKeyRelease(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        upArrowPressed = false;
        break;
    case GLUT_KEY_DOWN:
        downArrowPressed = false;
        break;
    }
}

// ============================================================================
// Query functions - Check if key is currently pressed
// ============================================================================
// These allow game logic to ask "is W pressed right now?"
// Returns: true if key is held down, false otherwise
// ============================================================================

bool isKeyPressed(char key)
{
    return keyStates[tolower(key)];
}

bool isUpPressed()
{
    return upArrowPressed;
}

bool isDownPressed()
{
    return downArrowPressed;
}
