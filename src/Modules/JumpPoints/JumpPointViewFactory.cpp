// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "JumpPointViewFactory.h"
#include "VectorMath.h"

namespace Eegeo
{
    namespace UI
    {
        namespace JumpPoints
        {
            
            JumpPointViewFactory::JumpPointViewFactory(IUIRenderableFilter& uiRenderableFilter
                                                       , IUIQuadFactory& uiQuadFactory
                                                       , IUICameraProvider& uiCameraProvider
                                                       , InteriorsExplorer::IInteriorsExplorerModule& interiorsExplorerModule)
            : m_uiRenderableFilter(uiRenderableFilter)
            , m_uiQuadFactory(uiQuadFactory)
            , m_uiCameraProvider(uiCameraProvider)
            , m_interiorsExplorerModule(interiorsExplorerModule)
            {
                
            }
            
            JumpPointViewFactory::~JumpPointViewFactory()
            {
                
            }
            
            JumpPointView* JumpPointViewFactory::CreateViewForJumpPoint(JumpPoint& jumpPointModel)
            {
                JumpPointView* pJumpPointView = Eegeo_NEW(JumpPointView)(jumpPointModel
                                                                        , m_uiRenderableFilter
                                                                        , m_uiQuadFactory.CreateUIQuad(jumpPointModel.GetFileName()
                                                                        , jumpPointModel.GetDimensions()
                                                                        , jumpPointModel.GetUVMin()
                                                                        , jumpPointModel.GetUVMax())
                                                                        , m_uiCameraProvider
                                                                        , m_interiorsExplorerModule
                                                                        );
                return pJumpPointView;
            }
            
        }
    }
}
