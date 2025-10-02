#include "renderer.h"
#include <stdio.h> 
#include <stdint.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h> // For nanosleep

int frame(){ 
    printf("in testfunctionPtr - clinet \n");
    DrawSquare(10, 10, 10, 10);
    return 0;
}

int begin(){ 
    printf("in begin - clinet \n");
    Run(frame);
    return 0;
}

