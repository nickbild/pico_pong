# Pico Pong

Play gesture-controlled Pong with a Raspberry Pi Pico!

<p align="center">
<img src="https://raw.githubusercontent.com/nickbild/pico_pong/main/media/pico_pong.gif">
</p>

## How It Works

A Raspberry Pi Pico generates a 640x480@60Hz monochrome VGA signal.  Due to memory constraints, the usable screen size is 640x350.  I would like to say that I chose monochrome because it has the proper retro look for Pong, but actually I just got tired of struggling to fit a cycle-perfect VGA generator into the tiny 32-instruction deep PIO instruction memory and had no will left to squeeze in color.

Data for the display is stored in a buffer in RAM, which the PIO program grabs at exactly the right nanosecond as it paints the screen via DMA.  This leaves the CPU free to do other work, like play a game, while the PIO handles the cycle-intensive process of generating a VGA signal.  The CPU simply needs to update the appropriate memory locations as needed to change the video output.

Fortunately the Raspberry Pi Pico is known for it's ability to be overclocked.  I had to run it at over 258 MHz to get everything working correctly—not bad for a board rated at 133 MHz.

The player paddle is controlled by two infrared (IR) LED / IR phototransistor pairs.  The IR LED shines upward onto a mirror (held in place by a QuadHands), which reflects the IR light back onto the receiver.  When that signal is interrupted (i.e. by a hand), it triggers the Pico via GPIO to move the player paddle.  One receiver moves the paddle up on the screen, and the other moves the paddle down.  I was surprised by how smooth and natural an interface this ended up being to Pong; much nicer than a potentiometer in my opinion.  The paddle on the left side of the screen is computer controlled.

## Media

YouTube: https://www.youtube.com/watch?v=2wJyQ80iF74

Playing Pong:
![Pong](https://raw.githubusercontent.com/nickbild/pico_pong/main/media/20210507_190239_sm.jpg)

The gesture controller:
![Controller](https://raw.githubusercontent.com/nickbild/pico_pong/main/media/20210507_190101_sm.jpg)

The Pico and VGA breakout board:
![Pico](https://raw.githubusercontent.com/nickbild/pico_pong/main/media/20210507_190134_sm.jpg)

## Bill of Materials

- 1 x Raspberry Pi Pico
- 2 x 940nm IR phototransistors
- 2 x 940nm IR LEDs
- 1 x VGA breakout adapter
- 3 x 100 ohm resistors
- 3 x 387 ohm resistors
- 2 x 10 ohm resistors
- 2 x Breadboards
- Small disc mirrors
- Miscellaneous wire

## About the Author

[Nick A. Bild, MS](https://nickbild79.firebaseapp.com/#!/)
