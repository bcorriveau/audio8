/*
  File audio8.h 

  API for playing retro 8 bit audio


  MIT License

  Copyright (c) 2023 Brian Corriveau  https://github.com/bcorriveau/audio8

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
 
*/

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
