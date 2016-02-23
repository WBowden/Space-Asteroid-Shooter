#include "sprite.h"

	CSprite::CSprite() :
	m_pSpriteTexture( NULL ),
	m_pD3DXSprite( NULL ),
	m_spriteW( 0 ),
	m_spriteH( 0 ),
	m_index( 0 ),
	m_State( 0 ),
	m_posX( 0.0f ),
	m_posY( 0.0f ),
	m_speedX( 0.0f ),
	m_speedY( 0.0f ),

	m_isVisible( true ),
	m_Collided( true ),
	m_frameAnimate( true ),
	m_isActive( true ),
	m_partofScript( false ),
	m_collideModify( false ),
	m_oneFrame( false ),
	m_isDestroyed( false ),

	m_fpsModify( 0 ),
	m_frameSkip( 0 ),
	m_frameW( 0 ),
	m_frameH( 0 ),
	m_frameAcross( 1 ),
	m_offsetX( 0 ),
	m_offsetY( 0 ),
	m_frameScaleW( 0 ),
	m_frameScaleH( 0 ),
	m_CurrAnimation( 0 ),
	m_CurrFrame( 0 ),
	m_topCollision( 0 ),
	m_botCollision( 0 ),
	m_leftCollision( 0 ),


m_rightCollision( 0 )
{
    for( int i = 0; i < TOTAL_ANIMATIONS; i++ )
        m_NumOfAnimations[i] = NULL;

    for(int i = 0; i < MAX_SIZE; i++ )
    {
        m_spriteType[i]   = NULL;
        m_spriteName[i]   = NULL;
        m_textureName[i] = NULL;
    }
}

CSprite::~CSprite()
{
    if( m_pSpriteTexture != NULL ) 
        m_pSpriteTexture->Release();

    if( m_pD3DXSprite != NULL ) 
        m_pD3DXSprite->Release();
}


void CSprite::zeroSpriteValues()
{
    m_spriteW            = 0;
    m_spriteH            = 0;
    m_index              = 0;
    m_State				 = 0;
    m_posX				 = 0;
    m_posY				 = 0;
    m_speedX			 = 0;
    m_speedY			 = 0;
    m_isVisible          = true;
    m_Collided           = true;
    m_frameAnimate       = true;
    m_isActive           = true;
    m_partofScript       = false;
    m_collideModify		 = false;
    m_oneFrame			 = false;
    m_isDestroyed        = false;
    m_fpsModify			 = 0;
    m_frameSkip			 = 0;
    m_frameW			 = 0;
    m_frameH			 = 0;
    m_frameAcross		 = 1;
    m_offsetX			 = 0;
    m_offsetY			 = 0;
    m_frameScaleW		 = 0;
    m_frameScaleH		 = 0;
    m_CurrAnimation		 = 0;
    m_CurrFrame			 = 0;
    m_topCollision       = 0;
    m_botCollision		 = 0;
    m_leftCollision      = 0;
    m_rightCollision     = 0;

    for( int i = 0; i < TOTAL_ANIMATIONS; i++ )
        m_NumOfAnimations[i] = NULL;

    for(int i = 0; i < MAX_SIZE; i++ )
    {
        m_spriteType[i]   = NULL;
        m_spriteName[i]   = NULL;
        m_textureName[i] = NULL;
    }
}


void CSprite::releaseMemory()
{


    for( int i = 0; i < TOTAL_ANIMATIONS; i++ )
    {

        if( m_NumOfAnimations[i] != NULL )
        {
            delete [] m_NumOfAnimations[i];
            m_NumOfAnimations[i] = NULL;
        }
    }
}


void CSprite::loadAnimation( int m_AnimationNumber, int nStartFrame, int nEndFrame, 
                             END_ANIMATION nOption, int nNextAnimation )
{


    bool bOverFlow = false;
    int  frameNum = 0;
    int  frameTotal = 0;
    int  i = 0;

    if( m_AnimationNumber >= 0 && 
        m_AnimationNumber < TOTAL_ANIMATIONS && 
        m_NumOfAnimations[m_AnimationNumber] == NULL )
    {
        if( nStartFrame < nEndFrame )
            frameTotal = (nEndFrame - nStartFrame);

        if( nStartFrame > nEndFrame  )
            frameTotal = (nStartFrame - nEndFrame);

        frameTotal += 3; 

        m_NumOfAnimations[m_AnimationNumber] = new int[frameTotal];
        m_frameNum[m_AnimationNumber] = frameTotal;

        frameNum = nStartFrame;

        if( m_AnimationNumber >= 0 && m_AnimationNumber < TOTAL_ANIMATIONS )
        {
            while( frameNum != (nEndFrame + 1) )
            {
                if( i <= (frameTotal - 1) )
                    m_NumOfAnimations[m_AnimationNumber][i] = frameNum;
                else
                    bOverFlow = true;

                if( nStartFrame < nEndFrame )
                    {++frameNum;}
                else if( nStartFrame > nEndFrame )
                    {--frameNum;}

                ++i;
            }
        

            if( nOption == LOOP_ANIMATION )
                m_NumOfAnimations[m_AnimationNumber][i] = LOOP_ANIMATION;

            if( nOption == MAINTAIN_LAST_FRAME )
                m_NumOfAnimations[m_AnimationNumber][i] = MAINTAIN_LAST_FRAME;

            if( nOption == GOTO_NEXT_ANIMATION )
            {
                m_NumOfAnimations[m_AnimationNumber][i] = GOTO_NEXT_ANIMATION;
                m_NumOfAnimations[m_AnimationNumber][i+1] = nNextAnimation;
            }

            if( nOption == GO_INACTIVE )
                m_NumOfAnimations[m_AnimationNumber][i] = GO_INACTIVE;
        }
    }
}


void CSprite::loadAnimationString( int m_AnimationNumber, char *chString, 
                                   END_ANIMATION nOption, int nNextAnimation )
{

    char *token = NULL;
    char chDelimiters[] = " ";
    char chBuffer[200] = { NULL };
    bool bOverFlow = false;
    int  frameNum = 0;
    int  frameTotal = 0;
    int  i = 0;
    
    if( m_AnimationNumber >= 0 && 
        m_AnimationNumber < TOTAL_ANIMATIONS && 
        m_NumOfAnimations[m_AnimationNumber] == NULL )
    {

        strcpy( chBuffer, chString );
        token = strtok( chBuffer, chDelimiters );

        while( token != NULL )
        {
            ++frameTotal;
            token = strtok( NULL, chDelimiters );
        }

        frameTotal += 3; 

        m_NumOfAnimations[m_AnimationNumber] = new int[frameTotal];
        m_frameNum[m_AnimationNumber] = frameTotal;

        strcpy( chBuffer, chString );
        token = strtok( chBuffer, chDelimiters );

        while( token != NULL )
        {
            frameNum = atoi( token );

            if( i <= (frameTotal - 1) )
                m_NumOfAnimations[m_AnimationNumber][i] = frameNum;
            else
                bOverFlow = true;

            ++i;
            token = strtok( NULL, chDelimiters );
        }
    
        if( nOption == LOOP_ANIMATION )
            m_NumOfAnimations[m_AnimationNumber][i] = LOOP_ANIMATION;

        if( nOption == MAINTAIN_LAST_FRAME )
            m_NumOfAnimations[m_AnimationNumber][i] = MAINTAIN_LAST_FRAME;

        if( nOption == GOTO_NEXT_ANIMATION )
        {
            m_NumOfAnimations[m_AnimationNumber][i] = GOTO_NEXT_ANIMATION;
            m_NumOfAnimations[m_AnimationNumber][i+1] = nNextAnimation;
        }

        if( nOption == GO_INACTIVE )
            m_NumOfAnimations[m_AnimationNumber][i] = GO_INACTIVE;
    }
}


void CSprite::incFrame( bool bUseModifier )
{

    DWORD dwNextNumber;
    int   nTempHolder = 0;

    if( bUseModifier == false )
    {
        nTempHolder = m_fpsModify;
        m_fpsModify = 0;
    }

    if( m_fpsModify > 0 )
    {

        for( int i = 0; i < m_fpsModify; ++i )
        {

            dwNextNumber = m_NumOfAnimations[m_CurrAnimation][m_CurrFrame + 1];

            if( dwNextNumber == LOOP_ANIMATION )
                m_CurrFrame = -1;

            if( dwNextNumber == GOTO_NEXT_ANIMATION )
                return;

            if( dwNextNumber == MAINTAIN_LAST_FRAME )
                return;

            if( dwNextNumber == GO_INACTIVE )
                return;

            ++m_CurrFrame;
        }
    }
    else if( m_fpsModify < 0 )
    {
        --m_frameSkip;

        if( m_fpsModify == m_frameSkip )
        {

            dwNextNumber = m_NumOfAnimations[m_CurrAnimation][m_CurrFrame + 1];

            if( dwNextNumber == LOOP_ANIMATION )
                m_CurrFrame = -1;

            if( dwNextNumber == GOTO_NEXT_ANIMATION )
                return;

            if( dwNextNumber == MAINTAIN_LAST_FRAME )
                return;

            if( dwNextNumber == GO_INACTIVE )
                return;

            ++m_CurrFrame;
            m_frameSkip = 0;
        }
    }
    else if( m_fpsModify == 0 )
    {
 
        dwNextNumber = m_NumOfAnimations[m_CurrAnimation][m_CurrFrame + 1];

        if( dwNextNumber == LOOP_ANIMATION )
            m_CurrFrame = -1;

        if( dwNextNumber == GOTO_NEXT_ANIMATION )
            return;

        if( dwNextNumber == MAINTAIN_LAST_FRAME )
            return;

        if( dwNextNumber == GO_INACTIVE )
            return;

        ++m_CurrFrame;
    }

    if( bUseModifier == false )
        m_fpsModify = nTempHolder;
}


void CSprite::decFrame( bool bUseModifier )
{


    DWORD dwNextNumber;
    int   nTempHolder = 0;

    if( bUseModifier == false )
    {
        nTempHolder = m_fpsModify;
        m_fpsModify = 0;
    }

    if( m_fpsModify > 0 )
    {

        for( int i = 0; i < m_fpsModify; ++i )
        {
            if( m_CurrFrame <= 0 )
            {
                for( int i = 0; i < m_frameNum[m_CurrAnimation]; i++ )
                {
                    dwNextNumber = m_NumOfAnimations[m_CurrAnimation][i];

                    if( dwNextNumber == LOOP_ANIMATION )
                        m_CurrFrame = i - 1;

                    if( dwNextNumber == GOTO_NEXT_ANIMATION )
                        return;

                    if( dwNextNumber == MAINTAIN_LAST_FRAME )
                        return;

                    if( dwNextNumber == GO_INACTIVE )
                        return;
                }
            }
            else
            {
                --m_CurrFrame;
            }
        }
    }
    else if( m_fpsModify < 0 )
    {

        --m_frameSkip;

        if( m_fpsModify == m_frameSkip )
        {
            if( m_CurrFrame <= 0 )
            {
                for( int i = 0; i < m_frameNum[m_CurrAnimation]; i++ )
                {
                    dwNextNumber = m_NumOfAnimations[m_CurrAnimation][i];

                    if( dwNextNumber == LOOP_ANIMATION )
                        m_CurrFrame = i - 1;

                    if( dwNextNumber == GOTO_NEXT_ANIMATION )
                        return;

                    if( dwNextNumber == MAINTAIN_LAST_FRAME )
                        return;

                    if( dwNextNumber == GO_INACTIVE )
                        return;
                }
            }
            else
            {
                --m_CurrFrame;
            }

            m_frameSkip = 0;
        }
    }
    else if( m_fpsModify == 0 )
    {
        if( m_CurrFrame <= 0 )
        {
            for( int i = 0; i < m_frameNum[m_CurrAnimation]; i++ )
            {
                dwNextNumber = m_NumOfAnimations[m_CurrAnimation][i];

                if( dwNextNumber == LOOP_ANIMATION )
                    m_CurrFrame = i - 1;

                if( dwNextNumber == GOTO_NEXT_ANIMATION )
                    return;

                if( dwNextNumber == MAINTAIN_LAST_FRAME )
                    return;

                if( dwNextNumber == GO_INACTIVE )
                    return;
            }
        }
        else
        {
            --m_CurrFrame;
        }
    }

    if( bUseModifier == false )
        m_fpsModify = nTempHolder;
}


HRESULT CSprite::drawSprite( LPDIRECT3DDEVICE9 m_pDevice )
{
    if( m_pD3DXSprite == NULL )
    {
        D3DXIMAGE_INFO d3dxImageInfo;

        D3DXCreateTextureFromFileEx( m_pDevice,
                                     m_textureName,
                                     m_spriteW,
                                     m_spriteH,
                                     1,
                            0,
                                     D3DFMT_UNKNOWN,
                            D3DPOOL_MANAGED,
                                     D3DX_DEFAULT,
                                     D3DX_DEFAULT,
                                     D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
                                     &d3dxImageInfo,
                                     NULL,
                                     &m_pSpriteTexture );

        D3DXCreateSprite( m_pDevice, &m_pD3DXSprite );
    }

    HRESULT hr = 0;

    if( m_isActive == true )
    {
        RECT  rcSource;
        DWORD dwFrameNumber;
        DWORD dwNextNumber;

        if( m_oneFrame == false )
        {
            dwFrameNumber = m_NumOfAnimations[m_CurrAnimation][m_CurrFrame];
        }
        else
        {
            dwFrameNumber = 0;
            m_frameAcross = 1;
        }

        rcSource.top    = ( ( dwFrameNumber / m_frameAcross ) * m_frameH );
        rcSource.left   = ( ( dwFrameNumber % m_frameAcross ) * m_frameW );
        rcSource.bottom = rcSource.top  + m_frameH;
        rcSource.right  = rcSource.left + m_frameW;

        if( m_offsetX != 0 || m_offsetY != 0 )
        {
            rcSource.top    += m_offsetY;
            rcSource.left   += m_offsetX;
            rcSource.bottom += m_offsetY;
            rcSource.right  += m_offsetX;
        }

        D3DXVECTOR3 vCenter( 0.0f, 0.0f, 0.0f );
        D3DXVECTOR3 vPosition( m_posX, m_posY, 0.0f );

        m_pD3DXSprite->Begin( D3DXSPRITE_ALPHABLEND );
        {
            m_pD3DXSprite->Draw( m_pSpriteTexture,
                                 &rcSource,
                                 &vCenter,
                                 &vPosition,
                                 D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f) );
        }
        m_pD3DXSprite->End();

        if( m_frameAnimate == true && m_oneFrame == false)
        {
   
            dwNextNumber = m_NumOfAnimations[m_CurrAnimation][m_CurrFrame+1];

            if( dwNextNumber == LOOP_ANIMATION )
                m_CurrFrame = -1;

            if( dwNextNumber == GOTO_NEXT_ANIMATION )
            {
                m_CurrAnimation = m_NumOfAnimations[m_CurrAnimation][m_CurrFrame+2];
                m_CurrFrame = -1;
            }

            if( dwNextNumber == GO_INACTIVE )
            {
                m_isActive = false;
                m_CurrFrame = -1;
            }

            if( dwNextNumber == MAINTAIN_LAST_FRAME )
                return hr;

            if( m_fpsModify == 0 )
            {
                ++m_CurrFrame;
            }
            else
            {
        
                if( m_fpsModify < 0 )
                {

                    --m_frameSkip;

                    if( m_fpsModify == m_frameSkip )
                    {
                        ++m_CurrFrame;
                        m_frameSkip = 0;
                    }
                }
                else
                {
                    for( int i = 0; i < m_fpsModify; ++i )
                    {
                        incFrame(false);
                    }
                }
            }
        }
    }

    return hr;
}
