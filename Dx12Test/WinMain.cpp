//////////////////////////////////////////////////////////////////////////
// Name:	WinMain.h
// Date:	April 2nd, 2010
// Author:	Kyle Lauing [klauing@devry.edu] or [kylelauing@gmail.com]
// Purpose: This file is used to create a simple window application for
//			use with the DirectXFramework developed for the GSP 381 course
//			for DeVry University.
// Disclaimer:	
//			Copyright © 2010 by DeVry Educational Development Corporation.
//			All rights reserved.  No part of this work may be reproduced 
//			or used in any form or by any means – graphic, electronic, or 
//			mechanical, including photocopying, recording, Web distribution 
//			or information storage and retrieval systems – without the 
//			prior consent of DeVry Educational Development Corporation.
//////////////////////////////////////////////////////////////////////////

#include "libraries.h"

using namespace std;

const int SCREEN_WIDTH    = 800;
const int SCREEN_HEIGHT   = 600;
const int SCREEN_BPP      = 32;

const int TOTAL_PARTICLES = 250;
const int SHIP_SPEED      = 5;

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define KEYDOWN(name,key) (name[key] & 0x80)

HWND                 g_hWnd       = NULL;
LPDIRECT3D9          g_pD3D       = NULL;
LPDIRECT3DDEVICE9    g_pd3dDevice = NULL;
LPDIRECTINPUT8       g_lpdi       = NULL;
LPDIRECTINPUTDEVICE8 g_pKeyboard  = NULL;

RECT  g_rcWindow;
RECT  g_rcViewport;
RECT  g_rcScreen;
BOOL  g_bWindowed       = true;
BOOL  g_bActive         = false;

DWORD LAST_FIRE_TICK	= 0;
int   SCORE_BOARD       = 0;
int   HEALTH_BAR		= 9;

double ELAPSED_TIME;
double CURRENT_TIME;
double END_TIME;
double ANIMATION_TIMER = 0.0;

struct PARTICLE
{
    int  x;
    int  y;
    int  nVelocity;
    bool bVisible;
    COLORREF color; 
};

int RandomInt( int a_Low, int a_High )
{
    int Range = a_High - a_Low;
    int Num = rand() % Range;
    return( Num + a_Low );
}

PARTICLE g_exhaust[TOTAL_PARTICLES];


typedef list<CSprite> SPRITELIST;

SPRITELIST				SPRITE_LIST;
SPRITELIST::iterator	SPRITE_A;
SPRITELIST::iterator	SPRITE_B;
SPRITELIST::iterator	SPRITE_C;

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HRESULT WinInit(HINSTANCE hInst, int nCmdShow, HWND* phWnd, HACCEL* phAccel);
HRESULT GameStart(HWND hWnd);
HRESULT GameMain(HWND hWnd);

HRESULT InitDirect3D(HWND hWnd, BOOL bWindowed);
void    FreeDirect3D(void);
HRESULT InitDirectInput(HWND hWnd);
void    FreeDirectInput(void);
void    InitializeSprites(void);

void    MoveShip(void);
void    MoveSprites(void);
void    CheckCollisions(void);
bool    SpriteCollisionTest(CSprite& sprite1, CSprite& sprite2);
bool    CollisionTest(int x1, int y1, int w1, int h1, 
                      int x2, int y2, int w2, int h2);
void    ShipScore(void);
HRESULT DisplayFrame(void);
HRESULT ShipHealthBar(void);
int     RandomInt(int a_Low, int a_High);


int APIENTRY WinMain( HINSTANCE hInst, 
                      HINSTANCE hPrevInst, 
                      LPSTR     pCmdLine, 
                      int       nCmdShow )
{
    MSG    msg;
    HWND   hWnd;
    HACCEL hAccel;

    memset(&msg,0,sizeof(msg));
    
    WinInit( hInst, nCmdShow, &hWnd, &hAccel );

    GameStart( hWnd );

    while( TRUE )
    {

        if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( 0 == GetMessage(&msg, NULL, 0, 0 ) )
            {

                return (int)msg.wParam;
            }

  
            if( 0 == TranslateAccelerator( hWnd, hAccel, &msg ) )
            {
                TranslateMessage( &msg ); 
                DispatchMessage( &msg );
            }
        }
        else
        {
            if( g_bActive )
            {


                CURRENT_TIME     = timeGetTime();
                ELAPSED_TIME = ((CURRENT_TIME - END_TIME) * 0.001);
                END_TIME    = CURRENT_TIME;

                if( FAILED( GameMain( hWnd ) ) )
                {


                    MessageBox( hWnd, TEXT("GameMain() failed. ")
                        TEXT("The game will now exit. "), TEXT("PrototypeX"), 
                        MB_ICONERROR | MB_OK );
                    return FALSE;
                }
            }
            else
            {

                WaitMessage();


            }
        }
    }
}


HRESULT WinInit( HINSTANCE hInst, int nCmdShow, HWND *phWnd, HACCEL *phAccel )
{
    WNDCLASSEX wc;
    HWND       hWnd;
    HACCEL     hAccel;

    wc.cbSize        = sizeof(wc);
    wc.lpszClassName = TEXT("MY_WINDOWS_CLASS");
    wc.lpfnWndProc   = MainWndProc;
    wc.style         = CS_VREDRAW | CS_HREDRAW;
    wc.hInstance     = hInst;
    wc.hIcon         = LoadIcon( hInst, MAKEINTRESOURCE(IDI_MAIN_ICON) );
    wc.hIconSm       = LoadIcon( hInst, MAKEINTRESOURCE(IDI_MAIN_ICON) );
    wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU);
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;

    if( RegisterClassEx( &wc ) == 0 )
        return E_FAIL;


    hAccel = LoadAccelerators( hInst, MAKEINTRESOURCE(IDR_MAIN_ACCEL) );


    DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
    DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
    DWORD dwMenuHeight    = GetSystemMetrics( SM_CYMENU );
    DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );
    DWORD dwWindowWidth   = SCREEN_WIDTH  + dwFrameWidth * 2;
    DWORD dwWindowHeight  = SCREEN_HEIGHT + dwFrameHeight * 2 + 
                            dwMenuHeight + dwCaptionHeight;


    DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX;
    hWnd = CreateWindowEx( 0, TEXT("MY_WINDOWS_CLASS"), 
                           TEXT("Space Asteroid Shooter"),
                           dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
                           dwWindowWidth, dwWindowHeight, NULL, NULL, hInst, NULL );
    if( hWnd == NULL )
        return E_FAIL;

    ShowWindow( hWnd, nCmdShow );
    UpdateWindow( hWnd );

    GetWindowRect( hWnd, &g_rcWindow );

    *phWnd   = hWnd;
    *phAccel = hAccel;

    return S_OK;
}


LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDM_TOGGLEFULLSCREEN:
                    return 0L;

                case IDM_EXIT:

                    PostMessage( hWnd, WM_CLOSE, 0, 0 );
                    return 0L;
            }
            break;

        case WM_GETMINMAXINFO:
            {

                MINMAXINFO* pMinMax = (MINMAXINFO*) lParam;

                DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
                DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
                DWORD dwMenuHeight    = GetSystemMetrics( SM_CYMENU );
                DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );

                pMinMax->ptMinTrackSize.x = SCREEN_WIDTH  + dwFrameWidth * 2;
                pMinMax->ptMinTrackSize.y = SCREEN_HEIGHT + dwFrameHeight * 2 + 
                                            dwMenuHeight + dwCaptionHeight;

                pMinMax->ptMaxTrackSize.x = pMinMax->ptMinTrackSize.x;
                pMinMax->ptMaxTrackSize.y = pMinMax->ptMinTrackSize.y;
            }
            return 0L;

        case WM_SIZE:

            if( SIZE_MAXHIDE == wParam || SIZE_MINIMIZED == wParam )
                g_bActive = FALSE;
            else
                g_bActive = TRUE;
            break;

        case WM_SETCURSOR:

            if( !g_bWindowed )
            {
                SetCursor( NULL );
                return TRUE;
            }
            break;

        case WM_EXITMENULOOP:

            break;

        case WM_EXITSIZEMOVE:

            break;

        case WM_SYSCOMMAND:

            switch( wParam )
            {
                case SC_MOVE:
                case SC_SIZE:
                case SC_MAXIMIZE:
                case SC_MONITORPOWER:
                    if( !g_bWindowed )
                        return TRUE;
            }
            break;
            
        case WM_ACTIVATE:
            if( WA_INACTIVE != wParam && g_pKeyboard )
            {

                g_pKeyboard->Acquire();
            }

            break;

        case WM_DESTROY:

            PostQuitMessage( 0 );
            return 0L;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}


HRESULT GameStart( HWND hWnd )
{
    HRESULT hr;


    if( FAILED( hr = InitDirect3D( hWnd, g_bWindowed ) ) )
        return hr;

    if( FAILED( hr = InitDirectInput( hWnd ) ) )
        return hr;

    InitializeSprites();


    LAST_FIRE_TICK = timeGetTime();

    return S_OK;
}

HRESULT GameMain( HWND hWnd )
{
    ANIMATION_TIMER += ELAPSED_TIME;

    if( ANIMATION_TIMER >= 0.016 )
        ANIMATION_TIMER = 0.0;
    else
        return S_OK;

    MoveShip();

    MoveSprites();

    CheckCollisions();

    ShipScore();

    DisplayFrame();

    return S_OK;
}


HRESULT InitDirect3D( HWND hWnd, BOOL bWindowed )
{
    g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

    D3DCAPS9 d3dCaps;
    g_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps );

    D3DDISPLAYMODE d3ddm;
    g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );

    if( bWindowed == TRUE )
    {
        d3dpp.Windowed         = TRUE;
        d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
        d3dpp.BackBufferFormat = d3ddm.Format;
    }
    else
    {
        d3dpp.Windowed         = FALSE;
        d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
        d3dpp.BackBufferWidth  = SCREEN_WIDTH;
        d3dpp.BackBufferHeight = SCREEN_HEIGHT;
        d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    }

    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

    d3dpp.Flags                  = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                          &d3dpp, &g_pd3dDevice );

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( 45.0f ), 
                                SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f );

    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    return S_OK;
}


void FreeDirect3D( void )
{
    SAFE_RELEASE( g_pd3dDevice );
    SAFE_RELEASE( g_pD3D );
}


HRESULT InitDirectInput( HWND hWnd )
{
    HRESULT hr;

    FreeDirectInput();


    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
        IID_IDirectInput8, (VOID**)&g_lpdi, NULL ) ) )
        return hr;

    if( FAILED( hr = g_lpdi->CreateDevice( GUID_SysKeyboard, &g_pKeyboard, NULL ) ) )
        return hr;

    if( FAILED( hr = g_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
        return hr;

    hr = g_pKeyboard->SetCooperativeLevel( hWnd, DISCL_NOWINKEY |  DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );

    if( FAILED(hr) )
        return hr;

    g_pKeyboard->Acquire();

    return S_OK;
}

void FreeDirectInput( void )
{

    if( g_pKeyboard ) 
	{
        g_pKeyboard->Unacquire();
	}

    SAFE_RELEASE( g_pKeyboard );
    SAFE_RELEASE( g_lpdi );
}

void InitializeSprites(void)
{
    CSprite sprite;
    int i = 0;

	//SHIP
    sprite.zeroSpriteValues();
    sprite.m_index					= 555;
    wcscpy_s( sprite.m_spriteType, L"ship" );
    wcscpy_s( sprite.m_textureName, L"spaceship.bmp" );
    sprite.m_spriteW				= 1152;
    sprite.m_spriteH				= 216;
    sprite.m_posX					= 0;
    sprite.m_posY				    = 250;
    sprite.m_frameAnimate			= false;
	sprite.m_fpsModify				= -2;

    sprite.m_CurrFrame				= 14;
    sprite.m_frameW					= 144;
    sprite.m_frameH					= 108;
    sprite.m_frameAcross			= 8;
    sprite.m_frameScaleW			= -10;
    sprite.m_frameScaleH			= -10;
    sprite.m_collideModify			= true;

    sprite.m_botCollision			= -50;
    sprite.m_topCollision			= -20;

    sprite.loadAnimationString(0, "7 7 6 6 5 5 4 4 3 3 2 2 1 1 0 0 9 9 10 10 11 11 12 12 13 13 14 14 15 15", 
        CSprite::MAINTAIN_LAST_FRAME );

    SPRITE_LIST.push_back(sprite);


	//STARS
	for( i = 0; i < 100; i++ )
    {
        sprite.zeroSpriteValues();
        wcscpy_s( sprite.m_spriteType, L"stars" );
        wcscpy_s( sprite.m_textureName, L"misc.bmp" );
        sprite.m_spriteW             = 300;
        sprite.m_spriteH			 = 300;
        sprite.m_oneFrame			 = true;
        sprite.m_posX				 = (float)RandomInt( 0, 800 );
        sprite.m_posY				 = (float)RandomInt( 0, 560 );
        sprite.m_speedX				 = (float)-(RandomInt( 1, 2 ));
        sprite.m_offsetX			 = 50;
		sprite.m_offsetY			 = 10;
		sprite.m_frameW				 = 16;
		sprite.m_frameH				 = 32;

        SPRITE_LIST.push_back(sprite);
    }

	//METEORS
	for( i = 0; i < 10; i++ )
    {
        sprite.zeroSpriteValues();
        wcscpy_s( sprite.m_spriteType, L"meteor" );
        wcscpy_s( sprite.m_textureName, L"misc.bmp" );
        sprite.m_spriteW             = 300;
        sprite.m_spriteH			 = 300;
        sprite.m_oneFrame			 = true;
        sprite.m_posX				 = 800;
        sprite.m_posY				 = (float)RandomInt( 0, 560 );
        sprite.m_speedX				 = (float)-(RandomInt( 1, 10 ));
        sprite.m_frameW				 = 32;
        sprite.m_frameH				 = 32;
        sprite.m_offsetX			 = 6;
        sprite.m_offsetY			 = 1;

        SPRITE_LIST.push_back(sprite);
    }

	//LAZER BULLETS
    for( i = 0; i < 20; i++ )
    {
        sprite.zeroSpriteValues();
        wcscpy_s( sprite.m_spriteType, L"lazer" );
        wcscpy_s( sprite.m_textureName, L"misc.bmp" );
        sprite.m_spriteW             = 200;
        sprite.m_spriteH			 = 200;
        sprite.m_isActive            = false;
        sprite.m_oneFrame			 = true;
        sprite.m_offsetX			 = 32;
        sprite.m_offsetY			 = 0;
        sprite.m_frameW				 = 18;
        sprite.m_frameH				 = 3;

        SPRITE_LIST.push_back(sprite);
    }

	//EXPLOSION
    for( i = 0; i < 20; i++ )
    {
        sprite.zeroSpriteValues();
        wcscpy_s( sprite.m_spriteType, L"explosion" );
        wcscpy_s( sprite.m_textureName, L"explosion.bmp" );
        sprite.m_spriteW             = 294;
        sprite.m_spriteH			 = 72;
        sprite.m_isActive            = false;
        sprite.m_frameAcross		 = 7;
        sprite.m_frameW				 = 42;
        sprite.m_frameH				 = 36;
		sprite.m_fpsModify			 = -3;
        sprite.loadAnimation( 0, 0, 13, CSprite::GO_INACTIVE );

        SPRITE_LIST.push_back(sprite);
    }

	//RED HEALTH (FIRST)
    sprite.zeroSpriteValues();
    wcscpy_s( sprite.m_spriteType, L"redHP" );
    wcscpy_s( sprite.m_textureName, L"misc.bmp" );
    sprite.m_spriteW				= 200;
    sprite.m_spriteH				= 200;
    sprite.m_isActive				= false;
    sprite.m_oneFrame				= true;
    sprite.m_offsetX				= 51;
    sprite.m_offsetY				= 0;
    sprite.m_frameW					= 4;
    sprite.m_frameH					= 13;

    SPRITE_LIST.push_back(sprite);

	//YELLOW HEALTH (SECOND)
    sprite.zeroSpriteValues();
    wcscpy_s( sprite.m_spriteType, L"yellowHP" );
    wcscpy_s( sprite.m_textureName, L"misc.bmp" );
    sprite.m_spriteW				= 200;
    sprite.m_spriteH				= 200;
    sprite.m_isActive				= false;
    sprite.m_oneFrame				= true;
    sprite.m_offsetX				= 56;
    sprite.m_offsetY				= 0;
    sprite.m_frameW					= 4;
    sprite.m_frameH					= 13;
    
    SPRITE_LIST.push_back(sprite);

	//GREEN HEALTH (THIRD)
    sprite.zeroSpriteValues();
    wcscpy_s( sprite.m_spriteType, L"greenHP" );
    wcscpy_s( sprite.m_textureName, L"misc.bmp" );
    sprite.m_spriteW				= 200;
    sprite.m_spriteH				= 200;
    sprite.m_isActive				= false;
    sprite.m_oneFrame				= true;
    sprite.m_offsetX				= 61;
    sprite.m_offsetY				= 0;
    sprite.m_frameW					= 4;
    sprite.m_frameH					= 13;

    SPRITE_LIST.push_back(sprite);

	//SCORE NUMBER FONT
    for( i = 0; i < 4; i++ )
    {
        sprite.zeroSpriteValues();
        wcscpy_s( sprite.m_spriteType, L"numbers" );
        wcscpy_s( sprite.m_textureName, L"numbers.bmp" );
        sprite.m_spriteW             = 185;
        sprite.m_spriteH             = 27;
        sprite.m_State			     = i;
        sprite.m_posX				 = (float)80 + (i * 17);
        sprite.m_posY				 = 10;
        sprite.m_frameAnimate        = false;
        sprite.m_offsetX			 =  9;
        sprite.m_offsetY			 =  5;
        sprite.m_frameAcross         = 10;
        sprite.m_frameW				 = 17;
        sprite.m_frameH				 = 17;
        sprite.loadAnimationString( 0, "0 1 2 3 4 5 6 7 8 9", CSprite::LOOP_ANIMATION );

        SPRITE_LIST.push_back(sprite);
    }
}


void MoveShip(void)
{
    char buffer[256];
    int  posShipX = 0;
    int  posShipY = 0;

    g_pKeyboard->GetDeviceState(sizeof(buffer),(LPVOID)&buffer); 

    for( SPRITE_A = SPRITE_LIST.begin(); SPRITE_A != SPRITE_LIST.end(); ++SPRITE_A )
    {
        if( !lstrcmp(SPRITE_A->m_spriteType, L"ship") )
        {
            if( KEYDOWN(buffer, DIK_UP) )
            {
                if( SPRITE_A->m_posY > 0 )
                    SPRITE_A->m_posY -= SHIP_SPEED;

                if( SPRITE_A->m_CurrFrame < 28 )
                    SPRITE_A->incFrame();
            }
            else if( KEYDOWN(buffer, DIK_DOWN) )
            {
                if( SPRITE_A->m_posY < 520 )
                    SPRITE_A->m_posY += SHIP_SPEED;

                if( SPRITE_A->m_CurrFrame > 0 )
                    SPRITE_A->decFrame();
            }
            else
            {
                if( SPRITE_A->m_CurrFrame > 14)
                    SPRITE_A->decFrame();

                if( SPRITE_A->m_CurrFrame < 14)
                    SPRITE_A->incFrame();
            }

            if( KEYDOWN(buffer, DIK_SPACE) )
            {

                DWORD currentTick = timeGetTime();
                DWORD differentTick = currentTick - LAST_FIRE_TICK;
                int numShots = 0;

                if( differentTick >= 75 )
                {
                    for( SPRITE_B = SPRITE_LIST.begin(); SPRITE_B != SPRITE_LIST.end(); ++SPRITE_B )
                    {
                        if( !lstrcmp(SPRITE_B->m_spriteType, L"lazer") && SPRITE_B->m_isActive == false )
                        {
                            SPRITE_B->m_posX = ( SPRITE_A->m_posX + 120 );
                            SPRITE_B->m_posY = ( SPRITE_A->m_posY + 48 );
                            SPRITE_B->m_isActive = true;

                            if( numShots == 0 )
                                SPRITE_B->m_speedX = 20;

                            ++numShots;
                            if( numShots == 1 )
                                break;
                        }
                    }

                    LAST_FIRE_TICK = currentTick;
                }

            }
            break;
        }
    }
}


void MoveSprites()
{

    
    for( SPRITE_A = SPRITE_LIST.begin(); SPRITE_A != SPRITE_LIST.end(); ++SPRITE_A )
    {
        if( !lstrcmp(SPRITE_A->m_spriteType, L"lazer") && SPRITE_A->m_isActive == true )
        {
            if( SPRITE_A->m_posX < SCREEN_WIDTH )
            {
                SPRITE_A->m_posX += SPRITE_A->m_speedX;
                SPRITE_A->m_posY += SPRITE_A->m_speedY;
            }
            else
            {
                SPRITE_A->m_isActive = false;
                SPRITE_A->m_posX = 0;
                SPRITE_A->m_posY = 0;
                SPRITE_A->m_speedX = 0;
                SPRITE_A->m_speedY = 0;
            }
        }

        if( !lstrcmp(SPRITE_A->m_spriteType, L"meteor")  )
        {
            SPRITE_A->m_posX += SPRITE_A->m_speedX;
            SPRITE_A->m_posY += SPRITE_A->m_speedY;

            if( SPRITE_A->m_posX > 800 )
			{
                SPRITE_A->m_posX = 0;
			}

            if( SPRITE_A->m_posX < -64 )
            {
                SPRITE_A->m_posX = 800;
                SPRITE_A->m_posY = (float)RandomInt( 0, 560 );
                SPRITE_A->m_speedX = (float)-(RandomInt( 1, 10 ));
            }
        }

		if( !lstrcmp(SPRITE_A->m_spriteType, L"stars")  )
        {
            SPRITE_A->m_posX += SPRITE_A->m_speedX;
            SPRITE_A->m_posY += SPRITE_A->m_speedY;

            if( SPRITE_A->m_posX > 800 )
			{
                SPRITE_A->m_posX = 0;
			}

            if( SPRITE_A->m_posX < -68 )
            {
                SPRITE_A->m_posX = 800;
                SPRITE_A->m_posY = (float)RandomInt( 0, 560 );
                SPRITE_A->m_speedX = (float)-RandomInt( 1, 2 );
            }
        }
    }
}


void CheckCollisions(void)
{
    int nCount1 = 0;
    int nCount2 = 0;
    int size = SPRITE_LIST.size();

    for( SPRITE_A = SPRITE_LIST.begin(); SPRITE_A != SPRITE_LIST.end(); ++SPRITE_A )
    {
        ++nCount1;

        for( SPRITE_B = SPRITE_LIST.begin(); SPRITE_B != SPRITE_LIST.end(); ++SPRITE_B )
        {

            while( nCount2 != nCount1 )
            {
                ++ SPRITE_B;
                ++ nCount2;   
            }

            if( nCount2 == size )
                break;

            if( SPRITE_A->m_Collided == true &&
                SPRITE_B->m_Collided == true &&
                SPRITE_A->m_isActive  == true &&
                SPRITE_B->m_isActive  == true)
            {
                if( SpriteCollisionTest( *SPRITE_A, *SPRITE_B ) )
                {

                    if( !lstrcmp(SPRITE_A->m_spriteType, L"meteor") && !lstrcmp(SPRITE_B->m_spriteType, L"lazer") )
                    {
                        SCORE_BOARD += 5;

                        SPRITE_B->m_isActive = false;


                        for( SPRITE_C = SPRITE_LIST.begin(); SPRITE_C != SPRITE_LIST.end(); ++SPRITE_C )
                        {
                            if( !lstrcmp(SPRITE_C->m_spriteType, L"explosion") && SPRITE_C->m_isActive == false )
                            {

                                SPRITE_C->m_posX = (SPRITE_A->m_posX - 10);
                                SPRITE_C->m_posY = (SPRITE_A->m_posY - 20);
                                SPRITE_C->m_speedX = SPRITE_A->m_speedX;
                                SPRITE_C->m_isActive = true;


                                SPRITE_A->m_posX = -100;
                                SPRITE_A->m_posY = -100;
                                SPRITE_A->m_speedX = 0;
                                SPRITE_A->m_speedY = 0;
                                SPRITE_A->m_partofScript = false;
                                break;
                            }
                        }
                    }


                    /*if( !lstrcmp(SPRITE_A->m_spriteType, L"lazer") && !lstrcmp(SPRITE_B->m_spriteType, L"meteor") )
                    {
                        SCORE_BOARD += 5;

                        SPRITE_A->m_isActive = false;


                        for( SPRITE_C = SPRITE_LIST.begin(); SPRITE_C != SPRITE_LIST.end(); ++SPRITE_C )
                        {
                            if( !lstrcmp(SPRITE_C->m_spriteType, L"explosion") && SPRITE_C->m_isActive == false )
                            {
 
                                SPRITE_C->m_posX = (SPRITE_B->m_posX - 10);
                                SPRITE_C->m_posY = (SPRITE_B->m_posY - 20);
                                SPRITE_C->m_speedX = SPRITE_B->m_speedX;
                                SPRITE_C->m_isActive = true;


                                SPRITE_B->m_posX = -100;
                                SPRITE_B->m_posY = -100;
                                SPRITE_B->m_speedX = 0;
                                SPRITE_B->m_speedY = 0;
                                break;
                            }
                        }
                    }*/

                    if( !lstrcmp(SPRITE_A->m_spriteType, L"ship") && !lstrcmp(SPRITE_B->m_spriteType, L"meteor") )
                    {
                        if( HEALTH_BAR != 0 )
                            --HEALTH_BAR;

                        if( HEALTH_BAR == 0 )
                        {
							SCORE_BOARD = 0;
                            HEALTH_BAR = 9;
                        }

                        for( SPRITE_C = SPRITE_LIST.begin(); SPRITE_C != SPRITE_LIST.end(); ++SPRITE_C )
                        {
                            if( !lstrcmp(SPRITE_C->m_spriteType, L"explosion") && SPRITE_C->m_isActive == false )
                            {

                                SPRITE_C->m_posX = (SPRITE_B->m_posX - 10);
                                SPRITE_C->m_posY = (SPRITE_B->m_posY - 20);
                                SPRITE_C->m_speedX = SPRITE_B->m_speedX;
                                SPRITE_C->m_isActive = true;

                                SPRITE_B->m_posX = -100;
                                SPRITE_B->m_posY = -100;
                                SPRITE_B->m_speedX = 0;
                                SPRITE_B->m_speedY = 0;
                                break;
                            }
                        }
                    }
                }
            }
        }

        nCount2 = 0;
    }
}


bool SpriteCollisionTest( CSprite& sprite1, CSprite& sprite2 )
{


    bool bCollide = false;
    
    if( sprite1.m_collideModify == true && sprite2.m_collideModify == true )
    {
 
        bCollide = CollisionTest( (int)sprite1.m_posX  + sprite1.m_leftCollision, 
                                  (int)sprite1.m_posY  - sprite1.m_topCollision, 
                                  sprite1.m_frameW  + sprite1.m_frameScaleW  + sprite1.m_rightCollision, 
                                  sprite1.m_frameH + sprite1.m_frameScaleH + sprite1.m_botCollision, 
                                  (int)sprite2.m_posX  + sprite2.m_leftCollision,
                                  (int)sprite2.m_posY  - sprite2.m_topCollision,
                                  sprite2.m_frameW  + sprite2.m_frameScaleW  + sprite2.m_rightCollision, 
                                  sprite2.m_frameH + sprite2.m_frameScaleH + sprite2.m_botCollision);
    }
    else if( sprite1.m_collideModify == true && sprite2.m_collideModify == false )
    {
 
        bCollide = CollisionTest( (int)sprite1.m_posX  + sprite1.m_leftCollision,
                                  (int)sprite1.m_posY  - sprite1.m_topCollision,
                                  sprite1.m_frameW  + sprite1.m_frameScaleW  + sprite1.m_rightCollision,
                                  sprite1.m_frameH + sprite1.m_frameScaleH + sprite1.m_botCollision,
                                  (int)sprite2.m_posX,
                                  (int)sprite2.m_posY,
                                  sprite2.m_frameW,
                                  sprite2.m_frameH);
    }
    else if( sprite1.m_collideModify == false && sprite2.m_collideModify == true )
    {

        bCollide = CollisionTest( (int)sprite1.m_posX,
                                  (int)sprite1.m_posY,
                                  sprite1.m_frameW,
                                  sprite1.m_frameH,
                                  (int)sprite2.m_posX  + sprite2.m_leftCollision,
                                  (int)sprite2.m_posY  - sprite2.m_topCollision,
                                  sprite2.m_frameW  + sprite2.m_frameScaleW  + sprite2.m_rightCollision,
                                  sprite2.m_frameH + sprite2.m_frameScaleH + sprite2.m_botCollision);
    }
    else
    {

        bCollide = CollisionTest( (int)sprite1.m_posX,
                                  (int)sprite1.m_posY,
								  sprite1.m_frameW,
                                  sprite1.m_frameH,
                                  (int)sprite2.m_posX,
                                  (int)sprite2.m_posY,
								  sprite2.m_frameW, 
                                  sprite2.m_frameH);
    }

    return(bCollide);
}


bool CollisionTest(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) 
{

    int width1  = (w1>>1) - (w1>>3);
    int height1 = (h1>>1) - (h1>>3);

    int width2  = (w2>>1) - (w2>>3);
    int height2 = (h2>>1) - (h2>>3);


    int cx1 = x1 + width1;
    int cy1 = y1 + height1;

    int cx2 = x2 + width2;
    int cy2 = y2 + height2;


    int dx = abs(cx2 - cx1);
    int dy = abs(cy2 - cy1);


    if(dx < (width1+width2) && dy < (height1+height2))
	{
        return true;
	}
    else 
	{
        return false;
	}
}


void ShipScore(void)
{

	int number4 = SCORE_BOARD % 10; 
    int number3 = (SCORE_BOARD % 100)/10;
    int number2 = (SCORE_BOARD % 1000)/100;
    int number1 = (SCORE_BOARD % 10000)/1000;

    for( SPRITE_A = SPRITE_LIST.begin(); SPRITE_A != SPRITE_LIST.end(); ++SPRITE_A )
    {
        if( !lstrcmp(SPRITE_A->m_spriteType, L"numbers") )
        {
            if( SPRITE_A->m_State == 0 )
                SPRITE_A->m_CurrFrame = number1;

            if( SPRITE_A->m_State == 1 )
                SPRITE_A->m_CurrFrame = number2;

            if( SPRITE_A->m_State == 2 )
                SPRITE_A->m_CurrFrame = number3;

            if( SPRITE_A->m_State == 3 )
                SPRITE_A->m_CurrFrame = number4;
        }

    }
}


HRESULT DisplayFrame()
{
    HRESULT hr;

    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0 );

    g_pd3dDevice->BeginScene();

    ShipHealthBar();

    g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    for( SPRITE_A = SPRITE_LIST.begin(); SPRITE_A != SPRITE_LIST.end(); ++SPRITE_A )
	{
        SPRITE_A->drawSprite( g_pd3dDevice );
	}
    g_pd3dDevice->EndScene();

    if( FAILED( hr = g_pd3dDevice->Present( NULL, NULL, NULL, NULL ) ) )
	{
        return hr;
	}
    return S_OK;
}

HRESULT ShipHealthBar( void )
{

    int healthPosX	= 10;
    int healthPosY	= 10;
    int markSpace	= 5;
    int markCount   = 0;
    int marksRed    = 3;
    int marksYellow = 3;
    int marksGreen  = 3;
    int i			= 0;

    for( SPRITE_A = SPRITE_LIST.begin(); SPRITE_A != SPRITE_LIST.end(); ++SPRITE_A )
    {
        if( !lstrcmp(SPRITE_A->m_spriteType, L"HealthBar") )
        {
            SPRITE_A->m_isActive = true;

            SPRITE_A->m_posX = (float)healthPosX - 3;
            SPRITE_A->m_posY = (float)healthPosY - 2;
            SPRITE_A->drawSprite( g_pd3dDevice );

            SPRITE_A->m_isActive = false;
        }

        if( !lstrcmp(SPRITE_A->m_spriteType, L"redHP") )
        {
            SPRITE_A->m_isActive = true;

            if( HEALTH_BAR > 0 )
            {
                for( i = 0; i < marksRed; ++i )
                {
                    SPRITE_A->m_posX = (float)healthPosX;
                    SPRITE_A->m_posY = (float)healthPosY;
                    SPRITE_A->drawSprite( g_pd3dDevice );

                    healthPosX += markSpace;
                    ++ markCount;

                    if( markCount >= HEALTH_BAR )
                        break;
                }
            }

            SPRITE_A->m_isActive = false;
        }

        if( !lstrcmp(SPRITE_A->m_spriteType, L"yellowHP") )
        {

            SPRITE_A->m_isActive = true;

            if( HEALTH_BAR > marksYellow )
            {
                for( i = 0; i < marksYellow; ++i )
                {
                    SPRITE_A->m_posX = (float)healthPosX;
                    SPRITE_A->m_posY = (float)healthPosY;
                    SPRITE_A->drawSprite( g_pd3dDevice );

                    healthPosX += markSpace;
                    ++ markCount;

                    if( markCount >= HEALTH_BAR )
                        break;
                }
            }

            SPRITE_A->m_isActive = false;
        }

        if( !lstrcmp(SPRITE_A->m_spriteType, L"greenHP") )
        {
            SPRITE_A->m_isActive = true;

            if( HEALTH_BAR > marksGreen )
            {
                for( i = 0; i < marksGreen; ++i )
                {
                    SPRITE_A->m_posX = (float)healthPosX;
                    SPRITE_A->m_posY = (float)healthPosY;
                    SPRITE_A->drawSprite( g_pd3dDevice );

                    healthPosX += markSpace;
                    ++markCount;

                    if( markCount >= HEALTH_BAR )
                        break;
                }
            }

            SPRITE_A->m_isActive = false;
            break;
        }
    }

    return S_OK;
}

