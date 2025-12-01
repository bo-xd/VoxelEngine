#!/bin/sh

gcc main.c Engine/Window.c Engine/Voxel.c Engine/Camera.c -lGL -lSDL3 -ldl -lm -o main

./main
