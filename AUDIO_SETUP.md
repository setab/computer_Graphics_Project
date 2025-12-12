# Audio Setup Guide

## Required Audio Files

Place these WAV audio files in your project directory:

### 1. **background_music.wav**

- Background music that loops continuously
- Recommended: Calm, upbeat electronic music
- Duration: 30 seconds to 2 minutes (will loop)

### 2. **paddle_hit.wav**

- Short sound when ball hits paddle
- Recommended: "Bleep" or "Ping" sound
- Duration: 0.1-0.3 seconds

### 3. **wall_hit.wav**

- Sound when ball hits top/bottom walls
- Recommended: Softer "Blip" sound
- Duration: 0.1-0.2 seconds

### 4. **score.wav**

- Sound when a player scores
- Recommended: Sad "buzz" or "whoosh" sound
- Duration: 0.5-1 second

### 5. **win.wav**

- Victory sound when game ends
- Recommended: Triumphant fanfare or chime
- Duration: 1-3 seconds

## Where to Get Sounds

### Free Sound Resources:

1. **Freesound.org** - https://freesound.org/
2. **Zapsplat** - https://www.zapsplat.com/
3. **Pixabay** - https://pixabay.com/sound-effects/
4. **OpenGameArt** - https://opengameart.org/

### Converting to WAV Format

If you have MP3 or other formats, convert them using:

```bash
# Using ffmpeg (install via: brew install ffmpeg)
ffmpeg -i input.mp3 -acodec pcm_s16le -ar 44100 output.wav
```

Or use online converters like:

- https://online-audio-converter.com/
- https://convertio.co/mp3-wav/

## Quick Test

After placing the files, compile and run:

```bash
make clean
make
./pong_game
```

## Controls with Audio

- **SPACE** - Pause/Resume (pauses music too)
- **R** - Restart game (restarts music)
- **ESC** - Quit (cleanup audio)
- **M** - Toggle music on/off (future feature)

## Troubleshooting

If sounds don't play:

1. Check that WAV files are in the same directory as the executable
2. Verify files are 44100 Hz sample rate
3. Check terminal for "Loaded audio: filename.wav" messages
4. Make sure files are not corrupt (try playing them in QuickTime)

## Optional: Volume Adjustment

You can adjust volumes in the code:

- Music volume: 0.0 (silent) to 1.0 (full)
- Sound effects volume: 0.0 to 1.0

The default is:

- Music: 0.5 (50%)
- Sound effects: 0.7 (70%)
