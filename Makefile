INPUT    ?= main.c
OUTPUT   ?= main
CC        = gcc
CFLAGS    = -g -Wall
LDFLAGS   = -framework GLUT -framework OpenGL -framework Cocoa 


build: 
	$(CC) $(CFLAGS) $(INPUT) -std=c11 -o $(OUTPUT) $(LDFLAGS) && rm -rf *.dSYM
	
.PHONY: clean dist

clean:
	-rm $(OUTPUT) 
	


