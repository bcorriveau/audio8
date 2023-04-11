/*
  File audio8.c

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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>

#include "audio8.h"

#define DEBUG 0

#define FREQUENCY (44100 / 2)
#define SAMPLES (512)
#define CHANNELS (1)
#define AMPLITUDE (30)      /* Volume */
#define SILENCE (128)

typedef enum {FALSE=0, TRUE=1} BOOL;


typedef struct tone_ {
    int hz;             /* hz of tone being generate */
    int wavelen;        /* wave length of the tone */
    struct {
        Effect type;
        unsigned int level;   /* effect level */
        int c;                /* current effect level */
        int low;              /* low for bounce */
        int inc;              /* bounce inc */
    } effect;
    int volume;
    int duration;       /* duration in ms */
    unsigned int si;    /* current sample index for square wave */
} tone_t;


#define SCALENOTES          7       /* All Notes in an Octave minus the last one */
#define ACCIDENTAL_FLAT     0
#define ACCIDENTAL_NATURAL  1
#define ACCIDENTAL_SHARP    2
#define ACCIDENTALS         3

/* All the notes in one octave minus the last one. */
typedef struct octave_s {
    int note[ACCIDENTALS][SCALENOTES];
} octave_t;

#define OCTAVE_MIN  0
#define OCTAVE_MAX  3
#define OCTAVES     4
/* First Octave contains middle C = 262Hz 
 *                         Notes - A    B    C    D    E    F    G                       */
octave_t octave[OCTAVES] = {
                              {{{  52,  58,  62,  69,  78,  82,  93 },      /* flats       */
                                {  55,  62,  65,  73,  82,  87,  98 },      /* naturals    */
                                {  58,  65,  69,  78,  87,  93, 104 }}},    /* sharps      */

                              {{{ 104, 117, 123, 139, 156, 165, 185 },      /* flats       */
                                { 110, 123, 131, 147, 165, 175, 196 },      /* naturals    */
                                { 117, 131, 139, 156, 175, 185, 208 }}},    /* sharps      */

                              {{{ 208, 233, 247, 277, 311, 330, 370 },      /* flats       */
                                { 220, 247, 262, 293, 330, 349, 392 },      /* naturals    */
                                { 233, 262, 277, 311, 349, 370, 415 }}},    /* sharps      */

                              {{{ 415, 466, 494, 554, 622, 659, 740 },      /* flats       */
                                { 440, 494, 523, 587, 659, 698, 784 },      /* naturals    */
                                { 466, 523, 554, 622, 698, 740, 831 }}}     /* sharps      */
};


/* Audio specification obtained */
static SDL_AudioSpec *obtained;

/* user tone data */
static tone_t user_tone[4];


/* gen_tone_val - generates a value for a tone for a sample index
 *
 * Params:
 *   si:        sample index
 *   wavelen:   wavelength
 *   volume:    volume
 *   level:    effect level
 */
int gen_tone_val(int si, int wavelen, int volume, int effect_level)
{
    int val;

    if ((si % wavelen) < (wavelen / 2) ) {
        val = volume;
    }
    else {
        val = 0 - volume;
    }

    /* check if effect should be applied - if si / level is odd effect is on */
    if (effect_level && (si / effect_level % 2))
        val = 0;

    return val;
}


/* create_tone_sq - creates a tone with a square wave
 *
 * Params:
 *  user_tone   - tones to create a wave. hz is zero for silence.
 *  stream      - points to where to put samples
 *  len         - length of stream in bytes
 */
void create_tone_sq(tone_t *user_tone, Uint8 *stream, int len ) 
{
    int i, v, samples;
    Uint8 *datap, value;

    datap = (Uint8 *) stream;
    samples = len / sizeof(value) / CHANNELS;

    /* first fill with silence */
    memset(datap, obtained->silence, len);

    if (DEBUG) {
        printf("\n***Filling %d bytes (%d samples) from sample %d for %d hz  effect %d  wavelen %d\n",
               len, samples, user_tone[0].si,
               user_tone[0].hz,
               user_tone[0].effect.level,
               user_tone[0].wavelen);
    }

    for(i=0; i < samples; i++) {
        for (v=0; v < 4; v++) {
            if (user_tone[v].hz == 0)
                continue;
            *datap += gen_tone_val(user_tone[v].si,
                                user_tone[v].wavelen,
                                user_tone[v].volume,
                                user_tone[v].effect.c);
            user_tone[v].si++; /* let si wrap */
            switch (user_tone[v].effect.type) {
            case NONE:
            case FIXED:
                break;
            case BOUNCE:
                if (user_tone[v].si % user_tone[v].wavelen == 0) {
                    user_tone[v].effect.c += user_tone[v].effect.inc;
                    if (user_tone[v].effect.c < user_tone[v].effect.low) {
                        user_tone[v].effect.c += 2;
                        user_tone[v].effect.inc = 1;
                    } else if (user_tone[v].effect.c > user_tone[v].effect.level) {
                        user_tone[v].effect.c -= 2;
                        user_tone[v].effect.inc = -1;
                    }
                }
                break;
            case UP:
                if (user_tone[v].si % user_tone[v].wavelen == 0) {
                    user_tone[v].effect.c--;
                    if (user_tone[v].effect.c <= 1)
                        user_tone[v].effect.c = user_tone[v].effect.level;
                }
                break;
            case DOWN:
                if (user_tone[v].si % user_tone[v].wavelen == 0) {
                    user_tone[v].effect.c++;
                    if (user_tone[v].effect.c >= user_tone[v].effect.level)
                        user_tone[v].effect.c = 1;
                }
                break;
            }
        }
        if (DEBUG) {
            if (i < 64) {
                printf("%d ",*datap);
            }
            if (i == 64) {
                printf("%d\n", *datap);
            }
        }
        datap++;
    }
}


/* create_tone_cb - calls the selected tone generator
 *
 * Params:
 *  userdata    - pointer to a user tone pointer
 *  stream      - points to where to put samples
 *  len         - length of stream in bytes
 */
void create_tone_cb(void *userdata, Uint8 *stream, int len ) 
{
    /* get pointer to the user tone to play */
    create_tone_sq(user_tone, stream, len);
}


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
void play_tone(int voice, int hz, int duration, int volume, Effect effect, int effect_level, int effect_low)
{

    if (effect > 4 || volume > 10 || volume < 0 || voice < 0 || voice > 3 || duration < 0)
        return;

    user_tone[voice].hz = hz;
    if (hz == 0)
        user_tone[voice].wavelen = 0;
    else
        user_tone[voice].wavelen = (((FREQUENCY * 10)/ hz) + 5) / 10;

    user_tone[voice].effect.type = effect;
    if (effect) {
        user_tone[voice].effect.level = abs(effect_level) * 8; /* absolute value for safety - x 8 for final effect value */
        user_tone[voice].effect.low = abs(effect_low) * 8;
        user_tone[voice].effect.inc = 1;
    } else
        user_tone[voice].effect.level = 0;
    user_tone[voice].effect.c = user_tone[voice].effect.level;
    user_tone[voice].duration = duration;
    user_tone[voice].volume = volume * 6;
    user_tone[voice].si = 0;                       /* current sample index */

    if (DEBUG) printf("playing tone %d  time(ms) = %d , volume = %d\n", hz, duration, volume);

    /* this manually limits the tone length in increments of 50ms */
    SDL_PauseAudio(0);
    if (duration) {
        while (duration > 0) {
            SDL_Delay(50);
            duration -= 50;
        }
        user_tone[voice].hz = 0;
    }
}


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
 void play_notes(int increment, int volume, char *notes)
{
    int i, len, hz;
    int a;              /* note index */
    int acc;            /* note accidental */
    int oct;            /* octave */
    int duration;
    BOOL adjust;

    if (increment < 50)
        return;

    len = strlen(notes);

    i = 0;
    oct = 0;

    while (i < len)
    {
        a = SCALENOTES;                 /* default to max index + 1 */
        acc = ACCIDENTAL_NATURAL;       /* default to natural notes */
        duration = increment;

        /* check for octave adjust and perform if needed */
        adjust = TRUE;
        switch (notes[i]) {
            case '^':
                oct++;
                if (oct > OCTAVE_MAX) {
                    oct = OCTAVE_MAX;
                }
                break;
            case 'v':
                oct--;
                if (oct < OCTAVE_MIN) {
                    oct = OCTAVE_MIN;
                }
                break;
            case '1':
            case '2':
            case '3':
            case '4':
                oct = notes[i] - '1';
                break;
            default:
                adjust = FALSE;
                break;
        }
        if (adjust) {
            i++;
            continue;
        }

        switch (notes[i]) {
            case 'A': 
            case 'B': 
            case 'C': 
            case 'D': 
            case 'E': 
            case 'F': 
            case 'G': 
                a = notes[i] - 'A'; 
                break; 

            case ' ': 
                hz = 0;         /* silence for a space */
                break;

            default:
                break;
        }
        i++;

        /* adjust for flats or sharps */
        switch (notes[i]) {
            case 'b':
                acc--;
                i++;
                break;

            case '#':
                acc++;
                i++;
                break;

            default :
                break;
        }

        /* set hz if not silence */
        if (a <  SCALENOTES) {
            hz = octave[oct].note[acc][a];
        }

        while (notes[i] == '-') {
            duration += increment;
            i++;
        }

        play_tone(0, hz, duration, volume, NONE, 0, 0);
        if (DEBUG) printf("play tone %c %d\n", notes[i-1], hz);
    }
}


/* audio_init - initialize audio */
void audio_init(void)
{ 
  /* Audio Setup */
  SDL_AudioSpec *desired;

  desired = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));
  obtained = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));

  desired->freq = FREQUENCY;
  desired->format = AUDIO_U8;
  desired->channels = CHANNELS;
  desired->samples = SAMPLES;
  desired->callback = create_tone_cb;
  desired->userdata = NULL;

  SDL_Init( SDL_INIT_AUDIO);

  /* Open the audio device */
  if ( SDL_OpenAudio(desired, obtained) < 0 ){
    fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
    exit(-1);
  }
  /* desired spec is no longer needed */
  if (desired->format != obtained->format) {
      printf("obtained format (%d) different than desired format (%d)\n"
              ,obtained->format,desired->format);
  }
  if (DEBUG) printf("freq %d  samples %d  channels %d  size %d  silence %d\n",
         obtained->freq, 
         obtained->samples,
         obtained->channels,
         obtained->size, 
         obtained->silence);
  free(desired);
  desired = NULL;

  /* clear out user tones */
  memset(user_tone, 0, sizeof(user_tone));

}


/* audio_close - Shut down audio */
void audio_close(void)
{
    SDL_PauseAudio(1);
    SDL_Quit();
    free(obtained);
}
