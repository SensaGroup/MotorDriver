sudo make
avrdude -p at90can128 -c usbasp -e -U flash:w:sensa_motor_board.hex
