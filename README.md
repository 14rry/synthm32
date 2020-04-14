# STM32 Audio Synthesizer
Audio Synthesizer for STM32 Arm processor in c++

## Supported Features:
* MIDI input: parses standard MIDI signals over [UART](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter)
* LCD Display output over [SPI](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface)
* Wavetable synthesis engine
  * Six waveforms: sine, triangle, square, saw, and to randomly generated noise signals
  * Ampltidue envelope: attack, release, and sustain
  * Pitch bending
  * Low Frequency Oscillator (LFO) pitch modulation

## Hardware Description
Developped and tested on the [STM32f103rb development board](https://www.st.com/en/microcontrollers-microprocessors/stm32f103rb.html)
Additional hardware:
* 1.8'' TFT LCD Display
* Rotary encoer and push-buttons
* FTDI Breakout Board
* LM386 Audio Amplifier

[insert pictures here]

## Software Description
[insert pictures here]

## Useful Links

* [HAL User Manual](https://www.st.com/content/ccc/resource/technical/document/user_manual/2f/71/ba/b8/75/54/47/cf/DM00105879.pdf/files/DM00105879.pdf/jcr:content/translations/en.DM00105879.pdf)
* [STM HAL library for ST7735](https://github.com/afiskon/stm32-st7735)
* [RC DAC circuit example](https://www.allaboutcircuits.com/technical-articles/turn-your-pwm-into-a-dac/)
* [RC circuit cutoff frequency calculator](https://www.omnicalculator.com/physics/rc-circuit)
* [Rotary encoder tutorial](https://www.beyond-circuits.com/wordpress/tutorial/tutorial12/)
* [Another rotary encoder implementation](https://github.com/MaJerle/stm32f429/blob/master/00-STM32F429_LIBRARIES/tm_stm32f4_rotary_encoder.c)
* [Background on different synthesizer waveforms](https://synthesizer-design.fandom.com/wiki/Synth_Waveforms)
* [“Hairless” MIDI to Serial Bridge software](https://projectgus.github.io/hairless-midiserial/)
* [MIDI note conversion chart](http://tonalsoft.com/pub/news/pitch-bend.aspx)
* [Summary of MIDI messages](https://www.midi.org/specifications/item/table-1-summary-of-midi-message)
