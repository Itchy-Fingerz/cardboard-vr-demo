// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#ifndef __ExampleApp__JumpPointsExample__
#define __ExampleApp__JumpPointsExample__

#include <vector>

#include "JumpPointsCameraController.h"
#include "IExample.h"
#include "Camera.h"
#include "Geometry.h"
#include "Streaming.h"
#include "GlobeCamera.h"
#include "JumpPointsSwitcher.h"
#include "ApplicationConfig.h"
#include "WorldMenuLoaderModel.h"
#include "WelcomeNoteViewer.h"
#include "PlaceNamesController.h"
#include "Modules/UI/UIInteraction/UIInteractionController.h"
#include "Modules/UI/IUICameraProvider.h"
#include "Modules/UI/UIQuad/IUIQuadFactory.h"
#include "Modules/UI/UIRenderableFilter.h"
#include "Modules/UI/UIProgressButton.h"

#include "Modules/JumpPoints/JumpPoint.h"
#include "Modules/JumpPoints/JumpPointsModule.h"
#include "Modules/JumpPoints/JumpPointRepository.h"

#include "Modules/DeadZoneMenu/DeadZoneMenuItemRepository.h"
#include "Modules/DeadZoneMenu/DeadZoneMenuItem.h"

#include "Modules/UI/Animations/AnimationsController.h"
#include "Modules/UI/Animations/IDv3Animateable.h"
#include "Modules/UI/Animations/IAnimationObserver.h"
#include "Modules/WorldMenu/WorldMenuModule.h"

#include "Modules/InteriorsExplorerModule/InteriorsExplorerModule.h"
#include "InteriorCameraAnimationPositionProvider.h"
#include "FloorSwitchCameraAnimator.h"
#include "InteriorDistanceVisibilityUpdater.h"
#include "VRRenderCamera.h"

namespace Examples
{
    /*!
     *  JumpPointsExample demonstrates a camera controller that can animate the camera along a spline
     */
    class JumpPointsExample : public IExample, Eegeo::NonCopyable, public Eegeo::UI::Animations::IAnimationObserver
    {
    private:

        Eegeo::EegeoWorld& m_world;
        Eegeo::UI::IUIQuadFactory& m_uiQuadFactory;
        Eegeo::UI::IUIInteractionObservable& m_uiInteractionObservable;
        Eegeo::UI::IUICameraProvider& m_uiCameraProvider;

        bool m_isAtFloorLevel;
        InteriorCameraAnimationPositionProvider* m_pInteriorCameraAnimationPositionProvider;
        FloorSwitchCameraAnimator* m_pFloorSwitchCameraAnimator;

        InteriorsExplorer::InteriorDistanceVisibilityUpdater* m_pInteriorDistanceVisibilityUpdater;
        
        Eegeo::Rendering::RenderableFilters& m_renderableFilters;
        InteriorsExplorer::IInteriorsExplorerModule& m_interiorsExplorerModule;
        Eegeo::UI::Animations::AnimationsController& m_animationsController;
        IVRHeadTracker& m_headTracker;
        const Examples::ApplicationConfig::ApplicationConfiguration& m_appConfig;
        WorldMenuLoader::SdkModel::WorldMenuLoaderModel& m_menuLoader;
        Eegeo::UI::UIRenderableFilter* m_pUIRenderableFilter;
        Eegeo::Resources::PlaceNames::PlaceNamesController& m_placeNameController;

        Eegeo::Helpers::TCallback0<JumpPointsExample> m_onWestPortEntryButtonCallback;

        Eegeo::VR::JumpPointsCameraController* m_pVRCameraController;

        TExteriorJumpPointsData m_exteriorJumpPoints;
        TInteriorJumpPointsData m_interiorJumpPoints;
        Eegeo::UI::JumpPoints::JumpPointsModule* m_pJumpPointsModule;
        Eegeo::Helpers::TCallback1<JumpPointsExample, Eegeo::UI::JumpPoints::JumpPoint&> m_onJumpPointSelected;

        Eegeo::UI::UIProgressBarConfig m_progressBarConfig;
        Eegeo::UI::UIProgressButton *m_pWestPortInteriorButton;
        Eegeo::UI::WorldMenu::WorldMenuModule& m_worldMenuModule;

        JumpPointsSwitcher* m_pJumpPointSwitcher;
        Eegeo::Helpers::TCallback0<JumpPointsExample> m_onInteriorFloorChanged;
        void OnInteriorFloorChanged();

        void LoadInteriorJumpPoints(const ApplicationConfig::TInteriorJumpPoints& interiorJumpPoints);
        void LoadExteriorJumpPoints(const ApplicationConfig::TExteriorJumpPoints& exteriorJumpPoints);

        void MoveCameraToStopPoint(const Eegeo::dv3& cameraPoint, const Eegeo::dv3& cameraLookat);
        void MoveCameraToStopPoint(const Eegeo::dv3 &cameraPoint, float cameraHeading);
        bool m_isInInterior;
        bool m_isCameraAnimating;
        bool m_shouldAnimateFloor;
        bool m_shouldMakeInteriorMarkerVisible;
        int m_targetFloor;

        WelcomeNoteViewer* m_pWelcomeNoteViewer;

        Eegeo::Helpers::TCallback1<JumpPointsExample, std::string&> m_locationChangedCallback;
        void OnLocationChanged(std::string& location);

        Eegeo::Helpers::TCallback1<JumpPointsExample, InteriorsExplorer::InteriorMenu::InteriorMenuItem&> m_interiorMenuItemSelected;

        void OnFloorMenuItemSelected(InteriorsExplorer::InteriorMenu::InteriorMenuItem& menuItem);
        
    public:
        
        JumpPointsExample(Eegeo::EegeoWorld& eegeoWorld,
                          Eegeo::Streaming::ResourceCeilingProvider& resourceCeilingProvider,
                          Eegeo::Camera::GlobeCamera::GlobeCameraController* pCameraController,
                          const Eegeo::Rendering::ScreenProperties& initialScreenProperties,
                          Eegeo::UI::IUIQuadFactory& quadFactory,
                          Eegeo::UI::IUIInteractionObservable& uiInteractionObservable,
                          Eegeo::UI::IUICameraProvider& uiCameraProvider,
                          InteriorsExplorer::IInteriorsExplorerModule& interiorsExplorerModule,
                          Eegeo::UI::Animations::AnimationsController& animationsController,
                          Eegeo::UI::WorldMenu::WorldMenuModule& worldMenuModule,
                          WorldMenuLoader::SdkModel::WorldMenuLoaderModel& menuLoader,
                          IVRHeadTracker& headTracker,
                          const Examples::ApplicationConfig::ApplicationConfiguration& appConfig);
        
        virtual ~JumpPointsExample();
        
        static std::string GetName()
        {
            return "JumpPointsExample";
        }
        std::string Name() const
        {
            return GetName();
        }
        
        void Start();
        void OrientationUpdate();
        void EarlyUpdate(float dt);
        void Update(float dt);
        void UpdateHeadOrientation(const float headTansform[]);
        void PreWorldDraw() { }
        void Draw();
        void Suspend();
        
        void UpdateCardboardProfile(const float cardboardProfile[]);
        
        virtual Eegeo::Camera::RenderCamera& GetRenderCamera();
        virtual Eegeo::Camera::CameraState GetCurrentCameraState() const;
        virtual const Eegeo::VRCamera::VRCameraState& GetCurrentVRCameraState();
        
        virtual void NotifyScreenPropertiesChanged(const Eegeo::Rendering::ScreenProperties& screenProperties);
        
        virtual void OnAnimationAdded(Eegeo::UI::Animations::IAnimation& animation);
        virtual void OnAnimationProgress(Eegeo::UI::Animations::IAnimation& animation);
        virtual void OnAnimationRemoved(Eegeo::UI::Animations::IAnimation& animation);
        
        void NotifyViewNeedsLayout() {}
        
        void Event_TouchRotate 			(const AppInterface::RotateData& data) { }
        void Event_TouchRotate_Start	(const AppInterface::RotateData& data) { }
        void Event_TouchRotate_End 		(const AppInterface::RotateData& data) { }
        
        void Event_TouchPinch 			(const AppInterface::PinchData& data) { }
        void Event_TouchPinch_Start 	(const AppInterface::PinchData& data) { }
        void Event_TouchPinch_End 		(const AppInterface::PinchData& data) { }
        
        void Event_TouchPan				(const AppInterface::PanData& data) { }
        void Event_TouchPan_Start		(const AppInterface::PanData& data) { }
        void Event_TouchPan_End 		(const AppInterface::PanData& data) { }
        
        void Event_TouchTap 			(const AppInterface::TapData& data) { }
        void Event_TouchDoubleTap		(const AppInterface::TapData& data) { }
        void Event_TouchDown 			(const AppInterface::TouchData& data) { }
        void Event_TouchMove 			(const AppInterface::TouchData& data) { }
        void Event_TouchUp 				(const AppInterface::TouchData& data) { }
        
        void ShowInteriors();
        void HideInteriors();
        
        void OnWestportInteriorButtonSelected();
        void OnJumpPointSelected(Eegeo::UI::JumpPoints::JumpPoint& jumpPoint);

        void ChangeLocation(const std::string& location);
        void AnimateCameraToInteriorFloor(int floor, float time, float (*pEaseFunction)(float, float, float));
    };
}

#endif /* defined(__ExampleApp__JumpPointsExample__) */
