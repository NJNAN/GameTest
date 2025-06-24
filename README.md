# GameTest

This repository contains a minimal graphical 2D RPG example using **SFML**. The player can explore multiple maps, fight monsters, collect items, and trade with NPCs. The game displays simple textures for each tile and entity.

## Requirements

- C++17 compatible compiler
- SFML 2.x development libraries (`libsfml-dev` on Debian/Ubuntu)

## Building

Compile the program with SFML:

```sh
g++ -std=c++17 main.cpp -lsfml-graphics -lsfml-window -lsfml-system -o game
```

## Running

Launch the game:

```sh
./game
```

Use the arrow keys to move around. Colliding with monsters will start a simple battle, stepping on `E` switches maps, and running into NPCs will automatically trade if you have enough gold.
