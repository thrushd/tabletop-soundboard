# Audio Conversion
Since the teensy requires a specific audio format (16 bit PCM, 44100 Hz WAV files), this simple GUI tool is provided as a convienience feature for batch converting audio files to the required format through FFMPEG.

## Instructions
1. Download, install, and verify FFPEG can be run from command line
2. Download and install python
3. Stage all files for conversion in a single folder
4. Double click the python script to run the GUI
5. Browse to the input directory, then browse to the directory you want the converted files to be in (this can be the same)
6. Convert!

If you just want to run the FFMPEG command without the GUI it's:
```
ffmpeg -i "example.mp3" -ar 44100 "example.wav"
```