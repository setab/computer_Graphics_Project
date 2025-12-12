#ifndef AUDIO_H
#define AUDIO_H

// ============================================================================
// AUDIO.H - Audio System Interface
// ============================================================================
// Purpose: Declares audio functions for music and sound effects
// Library: OpenAL (built into macOS)
// ============================================================================

// Audio initialization and cleanup
void initAudio();
void cleanupAudio();

// Background music controls
void playBackgroundMusic();
void stopBackgroundMusic();
void pauseBackgroundMusic();
void resumeBackgroundMusic();

// Sound effects
void playPaddleHitSound();
void playWallHitSound();
void playScoreSound();
void playWinSound();

// Volume controls (0.0 to 1.0)
void setMusicVolume(float volume);
void setSoundVolume(float volume);

#endif
