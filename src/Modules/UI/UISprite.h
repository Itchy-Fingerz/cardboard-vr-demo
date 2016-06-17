//
//  UISprite.hpp
//  SDKSamplesApp
//
//  Created by Aqif Hamid on 5/24/16.
//
//

#ifndef UISprite_h
#define UISprite_h

#include "UIQuad/UIQuad.h"
#include "IUIRenderableFilter.h"

namespace Eegeo
{
    namespace UI
    {
        class UISprite 
        {
        
        private:
            IUIRenderableFilter& m_UIRenderableFilter;
            
            UIQuad* m_Quad;
            Eegeo::v2 m_Size;
            
        public:
            
            UISprite(IUIRenderableFilter& p_UIRenderableFilter
                     , UIQuad* quad
                     , const Eegeo::v2& size = Eegeo::v2::One()
                     , const Eegeo::dv3& ecefPosition = Eegeo::dv3::Zero()
                     , const Eegeo::v3& scale = Eegeo::v3::One()
                     , const Eegeo::v4& color = Eegeo::v4::One());
            
            virtual ~UISprite();
            
            virtual void Update(float dt);
            
            void SetEcefPosition(const Eegeo::dv3& position);
            Eegeo::dv3 GetEcefPosition();
            
            void SetSize(const Eegeo::v2& size);
            Eegeo::v2 GetSize();
            
            void SetColor(const Eegeo::v4& color);
            Eegeo::v4 GetColor();
            
            void SetScale(const Eegeo::v3& scale);
            Eegeo::v3 GetScale();
            
            bool GetItemShouldRender(){ return m_Quad->GetItemShouldRender(); }
            void SetItemShouldRender(bool shouldRender){ m_Quad->SetItemShouldRender(shouldRender); }
            
        };
        
    }
}

#endif /* UISprite_h */
