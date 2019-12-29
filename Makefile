INPUT     = main.c image.c general_gl_inits.c
OUTPUT    = main.out
CC        = gcc
CFLAGS    = -g -Wall
LDFLAGS   = -framework GLUT -framework OpenGL -framework Cocoa 


build: 
	$(CC) $(CFLAGS) $(INPUT) -std=c11 -o $(OUTPUT) $(LDFLAGS) && rm -rf *.dSYM
	
run:
	./main.out

.PHONY: clean dist

clean:
	-rm $(OUTPUT) 
	


