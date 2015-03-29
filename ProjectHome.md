Uses six leds to control 6 outputs on an Arduino.  The code uses analog input 0 (port C0) as a common anode for the LEDs.  The cathodes are connected to pins 8-13 (portb 0-5) through dropping resistors (100-220Ω) and the outputs are at pins 2-7 (port d 2-7).

The sketch also accepts commands over the serial port:
  * 'n': all outputs ON
  * 'f': all outputs OFF
  * '0'-'5': toggle output N (e.g. output 2+n)