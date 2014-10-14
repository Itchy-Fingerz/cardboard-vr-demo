// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#ifndef __ExampleApp__ExampleApp__
#define __ExampleApp__ExampleApp__

#include "GlobeCamera.h"
#include "EegeoWorld.h"
#include "ExampleController.h"

class ExampleApp : private Eegeo::NonCopyable
{
private:
	Eegeo::Camera::GlobeCamera::GlobeCameraController* m_pGlobeCameraController;
	Eegeo::Camera::GlobeCamera::GlobeCameraTouchController* m_pCameraTouchController;
	Eegeo::EegeoWorld* m_pWorld;
    Eegeo::Rendering::LoadingScreen* m_pLoadingScreen;
	Examples::ExampleController& m_exampleController;
    
    const Eegeo::Camera::RenderCamera* m_pActiveCamera;

	Eegeo::EegeoWorld& World()
	{
		return * m_pWorld;
	}
    
    void UpdateLoadingScreen(float dt);

public:
	ExampleApp(Eegeo::EegeoWorld* pWorld,
	           Examples::ExampleController& exampleController);

	~ExampleApp();

	void OnPause();

	void OnResume();

	void Update (float dt);

	void Draw (float dt);

	Eegeo::Camera::GlobeCamera::GlobeCameraController& GetCameraController()
	{
		return *m_pGlobeCameraController;
	}

	Eegeo::Camera::GlobeCamera::GlobeCameraTouchController& GetTouchController()
	{
		return *m_pCameraTouchController;
	}

	void Event_TouchRotate 			(const AppInterface::RotateData& data);
	void Event_TouchRotate_Start	(const AppInterface::RotateData& data);
	void Event_TouchRotate_End 		(const AppInterface::RotateData& data);

	void Event_TouchPinch 			(const AppInterface::PinchData& data);
	void Event_TouchPinch_Start 	(const AppInterface::PinchData& data);
	void Event_TouchPinch_End 		(const AppInterface::PinchData& data);

	void Event_TouchPan				(const AppInterface::PanData& data);
	void Event_TouchPan_Start		(const AppInterface::PanData& data);
	void Event_TouchPan_End 		(const AppInterface::PanData& data);

	void Event_TouchTap 			(const AppInterface::TapData& data);
	void Event_TouchDoubleTap		(const AppInterface::TapData& data);

	void Event_TouchDown 			(const AppInterface::TouchData& data);
	void Event_TouchMove 			(const AppInterface::TouchData& data);
	void Event_TouchUp 				(const AppInterface::TouchData& data);
};

#endif /* defined(__ExampleApp__ExampleApp__) */
