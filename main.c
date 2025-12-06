#include <SDL3/SDL_main.h>
#include "Engine/Window.h"

//Unity build;
#include "Engine/Window.c"
#include "Engine/Renderer.c"
#include "Engine/Camera.c"
#include "Engine/Shaderer.c"
#include "Engine/World/Block.c"
#include "Engine/World/Lighting.c"
#include "Engine/ui/text.c"
#include "Engine/Player/Player.c"

int main(int argc, char *argv[]) {
    CreateWindow("Engine", 1920, 1920);
    return 0;
}
