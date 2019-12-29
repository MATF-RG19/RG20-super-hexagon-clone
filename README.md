# RG20-super-hexagon-clone
Super-Hexagon Clone

This game is inspired by the famous [Super Hexagon](https://youtu.be/5mDjFdetU28?list=TLPQMTUxMTIwMTmZM8RhUJsiYg) game.
Goal in this game is to pass through as many hexagons as possible before you lose all off your lives.

## Building the game

### Prerequisites
- `glut` library for openGL

### Linux and OS X

In order to sucessfully build Linux version, go to branch `linux-build` before you actually try to build the game. Additionaly, you'll need GLUT library, if you haven't installed it already, since it usually doesn't come by default.

#### Installing GLUT (on most common distros)
`sudo apt-get install freeglut3-dev` on Ubuntu  
`sudo pacman -S freeglut` on Arch  
`yum install freeglut` on Fedora

- In order to build the code, you can just run: `make`  
- In order to run the code, you can just run `make run` or just directly run `./main.out`

### OS X

`GLUT` library should be available without any additional installations.

- In order to build the code, you can just run: `make`  
- In order to run the code, you can just run `make run` or just directly run `./main.out`

### Windows

Running the game hasn't been tested on Windows, and there are **no guarantees** that it will run with the current setup of the makefile (or the libraries).

## Controls

- `d` - Go right
- `a` - Go left
- `s` - Pause the game
- `m` - Change lightning model
- `Space` - Stop rotation
- `Escape` - Exit game

