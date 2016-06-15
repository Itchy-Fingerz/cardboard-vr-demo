//
//  UIAnimatedQuad.cpp
//  SDKSamplesApp
//
//  Created by Aqif Hamid on 5/24/16.
//
//

#include "UIAnimatedSprite.h"
#include "UISprite.h"
#include "RenderingModule.h"
#include "EegeoWorld.h"
#include "Logger.h"

namespace Eegeo
{
    namespace UI
    {
        
        UIAnimatedSprite::UIAnimatedSprite(IUIRenderableFilter& p_UIRenderableFilter
                         , UIQuad* quad
                         , float frameRate
                         , Eegeo::v2 spriteGridSize
                         , int spriteId
                         , Eegeo::v2 size
                         , Eegeo::dv3 ecefPosition
                         , Eegeo::v3 scale
                         , Eegeo::v4 color)
        : UISprite(p_UIRenderableFilter, quad, size, ecefPosition, scale, color, spriteId, spriteGridSize)
        , m_FrameRate(frameRate)
        {
            m_TimeElapsed = 0;
        }
        
        UIAnimatedSprite::~UIAnimatedSprite()
        {}
        
        void UIAnimatedSprite::Reset()
        {
            m_TimeElapsed = 0.0f;
        }
        
        void UIAnimatedSprite::Update(float dt)
        {
            m_TimeElapsed += dt;
            if(m_TimeElapsed>(1.0f/m_FrameRate)) {
                Reset();
                SetNextSprite();
            }
            UISprite::Update(dt);
            
        }
        
        
    }
}