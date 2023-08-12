"""
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
"""

from audio8 import audio_init, audio_close, play_tone, EffectType

def read_int(prompt, default):
    try:
        val = int(input(f'{prompt} (default {default}): '))
    except ValueError:
        print(f'{default}')
        val = default
    except Exception as e:
        print(type(e).__name__,e)
        val = default
    return val


def main():

    audio_init()

    while (True):
        voice = read_int('Voice 0-3, or -1 to quit', 0)
        if voice == -1:
            break;
        tone = read_int('Tone', 0)
        volume = read_int('Volume', 5)
        effect_value = read_int('Effect type (none=0,fixed=1,up=2,down=3,bounce=4)', 0)
        effect_type = EffectType(effect_value)
        effect_low = 0
        if effect_type != EffectType.NoEffect:
            effect_level = read_int('Effect level', 1)
            if effect_type == EffectType.Bounce:
                effect_low = read_int('Effect level low', 0)
        else:
            effect_level = 0
        duration = read_int('Duration in 200ms increments, 0 for continuous', 200)

        if tone == 0 or effect_type == EffectType.NoEffect or duration != 0 :
            play_tone(voice, tone, duration, volume, effect_type,
                      effect_level=effect_level,
                      effect_low=effect_low)
        else:
            print('------------- Modify Effect Level ----------------')
            print('Enter 0 to stop modifying and stop tone')
            print('     -1 to stop modifying and leave tone playing')
            print('--------------------------------------------------')
            while True:
                play_tone(voice, tone, duration, volume, effect_type,
                          effect_level=effect_level,
                          effect_low=effect_low)
                if effect_type == EffectType.NoEffect:
                    break
                e = read_int(f"Current effect level {effect_level}, enter next level", effect_level-1)
                effect_level = e
                if effect_level == 0:
                    # stop tone and begin new prompt
                    play_tone(voice, 0, duration, volume, effect_type)
                    break
                elif effect_level == -1:
                    break


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass

    audio_close()
    print("\nFinished\n")
