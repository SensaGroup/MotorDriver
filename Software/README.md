# Actuator Control Board

## Cross-compilation
Install required packages  
- Debian (Ubuntu) `sudo apt-get install binutils-avr gcc-avr avr-libc avrdude`
- Arch `sudo pacman -S avr-binutils avr-gcc avr-libc avrdude`

and run `sudo bash flash.sh` to flash microcontroller.

## Fuse bits
Set the fuses for external oscillator (16MHz), and disabled jtag (to use portf)
```
avrdude -p at90can128 -c usbasp -U lfuse:w:0xef:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m
```
