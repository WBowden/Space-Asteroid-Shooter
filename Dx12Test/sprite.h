#include "libraries.h"

#ifndef CSPRITE_H_INCLUDED


const int TOTAL_ANIMATIONS =  5; // Max number of animation states per sprite
const int MAX_SIZE         = 25;

class CSprite
{

public:

	enum END_ANIMATION
	{
		LOOP_ANIMATION = 200,
		GOTO_NEXT_ANIMATION,
		MAINTAIN_LAST_FRAME,
		GO_INACTIVE,
		DESTROY_SPRITE
	};

	CSprite();
	~CSprite();
	void zeroSpriteValues();
	void releaseMemory();
	void incFrame( bool bUseModifier = true );
	void decFrame( bool bUseModifier = true );
	void loadAnimation( int nAnimeNumber, int nStartFrame, int nEndFrame, 
		                END_ANIMATION nOption, int nNextAnimation = NULL );
	void loadAnimationString( int nAnimeNumber, char *chString, 
		                      END_ANIMATION nOption, int nNextAnimation = NULL );
	HRESULT drawSprite( LPDIRECT3DDEVICE9 m_pDevice );
	
	LPDIRECT3DTEXTURE9 m_pSpriteTexture;
	LPD3DXSPRITE       m_pD3DXSprite;

	float	m_posX;
	float	m_posY;
	float	m_speedX;
	float	m_speedY;

	int     m_index;
	int     m_State;

	wchar_t	m_spriteType[MAX_SIZE];
	wchar_t	m_spriteName[MAX_SIZE];
	wchar_t	m_textureName[MAX_SIZE];

	bool	m_isVisible;
	bool    m_isActive;

	bool	m_Collided;
	bool    m_collideModify;
	bool    m_isDestroyed;

	bool    m_partofScript;
	bool    m_frameAnimate;
	bool    m_oneFrame;

	int    *m_NumOfAnimations[TOTAL_ANIMATIONS];
	int     m_frameNum[TOTAL_ANIMATIONS];
	int     m_frameSkip;

	int		m_spriteW;
	int		m_spriteH;

	int     m_fpsModify;
	int		m_frameW;
	int		m_frameH;
	int     m_frameScaleW;
	int     m_frameScaleH;
	int		m_frameAcross;

	int     m_topCollision;
	int     m_botCollision;
	int     m_leftCollision;
	int     m_rightCollision;

	int     m_offsetX;
	int     m_offsetY;

	int 	m_CurrAnimation;
	int		m_CurrFrame;
	
};

#define CSPRITE_H_INCLUDED
#endif