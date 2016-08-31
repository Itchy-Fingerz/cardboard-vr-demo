// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#ifndef APPRUNNER_H_
#define APPRUNNER_H_

#include "Graphics.h"
#include "AppHost.h"
#include "GlDisplayService.h"
#include "Types.h"

class AppRunner : Eegeo::NonCopyable
{
public:
	AppRunner(
	    AndroidNativeState* pNativeState
	);
	~AppRunner();

	void Pause();
	void Resume();
	void ActivateSurface();
	void Update(float deltaSeconds, float headTansform[]);

    void HandleTouchEvent(const Eegeo::Android::Input::TouchInputEvent& message);
    
    void UpdateCardboardProfile(float cardboardProfile[]);
    void MagnetTriggered();
    
private:
	AndroidNativeState* m_pNativeState;

	GlDisplayService m_displayService;
	void ReleaseDisplay();
	bool TryBindDisplay();
    	AppHost* m_pAppHost;
	bool m_isPaused;
	void CreateAppHost();
};

#endif
