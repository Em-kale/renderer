// hello.c
#include <stdio.h>
#include <windows.h> 

//callback called by the application when something happens
//CALLBACK macro evaluated to the same thing as WINAPI (__stdcall)
//But indicates that the operating system calls this, rather than the application
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Your message-handling code goes here
}

// h1Instance - essentially a pointer to this module (executable)  
// hprevInstance is always NULL on modern OS (awesome) , kept for backwards compatibility
// lpCmdLine - command line arguments (as unicode string) 
// nCmdShow - flag describing if the application is minimixed or maximized, etc
// WINAPI just expands to be "__stdcall" which is a calling convention - it specifies how arguments are pushed onto the stack
// and that the function is responsible for cleaning up the stack itself before return
// also causes the function name to be decorated to _WinMain@<number of bytes of parameters> 
// name change is mostly helpful for cpp applications where two functions can have the same name 
// 
// One could literally just write int __stdcall WinMain ... instead
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    char className[] = 'windowClass'
    WNDCLASS wc = { };
    wc.h1Instance = h1Instance;  //several programs might create a window class with the same name
                                 //need to provide the location so the OS knows which one we are referring to
    wc.lpfnWndProc = WndProc; //this is the function called when something happens (main message handling loop)

};

