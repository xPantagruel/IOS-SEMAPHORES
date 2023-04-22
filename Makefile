# MAKEFILE
# Project: project 2, IPP 2023
# Author: Matej Macek, FIT
# Date: 17.04.2023

CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread -lrt 
SRC = proj2.c
EXEC = proj2

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

clean:
	rm -f $(EXEC) $(OBJ)