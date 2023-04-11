"""
MIT License

Copyright (c) 2023 Brian Corriveau  https://github.com/bcorriveau/snackade

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
"""

import ctypes
import pathlib
from enum import Enum


class EffectType(Enum):
    NoEffect = 0        # no effect, straight tone
    Fixed = 1           # constant effect
    Up = 2              # effect cycles up to given effect value
    Down = 3            # effect cycles down to given effect value
    Bounce = 4          # effect cycles up and down in given range

# Audio C library interface
libname = pathlib.Path().absolute() / "libcaudio8.so"
c_lib = ctypes.CDLL(libname)


def audio_init():
    c_lib.audio_init()


def audio_close():
    c_lib.audio_close()


def play_tone(voice: int,
              tone: int,
              duration: int,
              volume: int,
              effect_type: EffectType,
              effect_level: int=0,
              effect_low: int=0):
    '''
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
    '''
    c_lib.play_tone(voice, tone, duration, volume, effect_type.value, effect_level, effect_low)


def play_notes(inc: int, volume: int, notes: str):
    '''
    Play notes from a string with the given base time increment
    :param inc: base time increment in ms, minimum 100
    :param notes: string with notes to play
    :return: None
    '''
    c_lib.play_notes.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_char_p]
    c_lib.play_notes(inc, volume, notes.encode('utf-8'))
