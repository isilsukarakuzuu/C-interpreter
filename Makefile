CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -g

AdvCalc:
		$(CC) $(CFLAGS) -o AdvCalc ./src/AdvCalc.c