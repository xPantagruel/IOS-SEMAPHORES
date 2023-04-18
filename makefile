# MAKEFILE
# Project: project 2, IPP 2023
# Author: Matej Macek, FIT
# Date: 17.04.2023

CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread -lrt
SRC = proj2.c
OBJ = $(SRC:.c=.o)
EXEC = proj2

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)