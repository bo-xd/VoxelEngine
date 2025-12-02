#!/bin/sh

gcc main.c Engine/Window.c Engine/Voxel.c Engine/Camera.c Engine/Shaderer.c -lGL -lSDL3 -ldl -lm -lGLEW -o main

./main
