// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "JumpPointsExample.h"
#include "VectorMath.h"
#include "LatLongAltitude.h"
#include "CatmullRomSpline.h"
#include "CameraSplinePlaybackController.h"
#include "ResourceCeilingProvider.h"
#include "GlobeCameraController.h"
#include "EegeoWorld.h"
#include "EarthConstants.h"
#include "ScreenProperties.h"
#include "InteriorVisibilityUpdater.h"
#include "InteriorsExplorerModel.h"
#include "IVRHeadTracker.h"
#include "CameraHelpers.h"
#include "Modules/UI/Animations/AnimationsController.h"
#include "Modules/UI/Animations/IDv3Animateable.h"
#include "Modules/UI/Animations/Dv3PropertyAnimation.h"
#include "Modules/UI/Animations/AnimationEase.h"
#include "Modules/InteriorsExplorerModule/InteriorsExplorerModule.h"
#include "Modules/WorldMenu/WorldMenuModule.h"
#include "LodRefinementConfig.h"

#include "WorldMenuController.h"
#include "JumpPointConfigData.h"

#define INTERIOR_NEAR_MULTIPLIER 0.025f
#define EXTERIOR_NEAR_MULTIPLIER 0.1f

const float WelcomeMessageFadeSpeed = 0.5f;
const float WelcomeMessageFadeDelay = 4.0f;
const Eegeo::v2 WelcomeMessageSize = Eegeo::v2(48,3);

const float InteriorMenuFloorAngleThreshold = 70.f;
const float InteriorMenuHighPositionAngleThreshold = 100.f;

const float WelcomeNoteDistanceFromCamera = 50.f;

const float InteriorEntryDistance = 250.f;

#include "Logger.h"

namespace Examples
{
    void DeleteJumpPoint(Eegeo::UI::JumpPoints::JumpPoint* pJumpPoint)
    {
        Eegeo_DELETE pJumpPoint;
    }

    Eegeo::UI::JumpPoints::JumpPoint* GetJumpPointFromConfigData(const ApplicationConfig::JumpPointConfigData& jumpPointData, const std::string& spriteSheetPath, const Eegeo::v2& spriteSheetSize, bool isInInterior)
    {
        Eegeo::v2 outMin;
        Eegeo::v2 outMax;
        Eegeo::UI::CalculateUV(spriteSheetSize, jumpPointData.GetIconID(), outMin, outMax);

        Eegeo::UI::JumpPoints::TJumpPointsVector jpVector;

        for (ApplicationConfig::TJumpPointVector::const_iterator it = jumpPointData.GetChildJumpPoints().begin(); it != jumpPointData.GetChildJumpPoints().end(); ++it)
        {
            jpVector.push_back(GetJumpPointFromConfigData(*it, spriteSheetPath, spriteSheetSize, isInInterior));
        }

        Eegeo::UI::JumpPoints::JumpPoint* pJumpPoint = Eegeo_NEW(Eegeo::UI::JumpPoints::JumpPoint)(jumpPointData.GetID(),
                                                                                                  jumpPointData.GetJumpPointLocation(),
                                                                                                  spriteSheetPath,
                                                                                                  Eegeo::v2(jumpPointData.GetSize(),jumpPointData.GetSize()),
                                                                                                  outMin,
                                                                                                  outMax,
                                                                                                  isInInterior
                                                                                                  );
        pJumpPoint->SetChildJumpPoints(jpVector);
        return pJumpPoint;
    }

    JumpPointsExample::JumpPointsExample(Eegeo::EegeoWorld& eegeoWorld,
                                         Eegeo::Streaming::ResourceCeilingProvider& resourceCeilingProvider,
                                         Eegeo::Camera::GlobeCamera::GlobeCameraController* pCameraController,
                                         const Eegeo::Rendering::ScreenProperties& initialScreenProperties,
                                         Eegeo::UI::IUIQuadFactory& quadFactory,
                                         Eegeo::UI::IUIInteractionObservable& uiInteractionObservable,
                                         const Eegeo::Config::DeviceSpec& deviceSpecs,
                                         InteriorsExplorer::IInteriorsExplorerModule& interiorsExplorerModule,
                                         Eegeo::UI::Animations::AnimationsController& animationsController,
                                         Eegeo::UI::WorldMenu::WorldMenuModule& worldMenuModule,
                                         WorldMenuLoader::SdkModel::WorldMenuLoaderModel& menuLoader,
                                         IVRHeadTracker& headTracker,
                                         const Examples::ApplicationConfig::ApplicationConfiguration& appConfig)
    : m_world(eegeoWorld)
    , m_uiQuadFactory(quadFactory)
    , m_uiInteractionObservable(uiInteractionObservable)
    , m_renderableFilters(eegeoWorld.GetRenderingModule().GetRenderableFilters())
    , m_interiorsExplorerModule(interiorsExplorerModule)
    , m_animationsController(animationsController)
    , m_worldMenuModule(worldMenuModule)
    , m_menuLoader(menuLoader)
    , m_headTracker(headTracker)
    , m_appConfig(appConfig)
    , m_onWestPortEntryButtonCallback(this, &JumpPointsExample::OnWestportInteriorButtonSelected)
    , m_onInteriorFloorChanged(this, &JumpPointsExample::OnInteriorFloorChanged)
    , m_onJumpPointSelected(this, &JumpPointsExample::OnJumpPointSelected)
    , m_locationChangedCallback(this, &JumpPointsExample::OnLocationChanged)
    , m_interiorMenuItemSelected(this, &JumpPointsExample::OnFloorMenuItemSelected)
    , m_isInInterior(false)
    , m_isAtFloorLevel(false)
    , m_placeNameController(m_world.GetMapModule().GetPlaceNamesPresentationModule().GetPlaceNamesController())
    , m_pFloorSwitchCameraAnimator(NULL)
    , m_pInteriorCameraAnimationPositionProvider(NULL)
    , m_pInteriorDistanceVisibilityUpdater(NULL)
    , m_shouldAnimateFloor(false)
    , m_shouldMakeInteriorMarkerVisible(false)
    , m_targetFloor(0)
    {
        
        NotifyScreenPropertiesChanged(initialScreenProperties);
        Eegeo::m44 projectionMatrix = Eegeo::m44(pCameraController->GetRenderCamera().GetProjectionMatrix());
        m_pVRCameraController = Eegeo_NEW(Eegeo::VR::JumpPointsCameraController)(initialScreenProperties.GetScreenWidth(),
                                                                                 initialScreenProperties.GetScreenHeight(),
                                                                                 0.03f);
        m_pVRCameraController->GetCamera().SetProjectionMatrix(projectionMatrix);

        m_pVRCameraStreamingController = Eegeo_NEW(Eegeo::VRCamera::VRCameraStreamingController)(*m_pVRCameraController,
                                                                                                 resourceCeilingProvider,
                                                                                                 Eegeo::Config::LodRefinementConfig::GetLodRefinementAltitudesForDeviceSpec(deviceSpecs),
                                                                                                 m_world.GetMapModule().GetEnvironmentFlatteningService());
        
        m_pUIRenderableFilter = Eegeo_NEW(Eegeo::UI::UIRenderableFilter)();
        m_renderableFilters.AddRenderableFilter(*m_pUIRenderableFilter);
    }
    
    JumpPointsExample::~JumpPointsExample()
    {
    }
    
    void JumpPointsExample::Start()
    {
        m_isCameraAnimating = false;
        
        m_progressBarConfig.textureFilename = m_appConfig.GazeLoaderTexturePath();
        m_progressBarConfig.frameRate = 12.5f;
        m_progressBarConfig.spriteGridSize = Eegeo::v2(m_appConfig.GazeLoaderNumberOfTilesAlong1Axis(),m_appConfig.GazeLoaderNumberOfTilesAlong1Axis());
        m_progressBarConfig.spriteId = 0;
        m_progressBarConfig.color = Eegeo::v4::One();
        m_progressBarConfig.renderLayer = Eegeo::Rendering::LayerIds::Values::AfterWorld;

        m_pVRCameraController->SetNearMultiplier(INTERIOR_NEAR_MULTIPLIER);
        
        Eegeo::v2 dimension = Eegeo::v2(50,50);
        const Eegeo::v2 size(m_appConfig.JumpPointsSpriteSheetSize(), m_appConfig.JumpPointsSpriteSheetSize());
        
        Eegeo::v2 outMin;
        Eegeo::v2 outMax;
        Eegeo::UI::CalculateUV(size, 0, outMin, outMax);
        dimension = Eegeo::v2(50,50);
        
        m_pJumpPointsModule = Eegeo_NEW(Eegeo::UI::JumpPoints::JumpPointsModule)(*m_pUIRenderableFilter,
                                                                                 m_uiQuadFactory,
                                                                                 m_uiInteractionObservable,
                                                                                 m_progressBarConfig,
                                                                                 m_onJumpPointSelected);
        
        LoadInteriorJumpPoints(m_appConfig.GetInteriorJumpPoints());
        LoadExteriorJumpPoints(m_appConfig.GetExteriorJumpPoints());

        m_pJumpPointSwitcher = Eegeo_NEW(JumpPointsSwitcher)(m_pJumpPointsModule->GetRepository(), m_interiorsExplorerModule, m_exteriorJumpPoints, m_interiorJumpPoints);
        
        Eegeo::UI::CalculateUV(size, 14, outMin, outMax);
        m_pWestPortInteriorButton = Eegeo_NEW(Eegeo::UI::UIProgressButton)(*m_pUIRenderableFilter,
                                                                           m_uiQuadFactory,
                                                                           m_appConfig.JumpPointsSpriteSheet(),
                                                                           m_progressBarConfig,
                                                                           m_onWestPortEntryButtonCallback,
                                                                           dimension / 2.0f,
                                                                           Eegeo::Space::LatLongAltitude::FromDegrees(56.459928, -2.978063, 28.5).ToECEF(),
                                                                           Eegeo::v3::One(),
                                                                           Eegeo::v4::One(),
                                                                           outMin,
                                                                           outMax);
        m_pWestPortInteriorButton->SetItemShouldRender(false);
        m_uiInteractionObservable.RegisterInteractableItem(m_pWestPortInteriorButton);

        m_menuLoader.RegisterLocationChangedCallback(m_locationChangedCallback);
        ChangeLocation(m_menuLoader.GetCurrentSelectedLocation());

        m_pWelcomeNoteViewer = Eegeo_NEW(WelcomeNoteViewer)(m_uiQuadFactory, *m_pUIRenderableFilter);

        m_interiorsExplorerModule.RegisterMenuItemGazedCallback(m_interiorMenuItemSelected);

        m_pInteriorCameraAnimationPositionProvider = Eegeo_NEW(InteriorCameraAnimationPositionProvider)(m_world.GetMapModule().GetInteriorsPresentationModule().GetInteriorViewModel(),
                                                                                                        m_world.GetMapModule().GetEnvironmentFlatteningService());
        const Eegeo::dv3 cameraInteriorBasePos = Eegeo::Space::LatLongAltitude::FromDegrees(56.459809, -2.977735, 40).ToECEF();
        m_pFloorSwitchCameraAnimator = Eegeo_NEW(FloorSwitchCameraAnimator)(m_interiorsExplorerModule, *m_pVRCameraController, *m_pInteriorCameraAnimationPositionProvider, cameraInteriorBasePos);

        m_pInteriorDistanceVisibilityUpdater = Eegeo_NEW(InteriorsExplorer::InteriorDistanceVisibilityUpdater)(m_interiorsExplorerModule, m_world.GetMapModule().GetInteriorsPresentationModule().GetInteriorInteractionModel(), InteriorEntryDistance);
    }
    
    void JumpPointsExample::Suspend()
    {
        Eegeo_DELETE m_pInteriorDistanceVisibilityUpdater;
        Eegeo_DELETE m_pFloorSwitchCameraAnimator;
        Eegeo_DELETE m_pInteriorCameraAnimationPositionProvider;
        Eegeo_DELETE m_pWelcomeNoteViewer;

        if(m_isInInterior)
        {
            m_interiorsExplorerModule.UnregisterVisibilityChangedCallback(m_onInteriorFloorChanged);
        }

        m_interiorsExplorerModule.UnregisterMenuItemGazedCallback(m_interiorMenuItemSelected);
        m_menuLoader.UnregisterLocationChangedCallback(m_locationChangedCallback);

        Eegeo_DELETE m_pJumpPointSwitcher;
        m_uiInteractionObservable.UnRegisterInteractableItem(m_pWestPortInteriorButton);
        Eegeo_DELETE m_pWestPortInteriorButton;
        
        m_pJumpPointsModule->GetRepository().RemoveAllJumpPoints();
        for (TInteriorJumpPointsData::const_iterator itInterior = m_interiorJumpPoints.begin(); itInterior != m_interiorJumpPoints.end(); ++itInterior)
        {
            const TInteriorFloorJumpPointsData& jpFloorData = itInterior->second;

            for (TInteriorFloorJumpPointsData::const_iterator itFloorData = jpFloorData.begin(); itFloorData != jpFloorData.end(); ++itFloorData)
            {
                std::for_each(itFloorData->second.begin(), itFloorData->second.end(), DeleteJumpPoint);
            }
        }
        m_interiorJumpPoints.clear();

        for (TExteriorJumpPointsData::const_iterator itExterior = m_exteriorJumpPoints.begin(); itExterior != m_exteriorJumpPoints.end(); ++itExterior)
        {
            const TJumpPointsDataVector& jpVector = itExterior->second;
            std::for_each(jpVector.begin(), jpVector.end(), DeleteJumpPoint);
        }

        m_exteriorJumpPoints.clear();

        Eegeo_DELETE m_pJumpPointsModule;
        Eegeo_DELETE m_pVRCameraStreamingController;
        Eegeo_DELETE m_pVRCameraController;

        m_renderableFilters.RemoveRenderableFilter(*m_pUIRenderableFilter);
        Eegeo_DELETE m_pUIRenderableFilter;
    }

    Eegeo::Streaming::IStreamingVolume& JumpPointsExample::GetCurrentStreamingVolume(const Eegeo::Modules::Map::MapModule& mapModule) const
    {
        return m_pVRCameraStreamingController->GetStreamingVolume();
    }
    
    void JumpPointsExample::UpdateCardboardProfile(const float cardboardProfile[])
    {
        //9th parameter is eye distance in meters.
        m_pVRCameraController->SetEyeDistance(cardboardProfile[9]/2.0f);
    }
    
    void JumpPointsExample::EarlyUpdate(float dt)
    {
        m_pVRCameraStreamingController->UpdateStreamingVolume();
    }

    void JumpPointsExample::Draw()
    {

    }
    
    void JumpPointsExample::Update(float dt)
    {
        m_pJumpPointsModule->Update(dt);
        
        m_pWestPortInteriorButton->Update(dt);

        m_pWelcomeNoteViewer->Update(dt);

        m_pJumpPointSwitcher->Update(dt);
    }

    void JumpPointsExample::NotifyScreenPropertiesChanged(const Eegeo::Rendering::ScreenProperties& screenProperties)
    {
    }

    void JumpPointsExample::LoadInteriorJumpPoints(const ApplicationConfig::TInteriorJumpPoints& interiorJumpPoints)
    {
        Eegeo::v2 spriteSheetSize(m_appConfig.JumpPointsSpriteSheetSize(), m_appConfig.JumpPointsSpriteSheetSize());

        for (ApplicationConfig::TInteriorJumpPoints::const_iterator itJumpPoints = interiorJumpPoints.begin(); itJumpPoints != interiorJumpPoints.end(); ++itJumpPoints)
        {
            TInteriorFloorJumpPointsData floorData;

            const ApplicationConfig::TInteriorFloorJumpPoints& floorJumpPointsConfigData = (itJumpPoints->second);

            for (ApplicationConfig::TInteriorFloorJumpPoints::const_iterator itFloorJPs = floorJumpPointsConfigData.begin(); itFloorJPs != floorJumpPointsConfigData.end(); ++itFloorJPs)
            {
                TJumpPointsDataVector dataVector;

                const ApplicationConfig::TJumpPointVector& jumpPointConfigVector = itFloorJPs->second;

                for (ApplicationConfig::TJumpPointVector::const_iterator it = jumpPointConfigVector.begin(); it != jumpPointConfigVector.end(); ++it)
                {
                    dataVector.push_back(GetJumpPointFromConfigData(*it, m_appConfig.JumpPointsSpriteSheet(), spriteSheetSize, true));
                }

                floorData[itFloorJPs->first] = dataVector;
            }

            m_interiorJumpPoints[itJumpPoints->first] = floorData;
        }
    }

    void JumpPointsExample::LoadExteriorJumpPoints(const ApplicationConfig::TExteriorJumpPoints &exteriorJumpPoints)
    {
        Eegeo::v2 spriteSheetSize(m_appConfig.JumpPointsSpriteSheetSize(), m_appConfig.JumpPointsSpriteSheetSize());

        for (ApplicationConfig::TExteriorJumpPoints::const_iterator itLocationJPs = exteriorJumpPoints.begin(); itLocationJPs != exteriorJumpPoints.end(); ++itLocationJPs)
        {
            TJumpPointsDataVector dataVector;

            const ApplicationConfig::TJumpPointVector& jumpPointConfigVector = itLocationJPs->second;

            for (ApplicationConfig::TJumpPointVector::const_iterator it = jumpPointConfigVector.begin(); it != jumpPointConfigVector.end(); ++it)
            {
                dataVector.push_back(GetJumpPointFromConfigData(*it, m_appConfig.JumpPointsSpriteSheet(), spriteSheetSize, false));
            }

            m_exteriorJumpPoints[itLocationJPs->first] = dataVector;
        }
    }
    
    Eegeo::Camera::RenderCamera& JumpPointsExample::GetRenderCamera(){
        return m_pVRCameraController->GetCamera();
    }

    void JumpPointsExample::UpdateHeadOrientation(const float headTansform[])
    {
        m_pVRCameraController->UpdateHeadOrientation(headTansform);
    }
    
    Eegeo::Camera::CameraState JumpPointsExample::GetCurrentCameraState() const
    {
        return m_pVRCameraController->GetCameraState();
    }

    const Eegeo::VRCamera::VRCameraState& JumpPointsExample::GetCurrentVRCameraState()
    {
        return m_pVRCameraController->GetVRCameraState();
    }
    
    void JumpPointsExample::MoveCameraToStopPoint(const Eegeo::dv3 &cameraPoint, const Eegeo::dv3 &cameraLookat)
    {
        Eegeo::dv3 point = cameraLookat - cameraPoint;
        Eegeo::Space::EcefTangentBasis basis = Eegeo::Space::EcefTangentBasis(cameraPoint, Eegeo::v3(point.GetX(), point.GetY(), point.GetZ()));
        Eegeo::m33 orientation;
        orientation.SetFromBasis(basis.GetRight(), basis.GetUp(), -basis.GetForward());
        m_pVRCameraController->SetEcefPosition(cameraPoint);
        m_pVRCameraController->SetCameraOrientation(orientation);
        m_headTracker.ResetTracker();
    }
    
    void JumpPointsExample::MoveCameraToStopPoint(const Eegeo::dv3 &cameraPoint, float cameraHeading)
    {
        Eegeo::m33 orientation;
        Eegeo::Space::EcefTangentBasis basis;
        Eegeo::Camera::CameraHelpers::EcefTangentBasisFromPointAndHeading(cameraPoint, cameraHeading, basis);
        orientation.SetFromBasis(basis.GetRight(), basis.GetUp(), -basis.GetForward());
        m_pVRCameraController->SetEcefPosition(cameraPoint);
        m_pVRCameraController->SetCameraOrientation(orientation);
        m_headTracker.ResetTracker();
    }
    
    void JumpPointsExample::OnWestportInteriorButtonSelected()
    {
        
        if(m_pWestPortInteriorButton->GetColor().w < 0.9f)
            return;
        
        ShowInteriors();
        
        Eegeo::dv3 cameraPoint = Eegeo::Space::LatLongAltitude::FromDegrees(56.459809, -2.977735, 40).ToECEF();
        
        m_animationsController.RemoveAnimationsForTag(0);
        Eegeo::UI::Animations::Dv3PropertyAnimation* animation = Eegeo_NEW(Eegeo::UI::Animations::Dv3PropertyAnimation)(*m_pVRCameraController, this, m_pVRCameraController->GetCamera().GetEcefLocation(), cameraPoint, 5.0f, &Eegeo::UI::AnimationEase::EaseInOutCubic);
        animation->SetTag(0);
        m_animationsController.AddAnimation(animation);
        m_interiorsExplorerModule.SetShouldShowExitButton(true);
        m_interiorsExplorerModule.RegisterVisibilityChangedCallback(m_onInteriorFloorChanged);
    }
    
    void JumpPointsExample::ShowInteriors()
    {
        if(m_isInInterior)
            return;
        
        m_isInInterior = true;
        m_pWestPortInteriorButton->SetItemShouldRender(false);
        m_worldMenuModule.SetMenuShouldDisplay(false);
        m_interiorsExplorerModule.SetMenuVisibilityThresholdAngle(InteriorMenuHighPositionAngleThreshold);
        m_interiorsExplorerModule.SetShouldShowExitButton(true);
    }
    
    void JumpPointsExample::HideInteriors()
    {
        if(!m_isInInterior)
            return;
        
        m_isInInterior = false;
        m_interiorsExplorerModule.HideInteriors();
        m_pWestPortInteriorButton->SetItemShouldRender(true);
    }
    
    void JumpPointsExample::OnAnimationProgress(Eegeo::UI::Animations::IAnimation& animation)
    {
        if (m_isInInterior)
        {
            m_pInteriorDistanceVisibilityUpdater->UpdateVisiblity(m_pVRCameraController->GetCameraPosition());
        }

        if (m_shouldMakeInteriorMarkerVisible && animation.GetProgress() > 0.5f)
        {
            m_shouldMakeInteriorMarkerVisible = false;
            m_pWestPortInteriorButton->SetItemShouldRender(true);
        }
    }

    void JumpPointsExample::OnAnimationAdded(Eegeo::UI::Animations::IAnimation& animation)
    {
        m_isCameraAnimating = true;
    }
    
    void JumpPointsExample::OnAnimationRemoved(Eegeo::UI::Animations::IAnimation& animation)
    {
        m_isCameraAnimating = false;

        if (m_shouldAnimateFloor)
        {
            m_shouldAnimateFloor = false;
            int delta = Eegeo::Math::Abs(m_interiorsExplorerModule.GetSelectedFloor() - m_targetFloor);
            m_interiorsExplorerModule.SelectFloor(m_targetFloor, true);

            if(m_interiorsExplorerModule.GetSelectedFloor()>=0)
            {
                m_animationsController.RemoveAnimationsForTag(0);
                m_isAtFloorLevel = false;
                m_interiorsExplorerModule.SetMenuVisibilityThresholdAngle(InteriorMenuHighPositionAngleThreshold);
                m_interiorsExplorerModule.SetShouldShowExitButton(true);
                AnimateCameraToInteriorFloor(m_interiorsExplorerModule.GetSelectedFloor(), delta, &Eegeo::UI::AnimationEase::Linear);
            }
        }
    }
    
    void JumpPointsExample::OnJumpPointSelected(Eegeo::UI::JumpPoints::JumpPoint& jumpPoint)
    {
        
        if(m_isCameraAnimating)
            return;
        
        float time = 5.f;
        if(m_isInInterior)
            time = 3.5f;

        m_pWestPortInteriorButton->SetItemShouldRender(!jumpPoint.GetIsInInterior());

        m_pJumpPointsModule->GetController().ResetVisibility();
        jumpPoint.SetVisibilityStatus(false);

        m_animationsController.RemoveAnimationsForTag(0);
        Eegeo::UI::Animations::Dv3PropertyAnimation* animation = Eegeo_NEW(Eegeo::UI::Animations::Dv3PropertyAnimation)(*m_pVRCameraController, this, m_pVRCameraController->GetCamera().GetEcefLocation(), jumpPoint.GetEcefPosition(), time, &Eegeo::UI::AnimationEase::EaseInOutCubic);
        animation->SetTag(0);
        m_animationsController.AddAnimation(animation);

        if (m_isInInterior && !m_isAtFloorLevel)
        {
            m_pJumpPointSwitcher->SwitchJumpPoints(jumpPoint.GetChildJumpPoints(), time/2.f);
        }

        m_isAtFloorLevel = m_isInInterior;

        if (m_isAtFloorLevel)
        {
            m_interiorsExplorerModule.SetMenuVisibilityThresholdAngle(InteriorMenuFloorAngleThreshold);
            m_placeNameController.SetTargetAlpha(0.f);
            m_interiorsExplorerModule.SetShouldShowExitButton(false);
        }
        else
        {
            m_placeNameController.SetTargetAlpha(1.f);
            m_interiorsExplorerModule.SetMenuVisibilityThresholdAngle(InteriorMenuHighPositionAngleThreshold);
            m_interiorsExplorerModule.SetShouldShowExitButton(true);
        }
    }
    
    void JumpPointsExample::OnInteriorFloorChanged()
    {
    }

    void JumpPointsExample::ChangeLocation(const std::string &location)
    {
        const ApplicationConfig::WorldLocationData& currentLocation = m_appConfig.GetLocations().at(location);
        m_pVRCameraController->SetStartLatLongAltitude(currentLocation.GetLocationCameraPosition(), currentLocation.GetOrientation());
        m_pJumpPointSwitcher->SwitchLocation(location);
        m_pWestPortInteriorButton->SetItemShouldRender(currentLocation.GetLocationID()!=0);
    }

    void JumpPointsExample::OnLocationChanged(std::string &location)
    {
        ChangeLocation(location);
        if(m_appConfig.GetLocations().at(location).GetLocationID()!=0)
        {
            m_pWelcomeNoteViewer->ShowWelcomeNote(m_appConfig.GetLocations().at(location).GetWelcomeMessage(), WelcomeMessageFadeSpeed, WelcomeMessageFadeDelay, WelcomeMessageSize);
            Eegeo::v3 forward = Eegeo::v3::Cross(m_pVRCameraController->GetOrientation().GetRow(0), m_pVRCameraController->GetCameraPosition().ToSingle().Norm());
            Eegeo::dv3 position(m_pVRCameraController->GetCameraPosition() + (forward*WelcomeNoteDistanceFromCamera));
            m_pWelcomeNoteViewer->SetPosition(position);
        }
        
    }

    void JumpPointsExample::OnFloorMenuItemSelected(InteriorsExplorer::InteriorMenu::InteriorMenuItem& menuItem)
    {
        if (menuItem.GetId() < 0)
        {
            if (m_isAtFloorLevel)
            {
                AnimateCameraToInteriorFloor(m_interiorsExplorerModule.GetSelectedFloor(), 3.5f, &Eegeo::UI::AnimationEase::EaseInOutCubic);

                m_isAtFloorLevel = false;
                m_interiorsExplorerModule.SetMenuVisibilityThresholdAngle(InteriorMenuHighPositionAngleThreshold);
                m_interiorsExplorerModule.SetShouldShowExitButton(true);
                m_pJumpPointSwitcher->ReloadJumpPoints();
            }
            else
            {
                m_isAtFloorLevel = false;
                m_isInInterior = false;

                m_shouldMakeInteriorMarkerVisible = true;
                m_worldMenuModule.SetMenuShouldDisplay(true);

                Eegeo::dv3 cameraPoint = Eegeo::Space::LatLongAltitude::FromDegrees(56.459156, -2.975320, 300).ToECEF();

                m_animationsController.RemoveAnimationsForTag(0);
                Eegeo::UI::Animations::Dv3PropertyAnimation* animation = Eegeo_NEW(Eegeo::UI::Animations::Dv3PropertyAnimation)(*m_pVRCameraController, this, m_pVRCameraController->GetCamera().GetEcefLocation(), cameraPoint, 3.5f, &Eegeo::UI::AnimationEase::EaseInOutExpo);
                animation->SetTag(0);
                m_animationsController.AddAnimation(animation);
                m_interiorsExplorerModule.UnregisterVisibilityChangedCallback(m_onInteriorFloorChanged);
                m_interiorsExplorerModule.HideInteriors();
            }
        }
        else
        {
            if (m_isAtFloorLevel)
            {
                AnimateCameraToInteriorFloor(m_interiorsExplorerModule.GetSelectedFloor(), 3.5f, &Eegeo::UI::AnimationEase::EaseInOutCubic);

                m_isAtFloorLevel = false;
                m_interiorsExplorerModule.SetMenuVisibilityThresholdAngle(InteriorMenuHighPositionAngleThreshold);
                m_interiorsExplorerModule.SetShouldShowExitButton(true);
                m_pJumpPointSwitcher->ReloadJumpPoints();
                m_shouldAnimateFloor = true;
                m_targetFloor = menuItem.GetId();
                m_interiorsExplorerModule.SetMenuFloorItemSelected(m_targetFloor);
            }
            else
            {
                int delta = Eegeo::Math::Abs(m_interiorsExplorerModule.GetSelectedFloor() - menuItem.GetId());
                m_interiorsExplorerModule.SelectFloor(menuItem.GetId(), true);

                if(m_interiorsExplorerModule.GetSelectedFloor()>=0)
                {
                    m_isAtFloorLevel = false;
                    m_interiorsExplorerModule.SetMenuVisibilityThresholdAngle(InteriorMenuHighPositionAngleThreshold);
                    m_interiorsExplorerModule.SetShouldShowExitButton(true);
                    AnimateCameraToInteriorFloor(m_interiorsExplorerModule.GetSelectedFloor(), delta, &Eegeo::UI::AnimationEase::Linear);
                }
            }
        }

        m_placeNameController.SetTargetAlpha(1.f);
        m_pJumpPointsModule->GetController().ResetVisibility();
    }

    void JumpPointsExample::AnimateCameraToInteriorFloor(int floor, float time, float (*pEaseFunction)(float, float, float))
    {
        Eegeo::dv3 westportHouseCameraPoint = Eegeo::Space::LatLongAltitude::FromDegrees(56.459809, -2.977735, 40+(5*floor)).ToECEF();
        m_animationsController.RemoveAnimationsForTag(0);
        Eegeo::UI::Animations::Dv3PropertyAnimation* animation = Eegeo_NEW(Eegeo::UI::Animations::Dv3PropertyAnimation)(*m_pVRCameraController, this, m_pVRCameraController->GetCameraPosition(), westportHouseCameraPoint, time, pEaseFunction);
        animation->SetTag(0);
        m_animationsController.AddAnimation(animation);
    }

}
