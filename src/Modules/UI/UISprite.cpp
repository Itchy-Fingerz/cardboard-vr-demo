//
//  UISprite.cpp
//  SDKSamplesApp
//
//  Created by Aqif Hamid on 5/24/16.
//
//

#include "UISprite.h"

#include "UISprite.h"
#include "RenderingModule.h"
#include "IPlatformAbstractionModule.h"

namespace Eegeo
{
    namespace UI
    {
        
        UISprite::UISprite(IUIRenderableFilter& uiRenderableFilter
                           , UIQuad* pQuad
                           , const Eegeo::v2& size
                           , const Eegeo::dv3& ecefPosition
                           , const Eegeo::v3& scale
                           , const Eegeo::v4& color)
        : m_pQuad(pQuad)
        , m_UIRenderableFilter(uiRenderableFilter){
            m_Size = size;
            
            m_UIRenderableFilter.RegisterRenderable(m_pQuad);
            SetEcefPosition(ecefPosition);
            SetScale(scale);
            SetColor(color);
        }
        
        UISprite::~UISprite()
        {
            m_UIRenderableFilter.UnRegisterRenderable(m_pQuad);
            delete m_pQuad;
        }
        
        void UISprite::Update(float dt)
        {
        }
        
        void UISprite::SetEcefPosition(const Eegeo::dv3& position)
        {
            m_pQuad->SetEcefPosition(position);
        }
        
        Eegeo::dv3 UISprite::GetEcefPosition()
        {
            return m_pQuad->GetEcefPosition();
        }
        
        void UISprite::SetSize(const Eegeo::v2& size)
        {
            m_Size = Eegeo::v2(size);
        }
        
        Eegeo::v2 UISprite::GetSize()
        {
            return m_Size;
        }
        
        void UISprite::SetColor(const Eegeo::v4& color)
        {
            m_pQuad->SetColor(color);
        }
        
        Eegeo::v4 UISprite::GetColor()
        {
            return m_pQuad->GetColor();
        }
        
        void UISprite::SetScale(const Eegeo::v3& scale)
        {
            m_pQuad->SetScale(scale);
        }
        
        Eegeo::v3 UISprite::GetScale()
        {
            return m_pQuad->GetScale();
        }
        
        
    }
}
