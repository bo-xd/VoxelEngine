#!/bin/sh

gcc main.c Engine/Window.c -lGL -lSDL3 -ldl -lm -o main

./main
