#include <stdio.h>
#include <stdint.h>
#include <windows.h> 

// ------------------Internal Renderer Stuff ------------------------
//
#define WM_DRAW_SQUARE (WM_APP + 420)

typedef struct{
    LONG size;
    void *pPixelData; 
    HBITMAP hDIB; 
    LONG lPixelDataSize;
    LONG lWidth;
    LONG lHeight;
} RendererData;

BOOL resized = FALSE;

BOOL paintBitmapInWindow(HWND hwnd, HDC hWinDc, HBITMAP hBitmap){
    HDC hMemDc = CreateCompatibleDC(hWinDc); 

    HBITMAP oldBitMap = (HBITMAP)SelectObject(hMemDc, hBitmap);
    DIBSECTION dib; 

    int bresult = GetObject(hBitmap, sizeof(dib), &dib);

    RECT rectWindow;
    GetWindowRect(hwnd, &rectWindow);

    int width = rectWindow.right - rectWindow.left;
    int height = rectWindow.bottom - rectWindow.top;

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

int drawSquare(HWND hwnd, int x, int y, int height, int width){
    //we are going to create a new bitmap which will be
    //copied to the window when paint is called after we call invalidateRect
    printf("I am the renderer !! :3 I was called by a client, I will process the draw request nownow UwU! Nya!");
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
 switch (uMsg){
        case WM_DRAW_SQUARE: 
        {
            drawSquare(hwnd);
        }
        case WM_PAINT:
            {
               PAINTSTRUCT ps; 

               HDC hdc = BeginPaint(hwnd, &ps);
               RendererData* pRendererData = (RendererData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

               HBITMAP hBitmap = pRendererData->hDIB;
               DIBSECTION dib; 

               int bresult = GetObject(hBitmap, sizeof(dib), &dib);

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


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){ char CLASS_NAME[] = "windowClass"; WNDCLASSEXA wc = { };
    wc.hInstance = hInstance;  //several programs might create a window class with the same name
    wc.cbSize = sizeof(WNDCLASSEXA);                  //need to provide the location so the OS knows which one we are referring to
    wc.lpfnWndProc = WndProc; //this is the function called when something happens (main message handling loop)
    wc.lpszClassName = CLASS_NAME; 

    RegisterClassEx(&wc); //passes address of wc to registerClass 
    
    HWND hwnd; //where the instance of the window we create lives
    hwnd = CreateWindowEx(
        0, //optional window styles
        CLASS_NAME,
        "My first window!", //name of the window
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

    ShowWindow(hwnd, nCmdShow);
    

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0){
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    }
    return 0;
}

// ----------------- CLIENT API ----------------
// create the window - provide className etc, returns the hwnd
// needs a custom message handle
int initWindow(){}

int SendDrawSquareMessage(int x, int y, int height, int width){}
