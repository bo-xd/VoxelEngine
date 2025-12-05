#!/bin/sh

gcc main.c -lGL -lSDL3 -ldl -lm -lGLEW -lSDL3_image -lGLU -o main

./main
