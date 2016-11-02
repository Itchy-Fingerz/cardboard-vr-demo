// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "JumpPointsModule.h"

namespace Eegeo
{
    namespace UI
    {
        namespace JumpPoints
        {
            JumpPointsModule::JumpPointsModule(IUIRenderableFilter& uiRenderableFilter
                                               , IUIQuadFactory& uiQuadFactory
                                               , IUIInteractionObservable& uiInteractionObservable
                                               , const UIProgressBarConfig& progressBarConfig
                                               , Eegeo::Helpers::ICallback1<JumpPoint&>& onJumpPointSelected
                                               )
            {
                m_pViewFactory = Eegeo_NEW(JumpPointViewFactory)(uiRenderableFilter
                                                                , uiQuadFactory
                                                                , progressBarConfig
                                                                , onJumpPointSelected);
                
                m_pRepository = Eegeo_NEW(JumpPointRepository)();
                m_pController = Eegeo_NEW(JumpPointController)(*m_pRepository, *m_pViewFactory, uiInteractionObservable);
            }
            
            JumpPointsModule::~JumpPointsModule()
            {
                Eegeo_DELETE m_pController;
                Eegeo_DELETE m_pRepository;
                Eegeo_DELETE m_pViewFactory;
            }
            
            void JumpPointsModule::Update(float dt)
            {
                m_pController->Update(dt);
            }
            
            JumpPointRepository& JumpPointsModule::GetRepository() const
            {
                return *m_pRepository;
            }
            
            JumpPointController& JumpPointsModule::GetController() const
            {
                return *m_pController;
            }
            
        }
    }
}
