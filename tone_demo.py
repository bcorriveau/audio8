"""
Tone Demo

Shows off the different types of effects and the sounds that can
be made, and playing more than one voice at the same time.

"""
from time import sleep
from audio8 import audio_init, audio_close, play_tone, EffectType


def main():

    print("Slow lasers")
    play_tone(0, 100, 3000, 3, EffectType.Down, 15)

    print("Alert")
    play_tone(0, 100, 3000, 3, EffectType.Up, 6)

    print("Stars falling")
    play_tone(0, 20, 3000, 3, EffectType.Down, 1)

    print("Boat")
    play_tone(0, 200, 3000, 5, EffectType.Fixed, 40)

    print("Motorcycle")
    play_tone(0, 200, 3000, 5, EffectType.Fixed, 20)

    print("Helicopter")
    play_tone(0, 60, 3000, 5, EffectType.Fixed, 6)

    print("Slow flying saucer")
    play_tone(0, 200, 3000, 5, EffectType.Bounce, 2, 0)

    print("Fast flying saucer")
    play_tone(0, 200, 3000, 5, EffectType.Bounce, 1, 0)

    print("Destruct")
    play_tone(0, 1000, 500, 5, EffectType.Down, 3)
    sleep(0.5)

    print("Galloping")
    play_tone(0, 10, 3000, 10, EffectType.Fixed, 700)

    print("Galloping through lasers with stars falling")
    play_tone(0, 10, 0, 10, EffectType.Fixed, 700)
    play_tone(1, 100, 0, 3, EffectType.Down, 5)
    play_tone(2, 20, 0, 2, EffectType.Down, 1)
    sleep(3)


if __name__ == "__main__":

    audio_init()

    try:
        main()
    except KeyboardInterrupt:
        pass

    audio_close()
    print("\nFinished\n")
