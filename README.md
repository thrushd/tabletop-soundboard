# Tabletop Soundboard
## In progress!
A simple, compact, easy to use device to add music, ambiance, and sound effects to any tabletop RPG.

## How To
All data is given to the device through a micro SD card. Audio files, the config, and gif are all placed in the root directory with no sub folders.

### Listening
There are two audio outputs on the device, USB audio and a standard 3.5mm jack. For the USB audio, you can easily loop it back to play through your computer speakers, a connected bluetooth speaker, etc. Instructions for setting up this loopback can be found [here](https://www.addictivetips.com/windows-tips/output-mic-sound-to-speakers-windows-10/). 

### Scenes and Settings
Sounds are organized into 'scenes', with each scene containing up to 8 tracks (sound files). This organization is set with a configuration file, written in [TOML](https://toml.io/en/). There are three main settings, which can be set at three different levels: global, scene, and track level. Priority for settings is always given to lower levels, so a track level setting will always override a scene level setting, etc. If a setting is not given to a scene or track it will use the next highest defined setting.

| Setting | Description |
| --- | --- |
| `gain` | The volume level, decimal values from `0-1` |
| `loop` | Whether a track should start playing again when it reaches the end, `true` or `false` |
| `play` | Whether a track should start playing immediately when a scene is activated, `true` or `false` |
| `name` | Override the name of the scene or track filename, used in scene and track definitions |
| `filename` | Filename of sound, used in track definitions, include the extension |
| `tracks` | A list of filenames used to create a scene with minimal input, same conventions as `filename` |

A scene is defined by making a name after the keyword `scenes`, enclosed in brackets. So to make a new scene called `tavern` you would write: `[scenes.tavern]`. Under this definition you can add your scene level settings. After that you can define tracks. This follows the same convention, just the scene name followed by a number representing which track it should be. So to define track 3 in scene `tavern` you would write: `[tavern.3]`. Same as the scene level definitions, under this you put all of your track level settings.

### Fixed Tracks
Sometimes you might have a sound that is used in every scene. So instead of copy-pasting the definition of the track into each scene, you can define it as a special `fixed` track with the notation `[fixed.N]`, with `N` being the track number. This does however mean you cannot have a scene named `fixed`.

### Startup Animation
Gifs are fun, so when the device is powered on it will play a gif on the main display if one is present on the SD card. This name can be changed in the config file under the table `[gifs]` with the setting `startup`. If this behaviour is not desired it can be left out of the config and the device will boot without it.

### Config Example
```TOML
# Default Settings, required
[default]
gain = 0.5
loop = false
play = false

# Startup animation filename
[gifs]
startup = "startup.gif"

# Create fixed tracks that don't change depending on which scene you are in
# If track.n is declared in a scene it is ignored and overriden with the below items
[fixed.7]
filename = "dj_airhorn.wav"
name = "Nat 20"
gain = 1.0
[fixed.8]
filename = "sad_trombone.wav"
name = "Nat 1"
gain = 1.0

# Create a scene
[scenes.tavern]
name = "Old Ivey Tavern"  # rename the scene if desired, otherwise it will show as 'tavern'
[tavern.1]  # add a track
filename = "tavern_sounds_1.wav"  # specify the filename
name = "Tavern Sounds"  # change the displayed name if you want
play = true  # set if it should play on scene start
loop = true  # set if the track should loop or not
gain = 0.3  # set a non default volume gain
[tavern.2]  # add another track
filename = "piano_3.wav"
name = "Piano"
play = true
gain = 0.75
[tavern.3]  # this track is fine with all of the default settings, no need to override
filename = "bang.wav"

[scenes.camp]
gain = 1.0
[camp.1]
filename = "wolf_howl.wav"
[camp.2]
filename = "wind.wav"
gain = 0.2
loop = true
play = true
[camp.3]
filename = "crackling_campfire.wav"
gain = 0.75
loop = true
play = true

# If defalt settings are fine it can get very minimal, just a list of the file names
[scenes.mountain]
loop = true
tracks = ["mountain_ambiance.wav", "avalanch.wav", "yetti_growl.wav"]
```

## Audio Files
### Format
The audio is handled by the teensy audio library, which only supports a certain format (16 bit PCM, 44100 Hz WAV files). Since most online resources provide all kinds of formats a simple tool is provided to convert these to the proper WAV format. It can be found under `/audio-convert`.

### Sources
- [Tabletop Audio](https://tabletopaudio.com/) - Good for longer, ambient tracks
- [Battlebards](https://battlebards.com/#/tracks) - Tons of variety, music, soundscapes, effects, voiceovers, but it is paid
- [Ambient Mixer RPG Category](https://rpg.ambient-mixer.com/) - Ambiance, user curated collections of tracks, no easy download, but there are sources given for most tracks
- [Freesound](https://freesound.org/) - Free, huge variety
- [SoundBible](https://soundbible.com/) - Free, decent collection, WAV format
- [mixkit](https://mixkit.co/free-sound-effects/) - Free, decent collection, WAV format
- [pixabay](https://pixabay.com/sound-effects/) - Free, music and sound effects


## Feature Wishlist
- Scrolling text for longer file and scene names

## History
| Date | Firmware | PCBA | Case | 
| ---- | -------- | ---- | ---- |
| 2022-02-19 | MVP working on breadboard | | |
| 2022-03-29 |  | First prototypes ordered | First prototypes ordered |
| 2023-04-22 | Ported to PCBA | Assembled and working | |