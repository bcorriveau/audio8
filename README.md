# audio8 - audio retro 8 bit four voice library

## Synopsis
Audio package that provides API for generating and playing four voice tones and notes in realtime.
Python and C API are supported.

The SDL2 library (e.g. libsdl2-2.0) must be installed for the C backend library to build.

Tones can be played with:
- no effect
- fixed effect
- cycle up effect
- cycle down effect
- bounce effect

If an effect set then an effect level must also be set, and a low level if the bounce effect is used.

There is support for playing notes from Ab1 (52hz) to G#5 (831hz) parsed from a string, including
holding notes and breaks.

Three demo prpgrams are included:
- note_demo.py: demonstrates playing different notes and tunes
- tone_demo.py: demononstrate playing different tones with different effects
- tone_tester.py: allows playing different tones with effects and changing effects on the fly

## API Reference

### Python3 API
```
audio_init - initialize audio

audio_close - shut down audio

class EffectType(Enum):
    NoEffect = 0        # no effect, straight tone
    Fixed = 1           # constant effect
    Up = 2              # effect cycles up to given effect value
    Down = 3            # effect cycles down to given effect value
    Bounce = 4          # effect cycles up and down in given range

play_tone(voice: int,
          tone: int,
          duration: int,
          volume: int,
          effect_type: EffectType,
          effect_level: int=0,
          effect_low: int=0)
    
    Play a tone with or without effects. Wrapper around C call.
    If duration is 0 then the tone will keep playing until the same
    voice is re-used or played with 0 tone.
    :param voice: 0-4
    :param tone: >0 for hz or 0 for silence
    :param duration: Duration in 50ms units, or 0 for forever
    :param volume: 0-10
    :param effect_type: effect type, enum
    :param effect_level: 0 - 16k
    :param effect_low: 0 - 16k
    :return: None


play_notes(inc: int, volume: int, notes: str)

    Play notes from a string with the given base time increment
    :param inc: base time increment in ms, minimum 100
    :param notes: string with notes to play
    :return: None
    :notes:  Notes in the form of a null terminated string.
             Notes are A-G, '-' to hold note, space for silence.
             Notes can be flatted with 'b' or sharped with '#'.
             Octave can be moved up and down with '^' or 'v'.
             Octave can also be set with a number 1-4.

```
### C api
```
typedef enum {
    NONE    =   0,
    FIXED   =   1,      /* fixed effect */
    UP      =   2,      /* cycle up     */
    DOWN    =   3,      /* cycle down   */
    BOUNCE  =   4       /* cycle down then down */
} Effect;

/* audio_init - initialize audio */
void audio_init(void);

/* audio_close - Shut down audio */
void audio_close(void);

/* play_tone -  plays a tone for a duration
 *
 * Params:
 *  voice       - voice for tone, 0-3
 *  hz          - hertz of the tone (integer)
 *  duration    - duration in ms. 50ms increments, or 0 for continuous
 *  volume      - volume (0-10)
 *  effect      - type of effect
 *  effect_level - value of effect
 *  effect_low   - low value of effect with bounce
 *
 * Notes:
 *  Duration in 50ms increments.
 *  To Stop a continuous tone play 0 hz with 0 volume.
 */
void play_tone(int voice, int hz, int duration, int volume, Effect effect, int effect_level, int effect_low);

/* play_notes - plays notes from the given string
 *
 * Params:
 *  increment:   time in ms (minimum 50ms increments)
 *  volume:      note volume (0-10)
 *  notes:       Notes in the form of a null terminated string.
 *               Notes are A-G, '-' to hold note, space for silence.
 *               Notes can be flatted with 'b' or sharped with '#'.
 *               Octave can be moved up and down with '^' or 'v'.
 *               Octave can also be set with a number 1-4.
 *
 */
 void play_notes(int increment, int volume, char *notes);

```
## Motivation
The benefits of this 8 bit audio library are:
  - Generate retro sounds in realtime, no wave files needed
  - Up to four voices at the same time
  - MIT License
## Code Example
```
audio_init();

print("Slow flying saucer")
play_tone(0, 200, 3000, 5, EffectType.Bounce, 2, 0)

print("Up from A and down from G to octaves up")
play_notes(200, 5, "^A- B- C- D- E- F- G- ^^G F E D C B A")

audio_close();
```
## License
MIT License
