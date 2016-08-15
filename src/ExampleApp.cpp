// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "ExampleApp.h"
#include "GlobeCameraTouchController.h"
#include "RenderCamera.h"
#include "GlobeCameraController.h"
#include "GlobeCameraInterestPointProvider.h"
#include "GlobeCameraController.h"
#include "CameraHelpers.h"
#include "LatLongAltitude.h"
#include "ScreenProperties.h"

// Modules
#include "MapModule.h"
#include "TerrainModelModule.h"
#include "GlobalFogging.h"
#include "CityThemesModule.h"
#include "CityThemesService.h"
#include "CameraFrustumStreamingVolume.h"
#include "LodRefinementConfig.h"
#include "QuadTreeCube.h"
#include "StreamingVolumeController.h"

//example factories
#include "BuildingHighlightExampleFactory.h"
#include "CameraTransitionExampleFactory.h"
#include "ControlCityThemeExampleFactory.h"
#include "DebugPrimitiveRenderingExampleFactory.h"
#include "DynamicText3DExampleFactory.h"
#include "EnvironmentRayCasterExampleFactory.h"
#include "EnvironmentFlatteningExampleFactory.h"
#include "EnvironmentNotifierExampleFactory.h"
#include "FileIOExampleFactory.h"
#include "HeatmapExampleFactory.h"
#include "LoadModelExampleFactory.h"
#include "ModifiedRenderingExampleFactory.h"
#include "NavigationGraphExampleFactory.h"
#include "Pick3DObjectExampleFactory.h"
#include "PinOverModelExampleFactory.h"
#include "PinsExampleFactory.h"
#include "PODAnimationExampleFactory.h"
#include "PolyChartExampleFactory.h"
#include "RenderToTextureExampleFactory.h"
#include "ResourceSpatialQueryExampleFactory.h"
#include "RouteDrawingExampleFactory.h"
#include "RouteSimulationAnimationExampleFactory.h"
#include "RouteThicknessPolicyExampleFactory.h"
#include "ScreenPickExampleFactory.h"
#include "ScreenUnprojectExampleFactory.h"
#include "SingleCityExampleFactory.h"
#include "StencilAreaExampleFactory.h"
#include "ToggleTrafficExampleFactory.h"
#include "TrafficCongestionExampleFactory.h"
#include "WebRequestExampleFactory.h"

#include "Examples/VRCameraSpline/VRCameraSplineExampleFactory.h"
#include "Examples/JumpPoints/JumpPointsExampleFactory.h"
#include "CameraSplineExampleFactory.h"
#include "ReadHeadingExampleFactory.h"
#include "FireworksExampleFactory.h"
#include "MeshExampleFactory.h"
#include "GeofenceExampleFactory.h"
#include "BuildingSelectionExampleFactory.h"
#include "RemoveMapLayerExampleFactory.h"

#include "InteriorsCameraController.h"
#include "InteriorsCameraControllerFactory.h"
#include "InteriorsPresentationModule.h"
#include "InteriorsModelModule.h"
#include "InteriorsCameraControllerFactory.h"
#include "GlobeCameraControllerFactory.h"
#include "LatLongAltitude.h"

#include "Modules/DeadZoneMenu/DeadZoneMenuModule.h"

#include "WorldMenu/WorldMenuModule.h"
#include "WorldMenu/WorldMenuController.h"
#include "WorldMenu/WorldMenuItem.h"

#include "Modules/UI/UIQuad/IUIQuadFactory.h"
#include "Modules/UI/UIQuad/UIQuadFactory.h"

#include "Modules/UI/UIGaze/UIGazeView.h"
#include "Modules/UI/UIQuad/UIGeometryHelpers.h"
#include "Modules/UI/UIInteraction/UIInteractionController.h"
#include "Modules/VRDistortionModule/VRCardboardDeviceProfile.h"

#include "Modules/UI/Animations/AnimationsController.h"
#include "Logger.h"
namespace
{
    Eegeo::Rendering::LoadingScreen* CreateLoadingScreen(const Eegeo::Rendering::ScreenProperties& screenProperties,
                                                         const Eegeo::Modules::Core::RenderingModule& renderingModule,
                                                         const Eegeo::Modules::IPlatformAbstractionModule& platformAbstractionModule)
    {
        Eegeo::Rendering::LoadingScreenConfig loadingScreenConfig;
        loadingScreenConfig.loadingBarBackgroundColor = Eegeo::v4(0.45f, 0.7f, 1.0f, 1.0f);
        loadingScreenConfig.fadeOutDurationSeconds = 1.5f;
        loadingScreenConfig.screenWidth = screenProperties.GetScreenWidth();
        loadingScreenConfig.screenHeight = screenProperties.GetScreenHeight();
        loadingScreenConfig.backgroundColor = Eegeo::v4(132.f/255.f, 203.f/255.f, 235.f/255.f, 1.f);
        loadingScreenConfig.loadingBarOffset = Eegeo::v2(0.5f, 0.1f);
        loadingScreenConfig.layout = Eegeo::Rendering::LoadingScreenLayout::Centred;
       
        
        Eegeo::Rendering::LoadingScreen* pLoadingScreen = Eegeo::Rendering::LoadingScreen::Create(
            "SplashScreen-1024x768.png",
            loadingScreenConfig,
            renderingModule.GetShaderIdGenerator(),
            renderingModule.GetMaterialIdGenerator(),
            renderingModule.GetGlBufferPool(),
            renderingModule.GetVertexLayoutPool(),
            renderingModule.GetVertexBindingPool(),
            platformAbstractionModule.GetTextureFileLoader());
            
        return pLoadingScreen;
    }
}


ExampleApp::ExampleApp(Eegeo::EegeoWorld* pWorld,
                       Eegeo::Config::DeviceSpec deviceSpecs,
                       Examples::IExampleControllerView& view,
                       Examples::IVRHeadTracker& headTracker,
                       const Eegeo::Rendering::ScreenProperties& screenProperties,
                       Eegeo::Modules::CollisionVisualizationModule& collisionVisualizationModule,
                       Eegeo::Modules::BuildingFootprintsModule& buildingFootprintsModule,
                       Examples::ApplicationConfig::ApplicationConfiguration& appConfig)
	: m_pCameraControllerFactory(NULL)
	, m_pCameraTouchController(NULL)
	, m_pWorld(pWorld)
    , m_nightTParam(0.0f)
    , m_night(false)
    , m_foggingFar(4000)
    , m_pLoadingScreen(NULL)
    , m_pExampleController(NULL)
    , m_currentClearColor(135.f/255.0f, 206.f/255.0f, 235.f/255.0f)
    , m_startClearColor(0.f/255.f,24.f/255.f,72.f/255.f)
    , m_destClearColor(135.f/255.0f, 206.f/255.0f, 235.f/255.0f)
    , m_screenPropertiesProvider(screenProperties)
    , m_toggleDayNightClickedCallback(this, &ExampleApp::ToggleNight)
    , m_splineExampleButtonClickedCallback(this, &ExampleApp::LoadSplineExample)
    , m_jumpPointExampleButtonClickedCallback(this, &ExampleApp::LoadJumpPointExample)
    , m_worldMenuItemGazeCallback(this, &ExampleApp::OnWorldMenuItemGazed)
    , m_getJumpPointStartPositionOrientation(this, &ExampleApp::GetJumpPointStartPositionOrientation)
    , m_locationChangedCallback(this, &ExampleApp::OnLocationChanged)
{
	Eegeo::EegeoWorld& eegeoWorld = *pWorld;

	Eegeo::Camera::GlobeCamera::GlobeCameraTouchControllerConfiguration touchControllerConfig = Eegeo::Camera::GlobeCamera::GlobeCameraTouchControllerConfiguration::CreateDefault();

	m_pCameraTouchController = new Eegeo::Camera::GlobeCamera::GlobeCameraTouchController(touchControllerConfig, m_screenPropertiesProvider.GetScreenProperties());

	const bool useLowSpecSettings = false;
	Eegeo::Camera::GlobeCamera::GlobeCameraControllerConfiguration globeCameraControllerConfig = Eegeo::Camera::GlobeCamera::GlobeCameraControllerConfiguration::CreateDefault(useLowSpecSettings);

    Eegeo::Modules::Map::MapModule& mapModule = eegeoWorld.GetMapModule();
    Eegeo::Modules::Map::Layers::TerrainModelModule& terrainModelModule = eegeoWorld.GetTerrainModelModule();
    
    const bool twoFingerPanTiltEnabled = true;
    const float interestPointLatitudeDegrees = 37.7858f;
    const float interestPointLongitudeDegrees = -122.401f;
    const float interestPointAltitudeMeters = 2.7f;
    const float cameraControllerOrientationDegrees = 0.0f;
    const float cameraControllerDistanceFromInterestPointMeters = 1781.0f;
    
    
    m_pStreamingVolume = Eegeo_NEW(Eegeo::Streaming::CameraFrustumStreamingVolume)(mapModule.GetResourceCeilingProvider(),
                                                                                   Eegeo::Config::LodRefinementConfig::GetLodRefinementAltitudesForDeviceSpec(deviceSpecs),
                                                                                   Eegeo::Streaming::QuadTreeCube::MAX_DEPTH_TO_VISIT,
                                                                                   mapModule.GetEnvironmentFlatteningService());
    
    m_pStreamingVolume->setDeepestLevelForAltitudeLodRefinement(11);
    m_pStreamingVolume->SetForceMaximumRefinement(true);
    
    m_pCameraControllerFactory = new Examples::DefaultCameraControllerFactory(
                                                                    terrainModelModule,
                                                                    mapModule,
                                                                    *m_pCameraTouchController,
                                                                    m_screenPropertiesProvider,
                                                                    globeCameraControllerConfig,
                                                                    twoFingerPanTiltEnabled,
                                                                    interestPointLatitudeDegrees,
                                                                    interestPointLongitudeDegrees,
                                                                    interestPointAltitudeMeters,
                                                                    cameraControllerOrientationDegrees,
                                                                    cameraControllerDistanceFromInterestPointMeters);
    
    m_pAnimationController = Eegeo_NEW(Eegeo::UI::Animations::AnimationsController());
    
    m_pLoadingScreen = CreateLoadingScreen(screenProperties, eegeoWorld.GetRenderingModule(), eegeoWorld.GetPlatformAbstractionModule());
    
    Eegeo::Modules::Map::Layers::InteriorsPresentationModule& interiorsPresentationModule = mapModule.GetInteriorsPresentationModule();
    
    
    m_pExampleController = new Examples::ExampleController(*m_pWorld,
                                                           view,
                                                           *m_pCameraControllerFactory,
                                                           *m_pCameraTouchController);


    Eegeo::Modules::Core::RenderingModule& renderingModule = m_pWorld->GetRenderingModule();
    m_pVRDistortion = Eegeo_NEW(Eegeo::VR::Distortion::VRDistortionModule)(m_screenPropertiesProvider.GetScreenProperties(),
                                                renderingModule.GetVertexLayoutPool(),
                                                renderingModule.GetVertexBindingPool(),
                                                renderingModule.GetShaderIdGenerator(),
                                                renderingModule.GetMaterialIdGenerator(),
                                                renderingModule.GetRenderableFilters(),
                                             renderingModule.GetGlBufferPool());
    m_pVRDistortion->Initialize();
    
    m_pVRSkybox = Eegeo_NEW(Eegeo::Skybox::SkyboxModule)(renderingModule,
                                                                      renderingModule.GetGlBufferPool(),
                                                                      renderingModule.GetVertexBindingPool(),
                                                                      renderingModule.GetVertexLayoutPool(),
                                                                      renderingModule.GetRenderableFilters()
                                                                      );
    m_pVRSkybox->Start();
    
    Eegeo::v2 size(4,4);
    Eegeo::v2 outMin;
    Eegeo::v2 outMax;
    Eegeo::UI::CalculateUV(size, 1, outMin, outMax);
    
    m_pUIRenderableFilter = Eegeo_NEW(Eegeo::UI::UIRenderableFilter)();
    renderingModule.GetRenderableFilters().AddRenderableFilter(*m_pUIRenderableFilter);
    
    m_pQuadFactory = Eegeo_NEW(Eegeo::UI::UIQuadFactory)(renderingModule, m_pWorld->GetPlatformAbstractionModule().GetTextureFileLoader());

    m_pUIGazeView = new Eegeo::UIGaze::UIGazeView(*m_pQuadFactory, *m_pUIRenderableFilter);
    
    m_pUIInteractionController = Eegeo_NEW(Eegeo::UI::UIInteractionController)(*m_pExampleController, *m_pUIGazeView);
    
    m_progressBarConfig.textureFilename = "mesh_example/gaze_loader.png";
    m_progressBarConfig.frameRate = 4.2f;
    m_progressBarConfig.spriteGridSize = Eegeo::v2(5,5);
    m_progressBarConfig.spriteId = 0;
    m_progressBarConfig.color = Eegeo::v4::One();
    m_progressBarConfig.renderLayer = Eegeo::Rendering::LayerIds::Values::AfterAll;

    m_pInteriorExplorerModule = Eegeo_NEW(InteriorsExplorer::InteriorsExplorerModule)(
                                                                                      *m_pUIRenderableFilter,
                                                                                      *m_pQuadFactory, *
                                                                                      m_pUIInteractionController,
                                                                                      *m_pExampleController,
                                                                                      m_progressBarConfig,
                                                                                      interiorsPresentationModule.GetInteriorInteractionModel(),
                                                                                      interiorsPresentationModule.GetInteriorSelectionModel(),
                                                                                      interiorsPresentationModule.GetInteriorTransitionModel(),
                                                                                      appConfig.JumpPointsSpriteSheet(),
                                                                                      appConfig.JumpPointsSpriteSheetSize());
    
    m_pDeadZoneMenuModule = new Eegeo::UI::DeadZoneMenu::DeadZoneMenuModule(*m_pUIRenderableFilter, *m_pQuadFactory, *m_pUIInteractionController, *m_pExampleController, appConfig.JumpPointsSpriteSheet(), m_progressBarConfig, appConfig.JumpPointsSpriteSheetSize());
    
    m_pMenuItem1 = new Eegeo::UI::DeadZoneMenu::DeadZoneMenuItem(1, 0, m_jumpPointExampleButtonClickedCallback);
    m_pMenuItem2 = new Eegeo::UI::DeadZoneMenu::DeadZoneMenuItem(2, 1, m_toggleDayNightClickedCallback);
    m_pMenuItem3 = new Eegeo::UI::DeadZoneMenu::DeadZoneMenuItem(3, 2, m_splineExampleButtonClickedCallback);
    
    
    m_pWorldMenuModule = Eegeo_NEW(Eegeo::UI::WorldMenu::WorldMenuModule)(*m_pUIRenderableFilter, *m_pQuadFactory, *m_pUIInteractionController,*m_pExampleController, appConfig.JumpPointsSpriteSheet(), m_progressBarConfig, appConfig.JumpPointsSpriteSheetSize());
    m_pWorldMenuModule->SetMenuShouldDisplay(true);
    

    m_pWorldMenuLoaderModel = Eegeo_NEW(Examples::WorldMenuLoader::SdkModel::WorldMenuLoaderModel)(m_pWorldMenuModule->GetRepository(), m_pVRDistortion->GetTransionModel(), appConfig);
    m_pWorldMenuLoaderModel->RegisterLocationChangedCallback(m_locationChangedCallback);
    

    m_worldMenuItemSelected = 1;

    m_pExampleController->RegisterScreenPropertiesProviderVRExample<Examples::VRCameraSplineExampleFactory>(m_screenPropertiesProvider, *m_pInteriorExplorerModule, headTracker, m_pDeadZoneMenuModule->GetRepository());
    m_pExampleController->RegisterJumpPointVRExample<Examples::JumpPointsExampleFactory>(m_screenPropertiesProvider, *m_pQuadFactory, *m_pUIInteractionController, *m_pExampleController, *m_pInteriorExplorerModule, m_pDeadZoneMenuModule->GetRepository(), *m_pAnimationController, *m_pWorldMenuModule, *m_pWorldMenuLoaderModel, headTracker, appConfig);
    
    m_pUIGazeView->HideView();



}

ExampleApp::~ExampleApp()
{
    
    if(m_pLoadingScreen!=NULL)
    {
        Eegeo_DELETE m_pLoadingScreen;
    }
    while(m_pWorldMenuItems.size()>0)
    {
        Eegeo::UI::WorldMenu::WorldMenuItem* menuItem = *m_pWorldMenuItems.begin();
        m_pWorldMenuModule->GetRepository().RemoveWorldMenuItem(menuItem);
        m_pWorldMenuItems.erase(m_pWorldMenuItems.begin());
        Eegeo_DELETE menuItem;
    }

    Eegeo_DELETE m_pQuadFactory;
    Eegeo_DELETE m_pUIGazeView;
    
    Eegeo_DELETE m_pVRSkybox;
    Eegeo_DELETE m_pVRDistortion;
    
    Eegeo_DELETE m_pStreamingVolume;
    Eegeo_DELETE m_pCameraControllerFactory;
    Eegeo_DELETE m_pCameraTouchController;
    Eegeo_DELETE m_pExampleController;

    m_pWorldMenuLoaderModel->UnregisterLocationChangedCallback(m_locationChangedCallback);
    Eegeo_DELETE m_pWorldMenuLoaderModel;

    Eegeo_DELETE m_pWorldMenuModule;

    Eegeo_DELETE m_pDeadZoneMenuModule;
    Eegeo_DELETE m_pMenuItem1;
    Eegeo_DELETE m_pMenuItem2;
    Eegeo_DELETE m_pMenuItem3;
    
    Eegeo_DELETE m_pInteriorExplorerModule;
    Eegeo_DELETE m_pUIInteractionController;
    
    m_pWorld->GetRenderingModule().GetRenderableFilters().RemoveRenderableFilter(*m_pUIRenderableFilter);
    Eegeo_DELETE m_pUIRenderableFilter;
    Eegeo_DELETE m_pAnimationController;
}

void ExampleApp::OnPause()
{
	Eegeo::EegeoWorld& eegeoWorld = World();
	eegeoWorld.OnPause();
}

void ExampleApp::OnResume()
{
	Eegeo::EegeoWorld& eegeoWorld = World();
	eegeoWorld.OnResume();
}


void ExampleApp::Update (float dt, float headTansform[])
{
    
    
    const Eegeo::Rendering::ScreenProperties& screenProperties = m_screenPropertiesProvider.GetScreenProperties();
    Eegeo::Camera::CameraState cameraState(m_pExampleController->GetCurrentCameraState());
    Eegeo::Camera::RenderCamera& renderCamera = m_pExampleController->GetRenderCamera();
    Eegeo::EegeoWorld& eegeoWorld(World());
    
    eegeoWorld.EarlyUpdate(dt);
    
    if(m_pLoadingScreen==NULL || m_pLoadingScreen->IsDismissed())
        m_pExampleController->EarlyUpdate(dt);
    
    std::vector<Eegeo::Geometry::Plane> frustumPlanes(Eegeo::Geometry::Frustum::PLANES_COUNT);
    BuildFrustumPlanesFromViewProjection(frustumPlanes, renderCamera.GetViewProjectionMatrix());
    const double d = renderCamera.GetAltitude() * Eegeo::Streaming::StreamingVolumeController::CAMERA_ALTITUDE_TO_FAR_PLANE_DISTANCE_MULTIPLIER;
    const double cameraFarPlaneD = fmin(fmax(d, Eegeo::Streaming::StreamingVolumeController::MIN_STREAMING_FAR_PLANE_DISTANCE), frustumPlanes[Eegeo::Geometry::Frustum::PLANE_FAR].d);
    frustumPlanes[Eegeo::Geometry::Frustum::PLANE_FAR].d = static_cast<float>(cameraFarPlaneD);
    
//    Workaround: added 100.0f to FOV to load textures for surroundings even when camera is not looking at it to fix interior loading crash.
//    m_pStreamingVolume->updateStreamingVolume(renderCamera.GetEcefLocation(), frustumPlanes, renderCamera.GetFOV());
    m_pStreamingVolume->updateStreamingVolume(renderCamera.GetEcefLocation(), frustumPlanes, renderCamera.GetFOV()+100.0f);
    m_pStreamingVolume->ResetVolume(cameraState.InterestPointEcef());
    
    Eegeo::EegeoUpdateParameters updateParameters(dt,
                                                  cameraState.LocationEcef(),
                                                  cameraState.InterestPointEcef(),
                                                  cameraState.ViewMatrix(),
                                                  cameraState.ProjectionMatrix(),
                                                  *m_pStreamingVolume,
                                                  screenProperties);
    
    eegeoWorld.Update(updateParameters);
    
    if(m_pLoadingScreen==NULL || m_pLoadingScreen->IsDismissed())
    {
        
        m_pWorldMenuModule->Update(dt);
        m_pAnimationController->Update(dt);
        m_pExampleController->Update(dt);
    
        m_pDeadZoneMenuModule->Update(dt);
        m_pUIInteractionController->Update(dt);
    
        Eegeo::v2 center = m_pVRDistortion->GetCardboardProfile().GetScreenMeshCenter(screenProperties.GetScreenWidth(), screenProperties.GetScreenHeight());
        m_pUIInteractionController->Event_ScreenInteractionMoved(center);
        m_pInteriorExplorerModule ->Update(dt);
        m_pWorldMenuLoaderModel->Update(dt);
    }
    
    UpdateNightTParam(dt);
    UpdateFogging();
    UpdateLoadingScreen(dt);
}

void ExampleApp::Draw (float dt, float headTansform[]){
    Eegeo::EegeoWorld& eegeoWorld = World();
    if(eegeoWorld.Validated())
    {
        m_pVRDistortion->BeginRendering();
        DrawLeftEye(dt, headTansform, eegeoWorld);
        m_pVRDistortion->RegisterRenderable();
        DrawRightEye(dt, headTansform, eegeoWorld);
        m_pVRDistortion->UnRegisterRenderable();
    }
    
    DrawLoadingScreen();
    
}

void ExampleApp::DrawLeftEye (float dt, float headTansform[], Eegeo::EegeoWorld& eegeoWorld){
    
    m_pExampleController->PreWorldDraw();
    
    glViewport(0, 0, m_screenPropertiesProvider.GetScreenProperties().GetScreenWidth(), m_screenPropertiesProvider.GetScreenProperties().GetScreenHeight());
    
    Eegeo::Camera::CameraState cameraState(m_pExampleController->GetCurrentLeftCameraState(headTansform));
    Eegeo::EegeoDrawParameters drawParameters(cameraState.LocationEcef(),
                                              cameraState.InterestPointEcef(),
                                              cameraState.ViewMatrix(),
                                              cameraState.ProjectionMatrix(),
                                              m_screenPropertiesProvider.GetScreenProperties());
    
    
    Eegeo::v3 forward(m_pExampleController->GetOrientation().GetRow(2));
    Eegeo::dv3 position(cameraState.LocationEcef() + (forward*50));
    m_pUIGazeView->Update(dt);
    m_pUIGazeView->SetEcefPosition(position);
    
    if(m_pLoadingScreen==NULL || m_pLoadingScreen->IsDismissed()){
        m_pWorldMenuModule->Update(dt);
        m_pInteriorExplorerModule ->Update(dt);
    }
    eegeoWorld.Draw(drawParameters);
    
    m_pExampleController->Draw();
    
    
}

void ExampleApp::DrawRightEye (float dt, float headTansform[], Eegeo::EegeoWorld& eegeoWorld){
    
    m_pExampleController->PreWorldDraw();
    
    glViewport(m_screenPropertiesProvider.GetScreenProperties().GetScreenWidth(), 0, m_screenPropertiesProvider.GetScreenProperties().GetScreenWidth(),m_screenPropertiesProvider.GetScreenProperties().GetScreenHeight());
    
    Eegeo::Camera::CameraState cameraState(m_pExampleController->GetCurrentRightCameraState(headTansform));
    Eegeo::EegeoDrawParameters drawParameters(cameraState.LocationEcef(),
                                               cameraState.InterestPointEcef(),
                                               cameraState.ViewMatrix(),
                                               cameraState.ProjectionMatrix(),
                                               m_screenPropertiesProvider.GetScreenProperties());
    
    Eegeo::v3 forward(m_pExampleController->GetOrientation().GetRow(2));
    Eegeo::dv3 position(cameraState.LocationEcef() + (forward*50));
    m_pUIGazeView->Update(dt);
    m_pUIGazeView->SetEcefPosition(position);
    
    if(m_pLoadingScreen==NULL || m_pLoadingScreen->IsDismissed()){
        m_pWorldMenuModule->Update(dt);
        m_pInteriorExplorerModule ->Update(dt);
    }
    eegeoWorld.Draw(drawParameters);
    
    m_pExampleController->Draw();
    
}


void ExampleApp::DrawLoadingScreen ()
{
    
    if (m_pLoadingScreen != NULL)
    {
        glViewport(0, 0, m_screenPropertiesProvider.GetScreenProperties().GetScreenWidth(), m_screenPropertiesProvider.GetScreenProperties().GetScreenHeight());
        m_pLoadingScreen->Draw();
        glViewport(m_screenPropertiesProvider.GetScreenProperties().GetScreenWidth(), 0, m_screenPropertiesProvider.GetScreenProperties().GetScreenWidth(),m_screenPropertiesProvider.GetScreenProperties().GetScreenHeight());
        m_pLoadingScreen->Draw();
    }
}

void ExampleApp::OnLocationChanged(std::string &location)
{
    m_pAnimationController->ClearAllAnimations();
    if(m_pWorldMenuLoaderModel->GetShouldRunVRSpline())
        m_pExampleController->ActivateExample("VRCameraSplineExample");
    else
        m_pExampleController->ActivateExample("JumpPointsExample");
}

void ExampleApp::OnWorldMenuItemGazed(Eegeo::UI::WorldMenu::WorldMenuItem& menuItem)
{
    
    if(menuItem.GetId()==m_worldMenuItemSelected)
        return;
    
    m_lastMenuItemSelected = m_worldMenuItemSelected;
    m_worldMenuItemSelected = menuItem.GetId();
    
    Eegeo::m33 orientation;
    Eegeo::dv3 position;
    
    switch(m_worldMenuItemSelected)
    {
        case 0:
        {
            break;
        }
            
        case 1:
        case 2:
        case 3:
        {
            if(m_lastMenuItemSelected==1 || m_lastMenuItemSelected==2 || m_lastMenuItemSelected==3)
                m_pExampleController->RestartExample();
            else
                m_pExampleController->ActivateExample("JumpPointsExample");
            break;
        }
            
        case 4:
        {
            m_pExampleController->ActivateExample("VRCameraSplineExample");
            break;
        }
    }
    
    
}

void ExampleApp::GetJumpPointStartPositionOrientation(Eegeo::dv3& position, Eegeo::m33& orientation)
{
    
    switch (m_worldMenuItemSelected)
    {        
        case 1:
        {
            
            
            Eegeo::Space::LatLongAltitude eyePosLla = Eegeo::Space::LatLongAltitude::FromDegrees(56.456160, -2.966101, 250);
            position = eyePosLla.ToECEF();
            
            Eegeo::Space::EcefTangentBasis tangentBasis;
            Eegeo::Camera::CameraHelpers::EcefTangentBasisFromPointAndHeading(position, 0.f, tangentBasis);
            
            
            orientation.SetRow(0, tangentBasis.GetRight());
            orientation.SetRow(1, tangentBasis.GetUp());
            orientation.SetRow(2, -tangentBasis.GetForward());
            
            break;
        }
            
        case 2:
        {
            
            
            Eegeo::Space::LatLongAltitude eyePosLla = Eegeo::Space::LatLongAltitude::FromDegrees(37.795215, -122.402797, 305.956129);
            position = eyePosLla.ToECEF();
            
            Eegeo::Space::EcefTangentBasis tangentBasis;
            Eegeo::Camera::CameraHelpers::EcefTangentBasisFromPointAndHeading(position, 0.f, tangentBasis);
            
            orientation.SetRow(0, tangentBasis.GetRight());
            orientation.SetRow(1, tangentBasis.GetUp());
            orientation.SetRow(2, -tangentBasis.GetForward());
            break;
        }
            
        case 3:
        {
            
            Eegeo::Space::LatLongAltitude eyePosLla = Eegeo::Space::LatLongAltitude::FromDegrees(48.856623, 2.297102, 450);
            
//            Eegeo::Space::LatLongAltitude eyePosLla = Eegeo::Space::LatLongAltitude::FromDegrees(35.657762, 139.746716, 188);
            position = eyePosLla.ToECEF();
            
            Eegeo::Space::EcefTangentBasis tangentBasis;
            Eegeo::Camera::CameraHelpers::EcefTangentBasisFromPointAndHeading(position, 0.f, tangentBasis);
            
            
            orientation.SetRow(0, tangentBasis.GetRight());
            orientation.SetRow(1, tangentBasis.GetUp());
            orientation.SetRow(2, -tangentBasis.GetForward());
            break;
        }
    }
    
}

void ExampleApp::UpdateNightTParam(float dt)
{
    m_nightTParam += dt;
    if(m_nightTParam>1.0f)
        m_nightTParam = 0.99f;
    
    m_nightTParam = Eegeo::Math::Clamp01(m_nightTParam);
    m_currentClearColor = Eegeo::v3::Lerp(m_startClearColor, m_destClearColor, m_nightTParam);
    
    m_pVRSkybox->UpdateSkyColor(m_currentClearColor);
}

void ExampleApp::ToggleNight()
{
    bool toggled_night = !m_night;
    
    std::stringstream themeNameBuidler;
    themeNameBuidler << (toggled_night ? "Night" : "Day");
    themeNameBuidler << "Default";
    Eegeo::Resources::CityThemes::ICityThemesService& cityThemeService = m_pWorld->GetMapModule().GetCityThemesModule().GetCityThemesService();

    const std::string& stateName = themeNameBuidler.str();
    cityThemeService.RequestTransitionToState(themeNameBuidler.str(), 1.f);
    
    if (cityThemeService.GetCurrentTheme().IsCityThemeStateSupported(stateName))
    {
        m_night = toggled_night;
        
        if (m_night) {
            m_startClearColor = Eegeo::v3(135.0f/255.0f, 206.0f/255.0f, 235.0f/255.0f);
            m_destClearColor = Eegeo::v3(0.0f/255.f,24.0f/255.f,72.0f/255.f) ;
        } else {
            m_startClearColor = Eegeo::v3(0.0f/255.f,24.0f/255.f,72.0f/255.f);
            m_destClearColor = Eegeo::v3(135.0f/255.0f, 206.0f/255.0f, 235.0f/255.0f);
        }
        m_nightTParam = 0.f;
    }
}

void ExampleApp::LoadNextExample()
{
    m_pExampleController->ActivateNext();
}

void ExampleApp::LoadSplineExample()
{
    m_pExampleController->ActivateExample("VRCameraSplineExample");
}

void ExampleApp::LoadJumpPointExample()
{
    m_pExampleController->ActivateExample("JumpPointsExample");
}

void ExampleApp::UpdateCardboardProfile(float cardboardProfile[])
{
    m_pExampleController->UpdateCardboardProfile(cardboardProfile);
    m_pVRDistortion->UpdateCardboardProfile(cardboardProfile);
}

void ExampleApp::MagnetTriggered(){
    const Eegeo::Rendering::ScreenProperties& screenProperties = m_screenPropertiesProvider.GetScreenProperties();
    Eegeo::v2 dim = Eegeo::v2(screenProperties.GetScreenWidth(), screenProperties.GetScreenHeight());
    Eegeo::v2 center = m_pVRDistortion->GetCardboardProfile().GetScreenMeshCenter(dim.x,dim.y);
    m_pUIInteractionController->Event_ScreenInteractionClick(center);
}

void ExampleApp::UpdateFogging(){
    Eegeo::Lighting::GlobalFogging& fogging = m_pWorld->GetCoreModule().GetLightingModule().GetGlobalFogging();
    fogging.SetHeightFogIntensity(0.0f);
    fogging.SetDistanceFogIntensity(1.0f);
    fogging.SetDistanceFogDistances(m_foggingFar - 500.0f, m_foggingFar);
    fogging.SetFogColour(Eegeo::v4(m_currentClearColor,1.0f));
    fogging.SetFogDensity(1.0f);
}

void ExampleApp::NotifyScreenPropertiesChanged(const Eegeo::Rendering::ScreenProperties& screenProperties)
{
    m_screenPropertiesProvider.NotifyScreenPropertiesChanged(screenProperties);
    
    if (m_pLoadingScreen != NULL)
    {
        m_pLoadingScreen->NotifyScreenDimensionsChanged(screenProperties.GetScreenWidth(), screenProperties.GetScreenHeight());
    }
    
    m_pExampleController->NotifyScreenPropertiesChanged(screenProperties);
}

void ExampleApp::UpdateLoadingScreen(float dt)
{
    if (m_pLoadingScreen == NULL)
        return;
    
    Eegeo::EegeoWorld& eegeoWorld = World();
    
    if (!eegeoWorld.Initialising() && !m_pLoadingScreen->IsDismissed())
    {
        m_pLoadingScreen->Dismiss();
        Eegeo::TtyHandler::TtyEnabled = true;
    }
    
    m_pLoadingScreen->SetProgress(eegeoWorld.GetInitialisationProgress());
    m_pLoadingScreen->Update(dt);
    
    if (!m_pLoadingScreen->IsVisible())
    {
        Eegeo_DELETE m_pLoadingScreen;
        m_pLoadingScreen = NULL;
    }
}

void ExampleApp::Event_TouchRotate(const AppInterface::RotateData& data)
{
	if(World().Initialising())
	{
		return;
	}

	m_pExampleController->Event_TouchRotate(data);

}

void ExampleApp::Event_TouchRotate_Start(const AppInterface::RotateData& data)
{
    if(World().Initialising())
	{
		return;
	}
    
	m_pExampleController->Event_TouchRotate_Start(data);
}

void ExampleApp::Event_TouchRotate_End(const AppInterface::RotateData& data)
{
    if(World().Initialising())
	{
		return;
	}
    
	m_pExampleController->Event_TouchRotate_End(data);
}

void ExampleApp::Event_TouchPinch(const AppInterface::PinchData& data)
{
    if(World().Initialising())
	{
		return;
	}
    
	m_pExampleController->Event_TouchPinch(data);
}

void ExampleApp::Event_TouchPinch_Start(const AppInterface::PinchData& data)
{
    if(World().Initialising())
	{
		return;
	}
    
	m_pExampleController->Event_TouchPinch_Start(data);
}

void ExampleApp::Event_TouchPinch_End(const AppInterface::PinchData& data)
{
    if(World().Initialising())
	{
		return;
	}
    
	m_pExampleController->Event_TouchPinch_End(data);
}

void ExampleApp::Event_TouchPan(const AppInterface::PanData& data)
{
    if(World().Initialising())
	{
		return;
	}
    
	m_pExampleController->Event_TouchPan(data);
}

void ExampleApp::Event_TouchPan_Start(const AppInterface::PanData& data)
{
    if(World().Initialising())
	{
		return;
	}
    
	m_pExampleController->Event_TouchPan_Start(data);
}

void ExampleApp::Event_TouchPan_End(const AppInterface::PanData& data)
{
    if(World().Initialising())
	{
		return;
	}
    
	m_pExampleController->Event_TouchPan_End(data);
}

void ExampleApp::Event_TouchTap(const AppInterface::TapData& data)
{
    if(World().Initialising())
	{
		return;
	}

    m_pExampleController->Event_TouchTap(data);
    
    const Eegeo::Rendering::ScreenProperties& screenProperties = m_screenPropertiesProvider.GetScreenProperties();
    Eegeo::v2 dim = Eegeo::v2(screenProperties.GetScreenWidth(), screenProperties.GetScreenHeight());
    Eegeo::v2 center = m_pVRDistortion->GetCardboardProfile().GetScreenMeshCenter(dim.x,dim.y);
    m_pUIInteractionController->Event_ScreenInteractionClick(center);
}

void ExampleApp::Event_TouchDoubleTap(const AppInterface::TapData& data)
{
    if(World().Initialising())
	{
		return;
	}
    
	m_pExampleController->Event_TouchDoubleTap(data);
}

void ExampleApp::Event_TouchDown(const AppInterface::TouchData& data)
{
    if(World().Initialising())
	{
		return;
	}
    
	m_pExampleController->Event_TouchDown(data);
}

void ExampleApp::Event_TouchMove(const AppInterface::TouchData& data)
{
    if(World().Initialising())
	{
		return;
	}
    
	m_pExampleController->Event_TouchMove(data);
}

void ExampleApp::Event_TouchUp(const AppInterface::TouchData& data)
{
    if(World().Initialising())
	{
		return;
	}
    
	m_pExampleController->Event_TouchUp(data);
}

