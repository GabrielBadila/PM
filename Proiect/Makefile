all: main.hex

main.elf: main.c lcd.c
	avr-gcc -std=c99 -Wall -Wextra -mmcu=atmega324a -DF_CPU=16000000 -Os -o $@ $^

main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex $^ $@
	avr-size $^

clean:
	rm -rf main.hex main.elf

.PHONY: all clean
