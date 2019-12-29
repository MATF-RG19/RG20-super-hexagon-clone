INPUT     = main.c image.c general_gl_inits.c
OUTPUT    = main.out
CC        = gcc
CFLAGS  = -g -Wall -I/usr/X11R6/include -I/usr/pkg/include
LDFLAGS = -L/usr/X11R6/lib -L/usr/pkg/lib
LDLIBS  = -lglut -lGLU -lGL

build: 
	$(CC) $(CFLAGS) $(INPUT) -std=c11 -o $(OUTPUT) $(LDFLAGS) $(LDLIBS)
	
run:
	./main.out

.PHONY: clean dist

clean:
	-rm $(OUTPUT) 
	


