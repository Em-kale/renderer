#include <stdio.h>
#include <stdint.h>
#include <windows.h> 
#include "renderer.h"
#include <time.h> // For nanosleep

// ------------------Internal Renderer Stuff ------------------------
#define WM_DRAW_SQUARE (WM_APP + 420)
#define WM_INIT_WINDOW (WM_APP + 69)

typedef struct{
    LONG size;
    void *pPixelData; 
    HBITMAP hDIB; 
    LONG lPixelDataSize;
    LONG lWidth;
    LONG lHeight;
} RendererData;

BOOL resized = FALSE;
HWND g_hWnd = NULL;

BOOL paintBitmapInWindow(HWND hwnd, HDC hWinDc, HBITMAP hBitmap){
    HDC hMemDc = CreateCompatibleDC(hWinDc); 

    HBITMAP oldBitMap = (HBITMAP)SelectObject(hMemDc, hBitmap);
    DIBSECTION dib; 

    int bresult = GetObject(hBitmap, sizeof(dib), &dib);

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    BOOL result; 
    if(resized == FALSE){ 
        result = BitBlt(hWinDc, 0, 0, width, height, hMemDc, 0, 0, SRCCOPY);
    } 
    else{ 
        int sourceHeight = dib.dsBmih.biHeight;
        int sourceWidth = dib.dsBmih.biWidth;
        result = StretchBlt(hWinDc, 0, 0, width, height, hMemDc, 0, 0, sourceWidth, sourceHeight, SRCCOPY);
        resized = FALSE;
    } 

    SelectObject(hMemDc, oldBitMap);
    DeleteDC(hMemDc);
    return result;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
 switch (uMsg){
        case WM_DRAW_SQUARE: 
        {
            printf("Drawing square *-*");
            PAINTSTRUCT ps; 

            RendererData* pRendererData = (RendererData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

            HBITMAP hBitmap = pRendererData->hDIB;

            DIBSECTION dib; 
            int bresult = GetObject(hBitmap, sizeof(dib), &dib);
            uint32_t* pixelPtr = (uint32_t*)pRendererData->pPixelData;
            
            //add square location calc logic
            for (long i = 10; i < 100000; i=i+1) {
                pixelPtr[i] = 0xFFF222FF;
            }

            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
            return 0;
        }
        case WM_PAINT:
            {
               printf("painting");
               PAINTSTRUCT ps; 

               HDC hdc = BeginPaint(hwnd, &ps);
               RendererData* pRendererData = (RendererData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

               HBITMAP hBitmap = pRendererData->hDIB;
               DIBSECTION dib; 

               paintBitmapInWindow(hwnd, hdc, hBitmap);    

               EndPaint(hwnd, &ps);
               return 0;
        }
        case WM_SIZE: 
        {
                printf("resizing!");
                RECT rectWindow;
                GetWindowRect(hwnd, &rectWindow);
                resized = TRUE;
                InvalidateRect(hwnd, &rectWindow, TRUE);
                return 0;
        }
        case WM_CREATE:
        {

            printf("creating!");
            void* pPixelData = NULL;

            HDC hdc = GetDC(hwnd); 

            BITMAPINFO bitMapInfo;

            RECT rectWindow;
            GetWindowRect(hwnd, &rectWindow);
            int width = rectWindow.right - rectWindow.left;
            int height = rectWindow.bottom - rectWindow.top;

            bitMapInfo.bmiHeader.biSize = sizeof(bitMapInfo);
            bitMapInfo.bmiHeader.biWidth =  width;
            bitMapInfo.bmiHeader.biHeight = height;
            bitMapInfo.bmiHeader.biPlanes = 1; //always 1, thanks microsoft.. 
            bitMapInfo.bmiHeader.biBitCount = 32; //the number of bits used to represent a pixel's color
            bitMapInfo.bmiHeader.biCompression = BI_RGB; //uncompressed color values - don't yet know when you might want compressed
            
            HBITMAP hDIB = CreateDIBSection(hdc, &bitMapInfo, DIB_RGB_COLORS, &pPixelData, 0, 0);

            RendererData *rendererData = malloc(sizeof(RendererData));
            
            rendererData->pPixelData = pPixelData;
            rendererData->hDIB = hDIB;
            rendererData->lPixelDataSize = bitMapInfo.bmiHeader.biWidth * bitMapInfo.bmiHeader.biHeight * (bitMapInfo.bmiHeader.biBitCount / 4);
            rendererData->lHeight = bitMapInfo.bmiHeader.biHeight;
            rendererData->lWidth = bitMapInfo.bmiHeader.biWidth;
            rendererData->size = sizeof(RendererData);         


            uint32_t* pixelPtr = (uint32_t*)pPixelData;

            for (long i = 0; i < width*height; ++i) {
                pixelPtr[i] = 0xFF00FF00;
            }

            LONG_PTR windowUserDataPtr = SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)rendererData);

            ReleaseDC(hwnd,hdc);
            return 0;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        case WM_NCDESTROY: {
            RendererData* pRendererData = (RendererData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            free(pRendererData);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){ 
    char CLASS_NAME[] = "windowClass";
    WNDCLASSEXA wc = { };

    wc.hInstance = hInstance;  //several programs might create a window class with the same name
    wc.cbSize = sizeof(WNDCLASSEXA);                  //need to provide the location so the OS knows which one we are referring to
    wc.lpfnWndProc = WndProc; //this is the function called when something happens (main message handling loop)
    wc.lpszClassName = CLASS_NAME; 

    RegisterClassEx(&wc); //passes address of wc to registerClass 
    
    HWND hwnd; //where the instance of the window we create lives
    hwnd = CreateWindowEx(
        0, //optional window styles
        CLASS_NAME,
        "Rendered Window", //name of the window
        WS_OVERLAPPEDWINDOW, //window styles, others include things like WS_POPUP, etc
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, //size and position, in the order. x,y, width, height
        NULL, //parent window
        NULL, //menu - will dive deeper on this later I imagine
        hInstance, //so the OS knows which application owns this instance, important for lots of stuff!!  
        NULL //additional app data - not sure what might go here
    );
    
    //if that fails, crash out :3
    if (hwnd == NULL){
        return 0;
    };
    
    g_hWnd = hwnd;
    ShowWindow(hwnd, nCmdShow);

    //create clientMain stub that tells the compiler to look elsewhere for it
    extern int begin();
    begin();

    return 0;
}

// ----------------- CLIENT API ----------------
// create the window - provide className etc, returns the hwnd
// needs a custom message handle

int Run(int (*clientFrameCallback)()){
    MSG msg = { };
    printf("here in begin");
    while (GetMessage(&msg, NULL, 0, 0) > 0){
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
        clientFrameCallback();
    }

    return 0;
}

int DrawSquare(int x, int y, int height, int width){
    //if that fails, crash out :3
    if (g_hWnd == NULL){
        return 0;
    };

    WPARAM wParam = 0;
    LPARAM lParam = 0;

    PostMessage(g_hWnd, WM_APP + 420, wParam, lParam);
    Sleep(10);
    return 0;
}


