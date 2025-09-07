#include "renderer.h"
#include <stdio.h> 
#include <stdint.h>
#include <stdlib.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){ char CLASS_NAME[] = "clientClass"; WNDCLASSEXA wc = { };
    printf("I am a client ^-^");
    system("renderer.exe");
    return 0;
}


