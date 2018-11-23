## Cross-compilation
Install required packages  
- Debian (Ubuntu) `sudo apt-get install binutils-avr gcc-avr avr-libc avrdude`

Compile
- run `sudo make`



## Fuse bits
By default at90can128 uses internal oscillator (8MHz) and to configure external 
oscillator we have to configure fuse bits using command:
```
avrdude -p at90can128 -c usbasp -U lfuse:w:0x9F:m
```
