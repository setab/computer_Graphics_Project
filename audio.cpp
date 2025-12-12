#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "audio.h"

// ============================================================================
// AUDIO.CPP - Audio System Implementation
// ============================================================================
// Purpose: Manages background music and sound effects using OpenAL
// OpenAL concepts: Device, Context, Buffers, Sources
// ============================================================================

// OpenAL device and context
static ALCdevice *device = nullptr;
static ALCcontext *context = nullptr;

// Audio buffers (store sound data)
static ALuint musicBuffer = 0;
static ALuint paddleHitBuffer = 0;
static ALuint wallHitBuffer = 0;
static ALuint scoreBuffer = 0;
static ALuint winBuffer = 0;

// Audio sources (play sound data)
static ALuint musicSource = 0;
static ALuint sfxSource = 0;

// Volume settings
static float musicVolume = 0.5f;
static float soundVolume = 0.7f;

// ============================================================================
// WAV file loading utilities
// ============================================================================

struct WAVHeader
{
    char riff[4]; // "RIFF"
    int32_t fileSize;
    char wave[4]; // "WAVE"
    char fmt[4];  // "fmt "
    int32_t fmtSize;
    int16_t audioFormat;
    int16_t numChannels;
    int32_t sampleRate;
    int32_t byteRate;
    int16_t blockAlign;
    int16_t bitsPerSample;
};

// Load WAV file and return OpenAL buffer
ALuint loadWAV(const char *filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open audio file: " << filename << std::endl;
        return 0;
    }

    // Read WAV header
    WAVHeader header;
    file.read(reinterpret_cast<char *>(&header), sizeof(WAVHeader));

    // Skip to data chunk
    char chunkId[4];
    int32_t chunkSize;
    while (file.read(chunkId, 4))
    {
        file.read(reinterpret_cast<char *>(&chunkSize), sizeof(int32_t));
        if (std::string(chunkId, 4) == "data")
        {
            break;
        }
        file.seekg(chunkSize, std::ios::cur);
    }

    // Read audio data
    std::vector<char> data(chunkSize);
    file.read(data.data(), chunkSize);
    file.close();

    // Determine OpenAL format
    ALenum format;
    if (header.numChannels == 1)
    {
        format = (header.bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    }
    else
    {
        format = (header.bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    }

    // Create OpenAL buffer
    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, data.data(), chunkSize, header.sampleRate);

    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        std::cerr << "OpenAL error loading " << filename << ": " << error << std::endl;
        return 0;
    }

    std::cout << "Loaded audio: " << filename << std::endl;
    return buffer;
}

// ============================================================================
// initAudio() - Initialize OpenAL and load all audio files
// ============================================================================
void initAudio()
{
    // Open default audio device
    device = alcOpenDevice(nullptr);
    if (!device)
    {
        std::cerr << "Failed to open audio device" << std::endl;
        return;
    }

    // Create audio context
    context = alcCreateContext(device, nullptr);
    if (!context || !alcMakeContextCurrent(context))
    {
        std::cerr << "Failed to create audio context" << std::endl;
        if (context)
            alcDestroyContext(context);
        alcCloseDevice(device);
        return;
    }

    std::cout << "OpenAL initialized successfully" << std::endl;

    // Load audio files (WAV format)
    musicBuffer = loadWAV("music/background_music.wav");
    paddleHitBuffer = loadWAV("music/paddle_hit.wav");
    // wallHitBuffer = loadWAV("music/wall_hit.wav");
    scoreBuffer = loadWAV("music/score.wav");
    winBuffer = loadWAV("music/win.wav");

    // Create audio sources
    alGenSources(1, &musicSource);
    alGenSources(1, &sfxSource);

    // Configure music source (looping)
    if (musicBuffer)
    {
        alSourcei(musicSource, AL_BUFFER, musicBuffer);
        alSourcei(musicSource, AL_LOOPING, AL_TRUE);
        alSourcef(musicSource, AL_GAIN, musicVolume);
    }

    // Configure sound effects source
    alSourcef(sfxSource, AL_GAIN, soundVolume);

    std::cout << "Audio system ready" << std::endl;
}

// ============================================================================
// cleanupAudio() - Release all audio resources
// ============================================================================
void cleanupAudio()
{
    // Stop all sources
    alSourceStop(musicSource);
    alSourceStop(sfxSource);

    // Delete sources
    alDeleteSources(1, &musicSource);
    alDeleteSources(1, &sfxSource);

    // Delete buffers
    if (musicBuffer)
        alDeleteBuffers(1, &musicBuffer);
    if (paddleHitBuffer)
        alDeleteBuffers(1, &paddleHitBuffer);
    if (wallHitBuffer)
        alDeleteBuffers(1, &wallHitBuffer);
    if (scoreBuffer)
        alDeleteBuffers(1, &scoreBuffer);
    if (winBuffer)
        alDeleteBuffers(1, &winBuffer);

    // Destroy context and close device
    alcMakeContextCurrent(nullptr);
    if (context)
        alcDestroyContext(context);
    if (device)
        alcCloseDevice(device);

    std::cout << "Audio system cleaned up" << std::endl;
}

// ============================================================================
// Background music controls
// ============================================================================

void playBackgroundMusic()
{
    if (musicBuffer)
    {
        alSourcePlay(musicSource);
    }
}

void stopBackgroundMusic()
{
    alSourceStop(musicSource);
}

void pauseBackgroundMusic()
{
    alSourcePause(musicSource);
}

void resumeBackgroundMusic()
{
    ALint state;
    alGetSourcei(musicSource, AL_SOURCE_STATE, &state);
    if (state == AL_PAUSED)
    {
        alSourcePlay(musicSource);
    }
}

// ============================================================================
// Sound effects - Play different sounds for game events
// ============================================================================

void playPaddleHitSound()
{
    if (paddleHitBuffer)
    {
        alSourceStop(sfxSource); // Stop previous sound
        alSourcei(sfxSource, AL_BUFFER, paddleHitBuffer);
        alSourcePlay(sfxSource);
    }
}

void playWallHitSound()
{
    if (wallHitBuffer)
    {
        alSourceStop(sfxSource);
        alSourcei(sfxSource, AL_BUFFER, wallHitBuffer);
        alSourcePlay(sfxSource);
    }
}

void playScoreSound()
{
    if (scoreBuffer)
    {
        alSourceStop(sfxSource);
        alSourcei(sfxSource, AL_BUFFER, scoreBuffer);
        alSourcePlay(sfxSource);
    }
}

void playWinSound()
{
    if (winBuffer)
    {
        alSourceStop(sfxSource);
        alSourcei(sfxSource, AL_BUFFER, winBuffer);
        alSourcePlay(sfxSource);
    }
}

// ============================================================================
// Volume controls
// ============================================================================

void setMusicVolume(float volume)
{
    musicVolume = volume;
    alSourcef(musicSource, AL_GAIN, musicVolume);
}

void setSoundVolume(float volume)
{
    soundVolume = volume;
    alSourcef(sfxSource, AL_GAIN, soundVolume);
}
