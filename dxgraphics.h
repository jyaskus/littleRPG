// Beginning Game Programming, 2nd Edition
// Chapter 8
// dxgraphics.h - Direct3D framework header file


#ifndef _DXGRAPHICS_H
#define _DXGRAPHICS_H

//function prototypes
int Init_Direct3D(HWND, int, int, int);
LPDIRECT3DSURFACE9 LoadSurface(char *, D3DCOLOR);
LPDIRECT3DTEXTURE9 LoadTexture(char *, D3DCOLOR);

//variable declarations
extern LPDIRECT3D9 d3d; 
extern LPDIRECT3DDEVICE9 d3ddev; 
extern LPDIRECT3DSURFACE9 backbuffer;

#endif

