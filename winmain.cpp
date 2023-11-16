// Beginning Game Programming, 2nd Edition
// Chapter 8
// winmain.cpp - Windows framework source code file

#include <d3d9.h>
#include <d3dx9.h>
#include <time.h>
#include <stdio.h>
#include "dxgraphics.h"
#include "dxaudio.h"
#include "dxinput.h"
#include "game.h"

//window event callback function
LRESULT WINAPI WinProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            //release the Direct3D objects
            if (d3ddev != NULL) d3ddev->Release();
            if (d3d != NULL) d3d->Release();

            //release sound objects (this is a class not a DX object)
            if (dsound != NULL) delete dsound;

            //release input objects
            if (dinput != NULL) dinput->Release();
            Kill_Keyboard();
            Kill_Mouse();

            //call the "front-end" shutdown function
            Game_End(hWnd);

            //tell Windows to kill this program
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


//helper function to set up the window properties
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    //create the window class structure
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX); 

    //fill the struct with info
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)WinProc;
    wc.cbClsExtra	 = 0;
    wc.cbWndExtra	 = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = APPTITLE;
    wc.hIconSm       = NULL;

    //set up the window with the class info
    return RegisterClassEx(&wc);
}


//entry point for a Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR     lpCmdLine,
                   int       nCmdShow)
{
	MSG msg;
    HWND hWnd;

	freopen("ErrorLog.txt","w", stdout);

	// register the class
	MyRegisterClass(hInstance);

    //set up the screen in windowed or fullscreen mode?
    DWORD style;
    if (FULLSCREEN)
        style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
    else
        style = WS_OVERLAPPED;

    //create a new window
    hWnd = CreateWindow(
       APPTITLE,              //window class
       APPTITLE,              //title bar
       style,                 //window style
       CW_USEDEFAULT,         //x position of window
       CW_USEDEFAULT,         //y position of window
       SCREEN_WIDTH,          //width of the window
       SCREEN_HEIGHT,         //height of the window
       NULL,                  //parent window
       NULL,                  //menu
       hInstance,             //application instance
       NULL);                 //window parameters

    //was there an error creating the window?
    if (!hWnd)
      return FALSE;

    //display the window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
	
    //initialize Direct3D
    if (!Init_Direct3D(hWnd, SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN))
    {
        MessageBox(hWnd, "Error initializing Direct3D", "Error", MB_OK);
        return 0;
	}

    //initialize DirectSound
    if (!Init_DirectSound(hWnd))
    {
        MessageBox(hWnd, "Error initializing DirectSound", "Error", MB_OK);
        return 0;
	}

    //initialize DirectInput
    if (!Init_DirectInput(hWnd))
    {
        MessageBox(hWnd, "Error initializing DirectInput", "Error", MB_OK);
        return 0;
    }
    
	//initialize the game
    if (!Game_Init(hWnd))
    {
        MessageBox(hWnd, "Error initializing the game", "Error", MB_OK);
        return 0;
    }

    // main message loop
    int done = 0;
	while (!done)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
	    {
            //look for quit message
            if (msg.message == WM_QUIT)
                done = 1;

            //decode and pass messages on to WndProc
		    TranslateMessage(&msg);
		    DispatchMessage(&msg);
	    }
        else
		{
            //process game loop (else prevents running after window is closed)
            Game_Run(hWnd);
		}
    }

	return msg.wParam;
}