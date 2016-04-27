// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "InteriorsExplorerModule.h"
#include "InteriorsExplorerModel.h"
#include "GlobeCameraController.h"
#include "InteriorsCameraControllerFactory.h"
#include "InteriorsCameraController.h"
#include "GlobeCameraTouchController.h"
#include "InteriorVisibilityUpdater.h"
#include "InteriorSelectionModel.h"
#include "InteriorInteractionModel.h"
#include "Logger.h"

    namespace InteriorsExplorer
    {
        namespace SdkModel
        {
            
            InteriorsExplorerModule::InteriorsExplorerModule(
                                                             Eegeo::Resources::Interiors::InteriorInteractionModel& interiorInteractionModel,
                                                             Eegeo::Resources::Interiors::InteriorSelectionModel& interiorSelectionModel,
                                                             Eegeo::Resources::Interiors::InteriorTransitionModel& interiorTransitionModel,
                                                             Eegeo::Resources::Interiors::Markers::InteriorMarkerModelRepository& markerRepository,
//                                                             WorldPins::SdkModel::IWorldPinsService& worldPinsService,
                                                             Eegeo::Rendering::EnvironmentFlatteningService& environmentFlatteningService,
//                                                             VisualMap::SdkModel::IVisualMapService& visualMapService,
                                                             const Eegeo::Resources::Interiors::InteriorsCameraControllerFactory& interiorCameraControllerFactory,
                                                             const Eegeo::Rendering::ScreenProperties& screenProperties,
                                                             Eegeo::Helpers::IIdentityProvider& identityProvider,
//                                                             ExampleAppMessaging::TMessageBus& messageBus,
//                                                             Metrics::IMetricsService& metricsService,
//                                                             const InitialExperience::SdkModel::IInitialExperienceModel& initialExperienceModel,
                                                             const bool interiorsAffectedByFlattening) :
            m_InteriorInteractionModel(interiorInteractionModel)
            {
//                m_pUserInteractionModel = Eegeo_NEW(InteriorExplorerUserInteractionModel)();
                
                const float transitionTime = 0.5f;
                m_pVisibilityUpdater = Eegeo_NEW(InteriorVisibilityUpdater)(interiorTransitionModel, interiorSelectionModel, interiorInteractionModel, transitionTime);
                
//                m_pGlobeCameraTouchController = interiorCameraControllerFactory.CreateTouchController(screenProperties);
//                
//                m_pGlobeCameraController = interiorCameraControllerFactory.CreateInteriorGlobeCameraController(false,
//                                                                                                               *m_pGlobeCameraTouchController,
//                                                                                                               screenProperties);
//
                
                Eegeo::Resources::Interiors::InteriorsCameraConfiguration cameraConfig = Eegeo::Resources::Interiors::InteriorsCameraController::CreateDefaultConfig();
                
//                m_pInteriorsCameraController = interiorCameraControllerFactory.CreateInteriorsCameraController(cameraConfig,
//                                                                                                               *m_pGlobeCameraTouchController,
//                                                                                                               *m_pGlobeCameraController,
//                                                                                                               interiorsAffectedByFlattening);
                
//                
//                m_pWorldPinController = Eegeo_NEW(InteriorWorldPinController)(interiorSelectionModel,
//                                                                              markerRepository,
//                                                                              worldPinsService,
//                                                                              *m_pInteriorsCameraController,
//                                                                              messageBus,
//                                                                              initialExperienceModel);
                Eegeo::Resources::Interiors::InteriorId id("westport_house");
                interiorSelectionModel.SelectInteriorId(id);
                m_pModel = Eegeo_NEW(InteriorsExplorerModel)(interiorInteractionModel,
                                                             interiorSelectionModel);
                
//                m_pViewModel = Eegeo_NEW(View::InteriorsExplorerViewModel)(false, identityProvider.GetNextIdentity(), messageBus);
//                m_pModel->ShowInteriorExplorer();
            }
            
            InteriorsExplorerModule::~InteriorsExplorerModule()
            {
//                Eegeo_DELETE m_pViewModel;
                Eegeo_DELETE m_pModel;
//                Eegeo_DELETE m_pWorldPinController;
                Eegeo_DELETE m_pInteriorsCameraController;
                Eegeo_DELETE m_pGlobeCameraTouchController;
                Eegeo_DELETE m_pGlobeCameraController;
                Eegeo_DELETE m_pVisibilityUpdater;
//                Eegeo_DELETE m_pUserInteractionModel;
            }
            
//            View::InteriorsExplorerViewModel& InteriorsExplorerModule::GetInteriorsExplorerViewModel() const
//            {
//                return *m_pViewModel;
//            }
//            
//            ScreenControl::View::IScreenControlViewModel& InteriorsExplorerModule::GetScreenControlViewModel() const
//            {
//                return *m_pViewModel;
//            }
            
            InteriorVisibilityUpdater& InteriorsExplorerModule::GetInteriorVisibilityUpdater() const
            {
                return *m_pVisibilityUpdater;
            }
            
            Eegeo::Resources::Interiors::InteriorsCameraController& InteriorsExplorerModule::GetInteriorsCameraController() const
            {
                return *m_pInteriorsCameraController;
            }
            
            bool InteriorsExplorerModule::InteriorLoaded(){
                return m_InteriorInteractionModel.HasInteriorModel();
            }
            
            void InteriorsExplorerModule::Update(float dt) const
            {
                m_pVisibilityUpdater->Update(dt);
//                m_pWorldPinController->Update(dt);
                
                
            }
            
            void InteriorsExplorerModule::ToggleInteriorDisplay(){
//                m_InteriorInteractionModel.SetSelectedFloorIndex(2);
                
               m_pVisibilityUpdater->SetInteriorShouldDisplay(!m_pVisibilityUpdater->GetInteriorShouldDisplay());
            }
            
            InteriorsExplorerModel& InteriorsExplorerModule::GetInteriorsExplorerModel() const
            {
                return *m_pModel;
            }
            
            Eegeo::Camera::GlobeCamera::GlobeCameraTouchController& InteriorsExplorerModule::GetTouchController() const
            {
                return *m_pGlobeCameraTouchController;
            }
            
//            InteriorExplorerUserInteractionModel& InteriorsExplorerModule::GetInteriorsExplorerUserInteractionModel() const
//            {
//                return *m_pUserInteractionModel;
//            }
        }
}
