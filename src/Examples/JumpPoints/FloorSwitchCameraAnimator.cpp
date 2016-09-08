// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "FloorSwitchCameraAnimator.h"

namespace Examples
{
    FloorSwitchCameraAnimator::FloorSwitchCameraAnimator(InteriorsExplorer::IInteriorsExplorerModule& interiorsExplorerModule,
                                                         Eegeo::VR::JumpPointsCameraController& vrCamera,
                                                         InteriorCameraAnimationPositionProvider cameraPositionProvider,
                                                         const Eegeo::dv3& cameraPosBase)
    : m_interiorsExplorerModule(interiorsExplorerModule)
    , m_vrCamera(vrCamera)
    , m_cameraPositionProvider(cameraPositionProvider)
    , m_cameraPosBase(cameraPosBase)
    , m_isAnimating(false)
    , m_animationCallback(this, &FloorSwitchCameraAnimator::FloorAnimationStateChanged)
    {
        m_interiorsExplorerModule.RegisterInteriorAnimationCallback(m_animationCallback);
    }

    FloorSwitchCameraAnimator::~FloorSwitchCameraAnimator()
    {
        m_interiorsExplorerModule.UnregisterInteriorAnimationCallback(m_animationCallback);
    }

    void FloorSwitchCameraAnimator::SetCameraBasePosition(const Eegeo::dv3 &cameraPosBase)
    {
        m_cameraPosBase = cameraPosBase;
    }

    void FloorSwitchCameraAnimator::FloorAnimationStateChanged(InteriorsExplorer::InteriorsExplorerFloorAnimationState& state)
    {
        m_isAnimating = (state == InteriorsExplorer::InteriorsExplorerFloorAnimationState::AnimationStarted);

        if (!m_isAnimating)
        {
            //Work around for issue with interior mode exiting when camera is looking away
            m_interiorsExplorerModule.SelectFloor(m_interiorsExplorerModule.GetSelectedFloor(), false);
        }
    }
}
