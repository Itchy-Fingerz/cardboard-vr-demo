//  Copyright (c) 2014 eeGeo Ltd. All rights reserved.

#include "JumpPointsCameraController.h"
#include "LatLongAltitude.h"
#include "CameraHelpers.h"
#include "SpaceHelpers.h"
#include "EcefTangentBasis.h"
#include "Quaternion.h"
#include "EarthConstants.h"
#include "MathFunc.h"
#include "IVRHeadTracker.h"
#include "Logger.h"

namespace Eegeo
{
    namespace VR
    {
        
        Eegeo::dv3 JumpPointsCameraController::GetEcefInterestPoint() const
        {
            dv3 ecefPosition = m_ecefPosition;
            return ecefPosition.Normalise() * Eegeo::Space::EarthConstants::Radius;
        }
        
        double JumpPointsCameraController::GetAltitudeAboveSeaLevel() const
        {
            return Space::SpaceHelpers::GetAltitude(m_pRenderCamera->GetEcefLocation());
        }
        
        void JumpPointsCameraController::SetProjectionMatrix(Eegeo::m44& projection)
        {
            m_pRenderCamera->SetProjectionMatrix(projection);
        }
        
        Camera::CameraState JumpPointsCameraController::GetCameraState() const
        {
            return Camera::CameraState(m_pRenderCamera->GetEcefLocation(),
                                       GetEcefInterestPoint(),
                                       m_pRenderCamera->GetViewMatrix(),
                                       m_pRenderCamera->GetProjectionMatrix());
        }
        
        m33& JumpPointsCameraController::GetOrientation()
        {
            return m_currentOrientation;
        }
        
        void JumpPointsCameraController::UpdateFromPose(const Eegeo::m33& orientation, float eyeDistance)
        {
            m_headTrackerOrientation = orientation;
            m_ecefPosition = m_pRenderCamera->GetEcefLocation();
            m33 orientationMatrix;
            m33::Mul(orientationMatrix, m_orientation, orientation);
            
            v3 eyeOffsetModified = dv3::ToSingle(m_ecefPosition.Norm()*eyeDistance);
            v3 rotatedEyeOffset = v3::Mul(eyeOffsetModified, orientationMatrix);
            
            m_currentOrientation = Eegeo::m33(orientationMatrix);
            
            float near, far;
            GetNearFarPlaneDistances(near,far);
            
            dv3 targetPos(m_ecefPosition.x + rotatedEyeOffset.x, m_ecefPosition.y + rotatedEyeOffset.y, m_ecefPosition.z + rotatedEyeOffset.z);
            
            m_pRenderCamera->SetOrientationMatrix(orientationMatrix);
            m_pRenderCamera->SetEcefLocation(targetPos);
            m_pRenderCamera->SetProjection(0.7f, near*m_nearMultiplier, far);
            
            
        }
        
        void JumpPointsCameraController::SetEcefPosition(const Eegeo::dv3& ecef)
        {
            m_ecefPosition = ecef;
            m_pRenderCamera->SetEcefLocation(m_ecefPosition);
        }
        
        void JumpPointsCameraController::SetStartLatLongAltitude(const Eegeo::Space::LatLongAltitude& eyePos, float heading)
        {
            m_ecefPosition = eyePos.ToECEF();
            
            Space::EcefTangentBasis tangentBasis;
            Camera::CameraHelpers::EcefTangentBasisFromPointAndHeading(m_ecefPosition, heading, tangentBasis);
            
            m_orientation.SetRow(0, tangentBasis.GetRight());
            m_orientation.SetRow(1, tangentBasis.GetUp());
            m_orientation.SetRow(2, -tangentBasis.GetForward());
            
            m_pRenderCamera->SetOrientationMatrix(m_orientation);
            m_pRenderCamera->SetEcefLocation(m_ecefPosition);
        }
        
        
        void JumpPointsCameraController::SetStartPositionAndOrientation(const Eegeo::dv3& position, const Eegeo::m33& orientation)
        {
            m_pRenderCamera->SetEcefLocation(position);
            m_orientation = orientation;
        }
        
        
        void JumpPointsCameraController::OnDv3Updated(Eegeo::dv3& delta)
        {
            m_pRenderCamera->SetEcefLocation(delta);
        }
        
        void JumpPointsCameraController::Update(float dt)
        {
            m_time += dt;
           
        }
        
        void JumpPointsCameraController::SetCameraOrientation(const Eegeo::m33& orientation)
        {
            m_orientation = orientation;
        }
        
        void JumpPointsCameraController::GetNearFarPlaneDistances(float& out_near, float& out_far)
        {
            double cameraAltitude = GetAltitudeAboveSeaLevel();
            double approxTerrainAltitude = 100;
            double approxTerrainAltitudeDelta = approxTerrainAltitude -100;
            
            const double ClipPlaneThresholdAltitude = 15000.0;
            Camera::CameraHelpers::GetAltitudeInterpolatedNearFar(cameraAltitude, approxTerrainAltitude, approxTerrainAltitudeDelta, ClipPlaneThresholdAltitude, out_near, out_far);
  
            
        }
            
    }
}
