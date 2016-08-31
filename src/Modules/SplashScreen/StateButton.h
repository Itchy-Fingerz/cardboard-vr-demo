// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#pragma once

#include "ICallback.h"
#include "UISprite.h"
#include "UIQuad/UIQuad.h"
#include "IUICameraProvider.h"
#include "UIInteraction/IUIInteractableItem.h"
#include "UIAnimatedSprite.h"
#include "UIProgressBarConfig.h"

namespace Eegeo
{
    namespace UI
    {
        namespace SplashScreen
        {
            class StateButton : public Eegeo::UI::IUIInteractableItem
            {
            private:
                
                UISprite* m_pActiveState;
                UISprite* m_pDeactiveState;
                UI::UIAnimatedSprite* m_pGazeProgress;
                
                Eegeo::Helpers::ICallback0& m_onClickedEvent;
                float m_radius;
                
                void calculateUV(int spriteSize, int position, Eegeo::v2& outMinUV, Eegeo::v2&outMaxUV);
                
            public:
                StateButton(IUIRenderableFilter& uiRenderableFilter
                                 , Eegeo::UI::IUIQuadFactory& quadFactory
                                 , const std::string& assetPath
                                 , int activeSpriteId
                                 , int deactiveSpriteId
                                 , int spriteSheetSize
                                 , const UIProgressBarConfig& progressBarConfig
                                 , Eegeo::Helpers::ICallback0& onClickedEvent
                                 , const Eegeo::v2& size = Eegeo::v2::One()
                                 , const Eegeo::dv3& ecefPosition = Eegeo::dv3::Zero()
                                 , const Eegeo::v3& scale = Eegeo::v3::One()
                                 , const Eegeo::v4& color = Eegeo::v4::One()
                                 );
                
                virtual ~StateButton();
                
                void Init(Eegeo::UI::IUIQuadFactory& quadFactory, UI::IUIRenderableFilter& uiRenderableFilter, const std::string& assetPath, const UIProgressBarConfig& progressBarConfig, const Eegeo::v2& size, const Eegeo::dv3& ecefPosition, const Eegeo::v3& scale, const Eegeo::v4& color, int activeSpriteId, int deactiveSpriteId, int spriteSheetSize);
                virtual void Update(float dt);
                virtual bool IsCollidingWithPoint(const Eegeo::v2& screenPoint, IUICameraProvider& cameraProvider);
                
                virtual void OnItemClicked();
                virtual void OnFocusGained();
                virtual void OnFocusLost();
                
                void SetActive();
                void SetDeactive();
                
                void Hide();
                void Show();
                
                const virtual float GetItemRadius() { return m_radius; }
                
                void SetEcefPosition(const Eegeo::dv3& position);
                Eegeo::dv3 GetEcefPosition();
                
                void SetSize(const Eegeo::v2& size);
                Eegeo::v2 GetSize();
                
                void SetColor(const Eegeo::v4& color);
                Eegeo::v4 GetColor();
                
                void SetScale(const Eegeo::v3& scale);
                Eegeo::v3 GetScale();
                
            };
        }
    }
}
