#!/bin/sh

gcc main.c Engine/Window.c Engine/Renderer.c Engine/Camera.c Engine/Shaderer.c -lGL -lSDL3 -ldl -lm -lGLEW -lSDL3_image -lGLU -o main

./main
