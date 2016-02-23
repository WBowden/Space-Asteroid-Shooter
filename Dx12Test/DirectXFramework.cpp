//////////////////////////////////////////////////////////////////////////
// Name:	DirectXFramework.cpp
// Date:	April 2nd, 2010
// Author:	Kyle Lauing [klauing@devry.edu] or [kylelauing@gmail.com]
// Purpose: This file is used to create a very simple framework for using
//			DirectX 9 for the GSP 381 course for DeVry University.
// Disclaimer:	
//			Copyright � 2010 by DeVry Educational Development Corporation.
//			All rights reserved.  No part of this work may be reproduced 
//			or used in any form or by any means � graphic, electronic, or 
//			mechanical, including photocopying, recording, Web distribution 
//			or information storage and retrieval systems � without the 
//			prior consent of DeVry Educational Development Corporation.
//////////////////////////////////////////////////////////////////////////
#include "DirectXFramework.h"


#define MOVE_UP 0x000000001
#define MOVE_DOWN 0x000000002
#define MOVE_LEFT 0x000000004
#define MOVE_RIGHT 0x00000008
#define STRETCH 0x00000010
#define SHRINK 0x00000020
#define DISPLAY_SPRITE 0x00000040


ID3DXSprite*			m_pD3DSprite;
IDirect3DTexture9 *		m_pTexture;
D3DXIMAGE_INFO			m_imageInfo;
ID3DXFont*				m_pD3DFont;


CDirectXFramework::CDirectXFramework(void)
{
	// Init or NULL objects before use to avoid any undefined behavior
	m_bVsync		= false;
	m_pD3DObject	= 0;
	m_pD3DDevice	= 0;
	
}

CDirectXFramework::~CDirectXFramework(void)
{
	// If Shutdown is not explicitly called correctly, call it when 
	// this class is destroyed or falls out of scope as an error check.
	Shutdown();
}

void CDirectXFramework::Init(HWND& hWnd, HINSTANCE& hInst, bool bWindowed)
{
	m_hWnd = hWnd;

	//////////////////////////////////////////////////////////////////////////
	// Direct3D Foundations - D3D Object, Present Parameters, and D3D Device
	//////////////////////////////////////////////////////////////////////////

	// Create the D3D Object
	m_pD3DObject = Direct3DCreate9(D3D_SDK_VERSION);

	// Find the width and height of window using hWnd and GetWindowRect()
	RECT rect;

	GetWindowRect(hWnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// Set D3D Device presentation parameters before creating the device
	D3DPRESENT_PARAMETERS D3Dpp;
	ZeroMemory(&D3Dpp, sizeof(D3Dpp));  // NULL the structure's memory

	D3Dpp.hDeviceWindow					= hWnd;										// Handle to the focus window
	D3Dpp.Windowed						= bWindowed;								// Windowed or Full-screen boolean
	D3Dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;								// Format of depth/stencil buffer, 24 bit depth, 8 bit stencil
	D3Dpp.EnableAutoDepthStencil		= TRUE;										// Enables Z-Buffer (Depth Buffer)
	D3Dpp.BackBufferCount				= 1;										// Change if need of > 1 is required at a later date
	D3Dpp.BackBufferFormat				= D3DFMT_X8R8G8B8;							// Back-buffer format, 8 bits for each pixel
	D3Dpp.BackBufferHeight				= height;									// Make sure resolution is supported, use adapter modes
	D3Dpp.BackBufferWidth				= width;									// (Same as above)
	D3Dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;					// Discard back-buffer, must stay discard to support multi-sample
	D3Dpp.PresentationInterval			= m_bVsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE; // Present back-buffer immediately, unless V-Sync is on								
	D3Dpp.Flags							= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;		// This flag should improve performance, if not set to NULL.
	D3Dpp.FullScreen_RefreshRateInHz	= bWindowed ? 0 : D3DPRESENT_RATE_DEFAULT;	// Full-screen refresh rate, use adapter modes or default
	D3Dpp.MultiSampleQuality			= 0;										// MSAA currently off, check documentation for support.
	D3Dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;						// MSAA currently off, check documentation for support.

	// Check device capabilities
	DWORD deviceBehaviorFlags = 0;
	m_pD3DObject->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_D3DCaps);

	// Determine vertex processing mode
	if(m_D3DCaps.DevCaps & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		// Hardware vertex processing supported? (Video Card)
		deviceBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;	
	}
	else
	{
		// If not, use software (CPU)
		deviceBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING; 
	}
	
	// If hardware vertex processing is on, check pure device support
	if(m_D3DCaps.DevCaps & D3DDEVCAPS_PUREDEVICE && deviceBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		deviceBehaviorFlags |= D3DCREATE_PUREDEVICE;	
	}
	
	// Create the D3D Device with the present parameters and device flags above
	m_pD3DObject->CreateDevice(
		D3DADAPTER_DEFAULT,		// which adapter to use, set to primary
		D3DDEVTYPE_HAL,			// device type to use, set to hardware rasterization
		hWnd,					// handle to the focus window
		deviceBehaviorFlags,	// behavior flags
		&D3Dpp,					// presentation parameters
		&m_pD3DDevice);			// returned device pointer

	//*************************************************************************

	//////////////////////////////////////////////////////////////////////////
	// Create a Font Object
	//////////////////////////////////////////////////////////////////////////
	
	// Load a font for private use for this process

	// Load D3DXFont, each font style you want to support will need an ID3DXFont
	D3DXCreateFont(m_pD3DDevice, 30, 0, FW_BOLD, 0, false, 
                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, TEXT("Times New Roman"), 
                  &m_pD3DFont);

	

	//////////////////////////////////////////////////////////////////////////
	// Create Sprite Object and Textures
	//////////////////////////////////////////////////////////////////////////
	// Create a sprite object, note you will only need one for all 2D sprites

	D3DXCreateSprite(m_pD3DDevice, &m_pD3DSprite);

	// Create a texture, each different 2D sprite to display to the screen
	// will need a new texture object.  If drawing the same sprite texture
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"test.tga", 0, 0, 0, 0,
                  D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, 
                  D3DX_DEFAULT, D3DCOLOR_XRGB(255, 0, 255), 
                  &m_imageInfo, 0, &m_pTexture);
	// multiple times, just call that sprite's Draw() with different 
	// transformation values.

	srand (time(NULL));
	for(int i = 0; i < numSprites; ++i)
	{
		mysprite[i].xp = rand()%750+50;
		mysprite[i].yp = rand()%550+50;
		mysprite[i].size = rand()%2+1;
		mysprite[i].rot = 0;


	}

	//*************************************************************************

	// create direct input object
	DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,(void **)&m_pDIObject, NULL);

	// Create Keyboard
	m_pDIObject->CreateDevice(GUID_SysKeyboard, &m_pDIKeyboard, NULL);

	//Set Keyboard data format
	m_pDIKeyboard->SetDataFormat(&c_dfDIKeyboard); 

	//Set Keyboard coop level
	m_pDIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 

	// Create Mouse
	m_pDIObject->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL);

	// Set Mouse Data Format
	m_pDIMouse->SetDataFormat(&c_dfDIMouse2);

	// Set Mouse Coop Level
	m_pDIMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
}

void CDirectXFramework::Update()
{
	Getinput();
	Pollinput();

	// get current keyboard
	hr = m_pDIKeyboard->GetDeviceState(sizeof(keyboardBuffer), (LPVOID)&keyboardBuffer);

	// get current mouse
//	hr = m_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState);


	
}
static int FPS = 0;

void CDirectXFramework::Render()
{
	timeGetTime();
	RECT rect;
	// If the device was not created successfully, return
	if(!m_pD3DDevice)
		return;
	//*************************************************************************
	

	//////////////////////////////////////////////////////////////////////////
	// All draw calls between swap chain's functions, and pre-render and post- 
	// render functions (Clear and Present, BeginScene and EndScene)
	//////////////////////////////////////////////////////////////////////////
	
	// Clear the back buffer, call BeginScene()
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	m_pD3DDevice->BeginScene();
	
			//////////////////////////////////////////////////////////////////////////
			// Draw 3D Objects (for future labs - not used in Week #1)
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			// Draw 2D sprites
			//////////////////////////////////////////////////////////////////////////
			// Note: You should only be calling the sprite object's begin and end once, 
			// with all draw calls of sprites between them

			// Call Sprite's Begin to start rendering 2D sprite objects
				m_pD3DSprite->Begin(NULL);
				//////////////////////////////////////////////////////////////////////////
				// Matrix Transformations to control sprite position, scale, and rotate
				// Set these matrices for each object you want to render to the screen
				//////////////////////////////////////////////////////////////////////////
				// Scaling
				// Rotation on Z axis, value in radians, converting from degrees
				// Translation
				// Multiply scale and rotation, store in scale
				// Multiply scale and translation, store in world
				// Set Transform

				D3DXMATRIX transMat, rotMat, scaleMat, worldMat;
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);

				for(int i = 0; i < numSprites; ++i)
				{
					if(i == numSprites-1)
					{

						D3DXMatrixRotationZ(&rotMat, D3DXToRadian(FPS));
						FPS++;
						if(controlActive & MOVE_DOWN) 
						{
							mysprite[numSprites-1].yp = mysprite[numSprites-1].yp + 1;
						}

						if(controlActive & MOVE_UP) 
						{
							mysprite[numSprites-1].yp = mysprite[numSprites-1].yp - 1;
						}

						if(controlDown & STRETCH) 
						{
							mysprite[numSprites-1].size = mysprite[numSprites-1].size + 1;
						}

						if(controlDown & SHRINK) 
						{
							mysprite[numSprites-1].size = mysprite[numSprites-1].size - 1;
						}

					}
					D3DXMatrixScaling(&scaleMat, mysprite[i].size, mysprite[i].size, 0.0f);
					D3DXMatrixTranslation(&transMat, mysprite[i].xp, mysprite[i].yp, 0.0f);
					D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
					D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
					// Set Transform for the object m_pD3DSprite
					m_pD3DSprite->SetTransform(&worldMat);

					// Draw the texture with the sprite object
					m_pD3DSprite->Draw(m_pTexture, 0, &D3DXVECTOR3(m_imageInfo.Width * 0.5f, 
									m_imageInfo.Height * 0.5f, 0.0f), 0,
									D3DCOLOR_ARGB(255, 255, 255, 255));
				
				}

				// End drawing 2D sprites
				m_pD3DSprite->End();

			//////////////////////////////////////////////////////////////////////////
			// Draw Text
			//////////////////////////////////////////////////////////////////////////
			
			
			// Calculate RECT structure for text drawing placement, using whole screen
			rect.left = 10;
			rect.right = 10;
			rect.bottom = 10;
			rect.top = 10;

			// Draw Text, using DT_TOP, DT_RIGHT for placement in the top right of the
			// screen.  DT_NOCLIP can improve speed of text rendering, but allows text
			m_pD3DFont->DrawText(0, L"William Bowden", -1, &rect, 
                 DT_TOP | DT_LEFT | DT_NOCLIP, 
                 D3DCOLOR_ARGB(255, 255, 255, 255));

			rect.left = 500;
			rect.right = 10;
			rect.bottom = 10;
			rect.top = 10;

			wchar_t text[256];
			swprintf(text, 256, L"FPS: %i", FPS);

			m_pD3DFont->DrawText(0, text, -1, &rect, 
                 DT_TOP | DT_LEFT | DT_NOCLIP, 
                 D3DCOLOR_ARGB(255, 255, 255, 255));
			
			// to be drawn outside of the rect specified to draw text in.

			// EndScene, and Present the back buffer to the display buffer
			m_pD3DDevice->EndScene();
			m_pD3DDevice->Present(NULL, NULL, NULL, NULL);

	//*************************************************************************
	
}

void CDirectXFramework::Shutdown()
{
	
	
	//*************************************************************************
	// Release COM objects in the opposite order they were created in

	// Texture
	m_pTexture->Release();
	// Sprite
	m_pD3DSprite->Release();
	// Font
	m_pD3DFont->Release();
	// 3DDevice	
		
	// 3DObject

	//*************************************************************************

	
}

void CDirectXFramework::Getinput()
{
	controlCurrent = 0;

	if(keyboardBuffer[DIK_UP] & 0x80) controlCurrent |= MOVE_UP;
	if(keyboardBuffer[DIK_DOWN] & 0x80) controlCurrent |= MOVE_DOWN;
	if(keyboardBuffer[DIK_LEFT] & 0x80) controlCurrent |= MOVE_LEFT;
	if(keyboardBuffer[DIK_RIGHT] & 0x80) controlCurrent |= MOVE_RIGHT;
	if(mouseState.rgbButtons[0] & 0x80)
		controlCurrent |= SHRINK;
	if(mouseState.rgbButtons[1] & 0x80)
		controlCurrent |= STRETCH;


	controlActive = controlCurrent;
	controlDown = (controlCurrent ^ controlPrevious) & controlCurrent;
	controlUp = (controlCurrent ^ controlPrevious) & controlPrevious;

	controlPrevious = controlCurrent;


}

void CDirectXFramework::Pollinput()
{
		// Poll keyboard
		hr = m_pDIKeyboard->GetDeviceState(sizeof(keyboardBuffer), (void**)&keyboardBuffer);

		if( FAILED(hr) )
		{
			//Keyboard lost, zero out keyboard data stucture.
			ZeroMemory(keyboardBuffer, sizeof(keyboardBuffer));
			// Try to acquire for next time we poll
			hr = m_pDIKeyboard->Acquire();
		}

		// Poll Mouse
		hr = m_pDIMouse->GetDeviceState(sizeof(mouseState), (void**)&mouseState);

		if( FAILED(hr) )
		{
			//Keyboard lost, zero out keyboard data stucture.
			ZeroMemory(&mouseState, sizeof(mouseState));
			// Try to acquire for next time we poll
			hr = m_pDIMouse->Acquire();
		}
}
