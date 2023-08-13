"""
Note playing Demo

Shows off the different features of the API
"""
from audio8 import  audio_init, audio_close, play_notes
from time import sleep


def main():
    audio_init()

    print("G scales")
    play_notes(200, 5, "G ^A B C D E F# G G ^A B C D E F# G G ^A B C D E F# G")
    sleep(0.5)

    print("Up and down the natural notes from C")
    play_notes(200, 5, "CDEFG^ABCCDEFG^ABC")
    play_notes(200, 5, "4CBA3GFEDCCBA2GFEDC")
    sleep(0.5)

    print("Up from A and down from G two octaves up")
    play_notes(200, 5, "^A- B- C- D- E- F- G- ^^G F E D C B A")
    sleep(0.5)

    print("Mary had a little lamb")
    play_notes(200, 5, "^^EDCDEEE-DDD-EEE-")
    play_notes(200, 5, "^^^EDCDEEECDCDEC-")
    sleep(0.5)

    print("Mary goes to Ireland")
    play_notes(50, 5, "^^E- CCC- D- C- D- E- EEE- E--")
    play_notes(50, 5, "^^C- E- D- DDD- D- C- D- E- EEE- E--")
    sleep(0.5)

    print("G and C octaves")
    play_notes(200, 5, "G^G^G^G")
    play_notes(200, 5, "C^C^C^CvCvCvC")

    audio_close()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        audio_close()
        pass

    print("\nFinished")
