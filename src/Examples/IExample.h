// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#ifndef __ExampleApp__Example__
#define __ExampleApp__Example__

#include "IAppOnMap.h"
#include "GlobeCameraController.h"
#include "GlobeCameraTouchController.h"
#include "EcefTangentBasis.h"
#include "RenderCamera.h"
#include <string>

namespace Examples
{


class IExample
{
public:
	virtual ~IExample() { }

	virtual std::string Name() const = 0;

	virtual void Start() = 0;
	virtual void EarlyUpdate(float dt) { }
	virtual void Update(float dt) = 0;
    virtual void PreWorldDraw() { }
	virtual void Draw() = 0;
	virtual void Suspend()= 0;
    
    virtual void NotifyScreenPropertiesChanged(const Eegeo::Rendering::ScreenProperties& screenProperties) { ; }
    
    virtual const Eegeo::Camera::RenderCamera& GetRenderCamera() const = 0;
    
    virtual Eegeo::dv3 GetInterestPoint() const = 0;
    

	virtual bool Event_TouchRotate 			(const AppInterface::RotateData& data)
	{
		return false;
	}
	virtual bool Event_TouchRotate_Start	(const AppInterface::RotateData& data)
	{
		return false;
	}
	virtual bool Event_TouchRotate_End 		(const AppInterface::RotateData& data)
	{
		return false;
	}

	virtual bool Event_TouchPinch 			(const AppInterface::PinchData& data)
	{
		return false;
	}
	virtual bool Event_TouchPinch_Start 	(const AppInterface::PinchData& data)
	{
		return false;
	}
	virtual bool Event_TouchPinch_End 		(const AppInterface::PinchData& data)
	{
		return false;
	}

	virtual bool Event_TouchPan				(const AppInterface::PanData& data)
	{
		return false;
	}
	virtual bool Event_TouchPan_Start		(const AppInterface::PanData& data)
	{
		return false;
	}
	virtual bool Event_TouchPan_End 		(const AppInterface::PanData& data)
	{
		return false;
	}

	virtual bool Event_TouchTap 			(const AppInterface::TapData& data)
	{
		return false;
	}
	virtual bool Event_TouchDoubleTap		(const AppInterface::TapData& data)
	{
		return false;
	}

	virtual bool Event_TouchDown 			(const AppInterface::TouchData& data)
	{
		return false;
	}
	virtual bool Event_TouchMove 			(const AppInterface::TouchData& data)
	{
		return false;
	}
	virtual bool Event_TouchUp 				(const AppInterface::TouchData& data)
	{
		return false;
	}
};




class GlobeCameraStateRestorer
{
	Eegeo::Space::EcefTangentBasis m_initialEcefTangentBasis;
	float m_initialDistance;
	Eegeo::Camera::GlobeCamera::GlobeCameraController& m_globeCameraController;
public:
	GlobeCameraStateRestorer(Eegeo::Camera::GlobeCamera::GlobeCameraController& globeCameraController)
		:m_globeCameraController(globeCameraController)
		,m_initialEcefTangentBasis(globeCameraController.GetInterestBasis())
		,m_initialDistance(globeCameraController.GetDistanceToInterest())
	{

	}

	~GlobeCameraStateRestorer()
	{
		m_globeCameraController.SetView(m_initialEcefTangentBasis, m_initialDistance);
	}
};

}

#endif /* defined(__ExampleApp__Example__) */
