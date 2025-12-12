#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#include <GLUT/glut.h>
#include <cmath>
#include <string>
#include <iostream>
#include "renderer.h"
#include "stb_image.h"

// ============================================================================
// RENDERER.CPP - All Visual Rendering Logic
// ============================================================================
// Purpose: Handles ALL graphics rendering for the game
// Why separate: Keeps visual code isolated from game logic
// OpenGL concepts used: Primitives, blending, color interpolation
// ============================================================================

// Global animation timer for background effects
float backgroundTimer = 0;

// Background texture
GLuint backgroundTexture = 0;
bool textureLoaded = false;

// ============================================================================
// loadBackgroundTexture() - Load image file as OpenGL texture
// ============================================================================
void loadBackgroundTexture(const char *filename)
{
    int width, height, channels;
    unsigned char *image = stbi_load(filename, &width, &height, &channels, STBI_rgb);

    if (!image)
    {
        std::cerr << "Failed to load background image: " << filename << std::endl;
        textureLoaded = false;
        return;
    }

    glGenTextures(1, &backgroundTexture);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);
    textureLoaded = true;

    std::cout << "Background image loaded successfully: " << width << "x" << height << std::endl;
}

// ============================================================================
// initGraphics() - OpenGL initialization
// ============================================================================
// Called once at startup to configure OpenGL rendering settings
// - Enable blending for transparency effects (trails, particles)
// - Enable anti-aliasing for smooth lines
// - Set blend function for proper alpha compositing
// - Load background texture
// ============================================================================
void initGraphics()
{
    glEnable(GL_BLEND);                                // Enable transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Standard alpha blending
    glEnable(GL_LINE_SMOOTH);                          // Smooth line rendering
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);            // Highest quality anti-aliasing
    glEnable(GL_TEXTURE_2D);                           // Enable 2D textures

    // Load background image
    loadBackgroundTexture("image1.jpg");
}

// ============================================================================
// drawBackground() - Render background image or gradient
// ============================================================================
// If texture loaded: Renders the background image
// If not loaded: Falls back to animated gradient
// ============================================================================
void drawBackground()
{
    if (textureLoaded)
    {
        // Draw textured background
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glColor3f(1.0f, 1.0f, 1.0f); // White to show image colors

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(0, 0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(WINDOW_WIDTH, 0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(0, WINDOW_HEIGHT);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        // Fallback to animated gradient
        backgroundTimer += 0.002f;

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= 20; i++)
        {
            float t = i / 20.0f;
            float r = 0.05f + 0.03f * sin(backgroundTimer + t);
            float g = 0.08f + 0.04f * sin(backgroundTimer * 1.3f);
            float b = 0.15f + 0.05f * sin(backgroundTimer * 0.7f);

            glColor3f(r, g, b);
            glVertex2f(0, t * WINDOW_HEIGHT);
            glVertex2f(WINDOW_WIDTH, t * WINDOW_HEIGHT);
        }
        glEnd();
    }
}

// ============================================================================
// drawCenterLine() - Dashed vertical divider
// ============================================================================
// Draws the classic pong center line
// Why: Visual separator between player zones
// Style: Dashed line with rounded ends for modern look
// ============================================================================
void drawCenterLine()
{
    glColor4f(0.3f, 0.3f, 0.4f, 0.6f); // Semi-transparent gray-blue
    glLineWidth(3);

    // Draw dashed line down the center
    float dashLength = 20;
    float gapLength = 15;

    for (float y = 0; y < WINDOW_HEIGHT; y += dashLength + gapLength)
    {
        glBegin(GL_LINES);
        glVertex2f(WINDOW_WIDTH / 2, y);
        glVertex2f(WINDOW_WIDTH / 2, y + dashLength);
        glEnd();
    }
}

// ============================================================================
// drawPaddle() - Render paddle with motion trail
// ============================================================================
// Draws paddle with glowing trail effect for visual feedback
// Trail: Shows recent positions with decreasing opacity
// Why: Makes movement feel more dynamic and responsive
// ============================================================================
void drawPaddle(const Paddle &paddle)
{
    // Draw motion trail (older positions fade out)
    for (int i = 0; i < 5; i++)
    {
        float alpha = (i + 1) / 6.0f * 0.3f; // Fade from 0.3 to 0.05
        glColor4f(0.5f, 0.7f, 1.0f, alpha);

        Vector2D trailPos = paddle.trailPositions[i];
        glBegin(GL_QUADS);
        glVertex2f(trailPos.x - paddle.width / 2, trailPos.y - paddle.height / 2);
        glVertex2f(trailPos.x + paddle.width / 2, trailPos.y - paddle.height / 2);
        glVertex2f(trailPos.x + paddle.width / 2, trailPos.y + paddle.height / 2);
        glVertex2f(trailPos.x - paddle.width / 2, trailPos.y + paddle.height / 2);
        glEnd();
    }

    // Draw main paddle body (bright white)
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(paddle.position.x - paddle.width / 2, paddle.position.y - paddle.height / 2);
    glVertex2f(paddle.position.x + paddle.width / 2, paddle.position.y - paddle.height / 2);
    glVertex2f(paddle.position.x + paddle.width / 2, paddle.position.y + paddle.height / 2);
    glVertex2f(paddle.position.x - paddle.width / 2, paddle.position.y + paddle.height / 2);
    glEnd();

    // Draw glowing outline for extra polish
    glColor4f(0.7f, 0.9f, 1.0f, 0.8f);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glVertex2f(paddle.position.x - paddle.width / 2 - 2, paddle.position.y - paddle.height / 2 - 2);
    glVertex2f(paddle.position.x + paddle.width / 2 + 2, paddle.position.y - paddle.height / 2 - 2);
    glVertex2f(paddle.position.x + paddle.width / 2 + 2, paddle.position.y + paddle.height / 2 + 2);
    glVertex2f(paddle.position.x - paddle.width / 2 - 2, paddle.position.y + paddle.height / 2 + 2);
    glEnd();
}

// ============================================================================
// drawBall() - Render ball with trail and spin effect
// ============================================================================
// Advanced ball rendering with multiple visual effects:
// 1. Motion trail (10 positions stored)
// 2. Spinning animation (rotates on bounce)
// 3. Glowing core
// 4. Outer glow ring
// Why: Makes ball movement exciting and easy to track
// ============================================================================
void drawBall(const Ball &ball)
{
    // Draw motion trail (long glowing tail)
    for (int i = 0; i < 10; i++)
    {
        float alpha = (i + 1) / 11.0f * 0.4f;                   // Fade along trail
        float size = ball.radius * (0.5f + (i / 10.0f) * 0.5f); // Shrink along trail

        glColor4f(1.0f, 0.8f, 0.3f, alpha); // Yellow-orange trail

        Vector2D trailPos = ball.trailPositions[i];
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(trailPos.x, trailPos.y);

        // Draw circle for each trail segment
        for (int j = 0; j <= 20; j++)
        {
            float angle = j * 2.0f * M_PI / 20.0f;
            glVertex2f(trailPos.x + cos(angle) * size,
                       trailPos.y + sin(angle) * size);
        }
        glEnd();
    }

    // Draw outer glow (larger, transparent circle)
    glColor4f(1.0f, 0.9f, 0.5f, 0.3f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(ball.position.x, ball.position.y);
    for (int i = 0; i <= 20; i++)
    {
        float angle = i * 2.0f * M_PI / 20.0f;
        glVertex2f(ball.position.x + cos(angle) * (ball.radius * 2),
                   ball.position.y + sin(angle) * (ball.radius * 2));
    }
    glEnd();

    // Draw main ball body with spinning effect
    glColor3f(1.0f, 1.0f, 0.9f); // Bright white-yellow
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(ball.position.x, ball.position.y);

    // Add spin rotation to vertices
    for (int i = 0; i <= 20; i++)
    {
        float angle = i * 2.0f * M_PI / 20.0f + ball.spinEffect; // Add spin rotation
        glVertex2f(ball.position.x + cos(angle) * ball.radius,
                   ball.position.y + sin(angle) * ball.radius);
    }
    glEnd();

    // Draw spin indicator lines (visual feedback of ball rotation)
    glColor4f(0.2f, 0.2f, 0.3f, 0.6f);
    glLineWidth(2);
    for (int i = 0; i < 4; i++)
    {
        float angle = (i * M_PI / 2.0f) + ball.spinEffect;
        glBegin(GL_LINES);
        glVertex2f(ball.position.x, ball.position.y);
        glVertex2f(ball.position.x + cos(angle) * ball.radius * 0.6f,
                   ball.position.y + sin(angle) * ball.radius * 0.6f);
        glEnd();
    }
}

// ============================================================================
// drawParticles() - Render particle effects
// ============================================================================
// Draws impact particles when ball hits paddle or wall
// Each particle has:
// - Position and velocity (flies outward from impact)
// - Life timer (fades out over time)
// - Color (can be customized per effect)
// Why: Provides satisfying visual feedback for collisions
// ============================================================================
void drawParticles(const GameState &state)
{
    glPointSize(4); // Particle size

    for (int i = 0; i < state.activeParticles; i++)
    {
        const Particle &p = state.particles[i];

        if (p.life > 0)
        {
            // Fade out based on remaining life
            glColor4f(p.color[0], p.color[1], p.color[2], p.life);

            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(p.position.x, p.position.y);

            // Draw small circle for each particle
            for (int j = 0; j <= 8; j++)
            {
                float angle = j * 2.0f * M_PI / 8.0f;
                glVertex2f(p.position.x + cos(angle) * p.size,
                           p.position.y + sin(angle) * p.size);
            }
            glEnd();
        }
    }
}

// ============================================================================
// drawScore() - Render player scores
// ============================================================================
// Displays scores using OpenGL bitmap fonts
// Position: Top corners of screen
// Style: Large, clear numbers
// ============================================================================
void drawScore(const GameState &state)
{
    glColor3f(0.7f, 0.8f, 0.9f); // Light blue-gray

    // Left player score
    std::string leftScore = std::to_string(state.leftPaddle.score);
    glRasterPos2f(WINDOW_WIDTH / 4 - 20, WINDOW_HEIGHT - 60);
    for (char c : leftScore)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Right player score
    std::string rightScore = std::to_string(state.rightPaddle.score);
    glRasterPos2f(3 * WINDOW_WIDTH / 4 - 20, WINDOW_HEIGHT - 60);
    for (char c : rightScore)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// ============================================================================
// drawPauseScreen() - Pause overlay
// ============================================================================
// Semi-transparent overlay with pause message
// Why: Clear visual feedback that game is paused
// ============================================================================
void drawPauseScreen()
{
    // Dark overlay
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();

    // Pause text
    glColor3f(1.0f, 1.0f, 1.0f);
    const char *text = "PAUSED - Press SPACE to continue";
    glRasterPos2f(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2);
    for (int i = 0; text[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

// ============================================================================
// drawWinScreen() - Victory message
// ============================================================================
// Displays winner and restart instructions
// Shown when a player reaches winning score
// ============================================================================
void drawWinScreen(const GameState &state)
{
    // Dark overlay
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();

    // Determine winner
    bool leftWon = state.leftPaddle.score >= state.winningScore;

    // Winner text
    glColor3f(1.0f, 0.9f, 0.3f); // Gold color
    const char *winText = leftWon ? "LEFT PLAYER WINS!" : "RIGHT PLAYER WINS!";
    glRasterPos2f(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 20);
    for (int i = 0; winText[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, winText[i]);
    }

    // Restart instruction
    glColor3f(0.8f, 0.8f, 0.8f);
    const char *restartText = "Press R to restart";
    glRasterPos2f(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 20);
    for (int i = 0; restartText[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, restartText[i]);
    }
}

// ============================================================================
// display() - Main rendering orchestrator
// ============================================================================
// Called every frame to render entire game
// Order matters: Background first, then game objects, then UI on top
// ============================================================================
void display(const GameState &state)
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear screen

    // Setup 2D orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Render in order (back to front)
    drawBackground();
    drawCenterLine();
    drawParticles(state);
    drawPaddle(state.leftPaddle);
    drawPaddle(state.rightPaddle);
    drawBall(state.ball);
    drawScore(state);

    // Overlays (if applicable)
    if (state.isPaused)
    {
        drawPauseScreen();
    }

    if (state.leftPaddle.score >= state.winningScore ||
        state.rightPaddle.score >= state.winningScore)
    {
        drawWinScreen(state);
    }

    glutSwapBuffers(); // Display rendered frame
}
