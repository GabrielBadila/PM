/*
** main.c
**
** Gabriel Badila, 333 CA
** Proiect PM 2017
**
** SNAKE
**
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "lcd.h"

#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE_X      (LCD_X_RES)
#define BUFF_SIZE_Y      (LCD_Y_RES / 8)

#define	width	14
#define	height	6

#define	LEFT	1
#define	RIGHT	2
#define	DOWN	3
#define	UP		4
#define NONE	5


typedef struct coord {
	int x;
	int y;
} coord;


/* Global parameters */
static coord snake[width * height];
static coord food;
static int size;
static int foodNum;
static int foodValue;
static int score;
static int direction;
static int level;
static int singleLevel;


/* Init buttons */
void init_buttons(void) {

    DDRA = 0xff;
    PORTA = 0x00;

	/* Setez pinii ca intrare. */
	DDRA &= ~((1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3));

	/* Activez rezistenta de pull-up pentru pini. */
	PORTA |= ((1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3));
	
}


/* Init snake */
void init_snake() {
	int i;
	
	direction = LEFT;
	foodNum = 0;
	score = 0;
	size = 3;
	
	set_food();

	for (i = 0; i < size; ++i) {
		snake[i].y = 3;
		snake[i].x = 6 + i;
	}
	
}


/* Set random food */
void set_food() {
	int i;
	
	foodNum++;
	
	/* food or bonus */
	if (foodNum % 5 == 0)
		foodValue = 15;
	else
		foodValue = 5;
	
	food.x = rand() % 10 + 1;
	food.y = rand() % 5 + 1;
	
	for (i = 0; i < size; ++i) {
		if (snake[i].x == food.x && snake[i].y == food.y) {
			foodNum--;
			set_food();
		}
	}
	
	/* set food for level 2 */
	if (level == 2) {
		if (food.x == 1 || food.x == width || food.y == 1 || food.y == height) {
			foodNum--;
			set_food();
		}
	}
	
	/* set food for level 3 */
	if (level == 3) {
		for (i = 5; i <= 10; i++) {
			if (food.x == i && food.y == 2) {
				foodNum--;
				set_food();
			}
		}
		for (i = 3; i <= 5; i++) {
			if (food.x == i && food.y == 6) {
				foodNum--;
				set_food();
			}
		}
		for (i = 10; i <= 12; i++) {
			if (food.x == i && food.y == 6) {
				foodNum--;
				set_food();
			}
		}
	}
	
}


/* Check food collisions */
void check_collision() {
	int aux;
	
	if (snake[0].x == food.x && snake[0].y == food.y) {
		int x = snake[size - 1].x - snake[size - 2].x;
		int y = snake[size - 1].y - snake[size - 2].y;
		snake[size].x = snake[size - 1].x + x;
		snake[size].y = snake[size - 1].y + y;
		size++;
		
		score += foodValue;
		if (foodNum % 10 == 0 && foodNum > 0 && singleLevel == 0) {
			level++;
			aux = score;
			init_snake;
			score = aux;
		}
		else
			set_food();
	}
	
}


/* Take action for the end of the game */
void take_action() {
	int j;
	char finalScore[10];
	itoa(score, finalScore, 10);
	lcd_clear();
	lcd_goto_xy(3, 3);
	for (j = 0; j< size; j++) {
		snake[j].x = 0;
		snake[j].y = 0;
	}
	
	lcd_str("GAME OVER");
	lcd_goto_xy(7, 4);
	lcd_str(finalScore);

	init_snake();
	if (singleLevel == 0)
		level = 1;
	_delay_ms(5000);
	
}


/* Check for end of the game */
void end_game() {
	int i;
	
	for (i = 1; i < size; ++i) {
	
		/* collision with snake */
		if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
			take_action();
			break;
		}
		
		/* collision with grid for level 2 */
		if (level == 2) {
			if (snake[0].x == 1 || snake[0].x == width || snake[0].y == 1 || snake[0].y == height) {
				take_action();
				break;
			}	
		}
		
		/* collision with grid for level 3 */
		if (level == 3) {
			for (i = 5; i <= 10; i++) {
				if (snake[0].x == i && snake[0].y == 2) {
					take_action();
					break;
				}
			}
			for (i = 3; i <= 5; i++) {
				if (snake[0].x == i && snake[0].y == 6) {
					take_action();
					break;
				}
			}
			for (i = 10; i <= 12; i++) {
				if (snake[0].x == i && snake[0].y == 6) {
					take_action();
					break;
				}
			}
		}
	}
	
}


/* Get movement directions */
void get_direction(void) {

    // DOWN
    if (!(PINA & (1 << PA0)) && direction != UP && direction != DOWN)
        direction = DOWN;
	
    // RIGHT
    if (!(PINA & (1 << PA1)) && direction != LEFT && direction != RIGHT)
        direction = RIGHT;
		
    // LEFT
    if (!(PINA & (1 << PA2)) && direction != RIGHT && direction != LEFT)
        direction = LEFT;
    
    // UP
    if (!(PINA & (1 << PA3)) && direction != DOWN && direction != UP)
        direction = UP;
		
}


/* Move snake */
void move_snake() {
	int i;

	for (i = size - 1; i > 0; --i) {
		snake[i].x = snake[i - 1].x;
		snake[i].y = snake[i - 1].y;
	}

	if (direction == UP)
		snake[0].y = (snake[0].y - 1) ? (snake[0].y - 1) : height;
	else if (direction == DOWN)
		snake[0].y = snake[0].y % height + 1;
	else if (direction == LEFT)
		snake[0].x = (snake[0].x - 1 ) ? (snake[0].x - 1) : width;
	else if (direction == RIGHT)
		snake[0].x = snake[0].x % width + 1;

}


/* Draw game */
void draw() {
	int i;

	/* draw food or bonus */
	lcd_goto_xy(food.x, food.y);
	if (foodNum % 5 == 0)
		lcd_chr('O');
	else
		lcd_chr('x');

	for (i = 0; i < size; ++i) {
		lcd_goto_xy(snake[i].x, snake[i].y);
		lcd_chr('#');
	}
	
	/* draw grid for level 2 */
	if (level == 2) {
		lcd_goto_xy(1,1);
		lcd_str("______________");
		for (i = 2; i < height; i++) {
			lcd_goto_xy(1, i);
			lcd_chr('l');
			lcd_goto_xy(width, i);
			lcd_chr('l');
		}
		lcd_goto_xy(1,width);
		lcd_str("l____________l");
	}
	
	/* draw grid for level 3 */
	if (level == 3) {
		lcd_goto_xy(5,2);
		lcd_str("______");
		lcd_goto_xy(3,6);
		lcd_str("___");
		lcd_goto_xy(10,6);
		lcd_str("___");
	}
	
}
 
 
 /* Selection menu at the start of the game */
void select_level() {

    // LEFT - level 1
    if (!(PINA & (1 << PA2)) && level == 0) {
        level = 1;
		singleLevel = 1;
    }

	//DOWN - level 2
    if (!(PINA & (1 << PA0)) && level == 0) {
        level = 2;
		singleLevel = 1;
    }
	
    // RIGHT - level 3
    if (!(PINA & (1 << PA1)) && level == 0) {
        level = 3;
		singleLevel = 1;
	}

     // UP - normal
    if (!(PINA & (1 << PA3)) && level == 0)
        level = 1;
		
}


/* Init game parameters and start menu */
void init_game() {

	// Setup LCD
	lcd_init();
	lcd_contrast(0x40);

	lcd_goto_xy(5, 2);
	lcd_str("Badila");
	lcd_goto_xy(2, 3);
	lcd_str("Gabriel Alin");
	lcd_goto_xy(5, 4);
	lcd_str("333CA");
    _delay_ms(3000);
	
	lcd_clear();
	
	lcd_goto_xy(5, 3);
	lcd_str("SNAKE");
	_delay_ms(1500);
	
	lcd_clear();
	
	lcd_goto_xy(2, 3);
	lcd_str("Select level:");
	_delay_ms(1200);
	
	lcd_clear();

	lcd_goto_xy(1, 2);
	lcd_str("Normal   UP");
	lcd_goto_xy(1, 3);
	lcd_str("Level 1  LEFT");
	lcd_goto_xy(1, 4);
	lcd_str("Level 2  DOWN");
	lcd_goto_xy(1, 5);
	lcd_str("Level 3  RIGHT");
	
	_delay_ms(2000);

	init_buttons();
	init_snake();

	level = 0;
	singleLevel = 0;

	while (1) {
		select_level();
		_delay_ms(150);
		if (level > 0)
			break;
	}

	lcd_clear();
	
}


/* main -- Game starts here */
int main(void)
{	
	
	init_game();

	while(1) {
		draw();
		get_direction();
		_delay_ms(150);
		lcd_clear();
		move_snake();
		check_collision();
		end_game();
	}
	while(1);
	
}
