//
//  UIInteractionModule.cpp
//  SDKSamplesApp
//
//  Created by Ali on 5/16/16.
//
//

#include <algorithm>

#include "UIInteractionModule.h"

namespace Eegeo
{
    namespace UI
    {
        void UIInteractionModule::CreateWorldSpaceRayFromScreen(const Eegeo::v2& screenPoint, Ray& ray)
        {
            Eegeo::Camera::RenderCamera renderCamera = m_pExampleController->GetRenderCamera();
            
            //normalize the point
            float nx = 2.0f * screenPoint.GetX() / renderCamera.GetViewportWidth() - 1.f;
            float ny = - 2.0f * screenPoint.GetY() / renderCamera.GetViewportHeight() + 1.f;
            
            //prepare near and far points
            Eegeo::v4 near(nx, ny, 0.0f, 1.0f);
            Eegeo::v4 far(nx, ny, 1.0f, 1.0f);
            
            Eegeo::m44 invVP;
            Eegeo::m44::Inverse(invVP, renderCamera.GetViewProjectionMatrix());
            
            //unproject the points
            Eegeo::v4 unprojectedNear = Eegeo::v4::Mul(near, invVP);
            Eegeo::v4 unprojectedFar = Eegeo::v4::Mul(far, invVP);
            
            //convert to 3d
            Eegeo::v3 unprojectedNearWorld = unprojectedNear / unprojectedNear.GetW();
            Eegeo::v3 unprojectedFarWorld = unprojectedFar / unprojectedFar.GetW();
            
            //check intersection with a ray cast from camera position
            ray.origin = renderCamera.GetEcefLocation();
            ray.direction = (unprojectedNearWorld - unprojectedFarWorld).Norm();
        }
        
        bool UIInteractionModule::IsScreenPointInsideModel(const Eegeo::v2& screenPoint, IUIInteractableItem* uiItem)
        {
            Ray ray;
            CreateWorldSpaceRayFromScreen(screenPoint, ray);
            ray.origin -= uiItem->GetItemEcefPosition();
            
            //the following is a standard ray sphere intersection - for other shapes, an appropriate intersection method
            //should be used
            
            double a =
            ray.direction.GetX() * ray.direction.GetX()
            + ray.direction.GetY() * ray.direction.GetY()
            + ray.direction.GetZ() * ray.direction.GetZ();
            
            double b =
            ray.direction.GetX() * (2.0 * ray.origin.GetX())
            + ray.direction.GetY() * (2.0 * ray.origin.GetY())
            + ray.direction.GetZ() * (2.0 * ray.origin.GetZ());
            
            double c =
            (ray.origin.GetX() * ray.origin.GetX()
             + ray.origin.GetY() * ray.origin.GetY()
             + ray.origin.GetZ() * ray.origin.GetZ());
            
            c -= (uiItem->GetItemRadius() * uiItem->GetItemRadius());
            
            double d = b * b + (-4.0) * a * c;
            
            //if determinant is negative sphere is in negative ray direction so can't hit
            if (d < 0.0)
            {
                return false;
            }
            
            d = sqrt(d);
            
            double t = (-0.5) * (b - d) / a;
            
            if (t >= 0.0)
            {
                return false;
            }
            
            return true;
        }
        
        UIInteractionModule::UIInteractionModule(Examples::ExampleController* p_ExampleController):
        m_pExampleController(p_ExampleController),
         m_InteractableItems()
        {
        }
        
        UIInteractionModule::~UIInteractionModule()
        {
            m_InteractableItems.clear();
        }
        
        void UIInteractionModule::Update(float dt)
        {
            for (int i = 0; i != m_InteractableItems.size(); i++) {
                m_InteractableItems[i]->Update(dt);
            }
        }
        
        void UIInteractionModule::Event_ScreenInteractionStart(const Eegeo::v2& point)
        {
            for (int i = 0; i != m_InteractableItems.size(); i++) {
                if (IsScreenPointInsideModel(point, m_InteractableItems[i])) {
                    m_InteractableItems[i]->SetItemHasFocus(true);
                }
            }
        }
        
        void UIInteractionModule::Event_ScreenInteractionMoved(const Eegeo::v2& point)
        {
            for (int i = 0; i != m_InteractableItems.size(); i++) {
                if (IsScreenPointInsideModel(point, m_InteractableItems[i])) {
                    m_InteractableItems[i]->SetItemHasFocus(true);
                }
                else
                    m_InteractableItems[i]->SetItemHasFocus(false);
            }
        }
        
        void UIInteractionModule::Event_ScreenInteractionEnd(const Eegeo::v2& point)
        {
            for (int i = 0; i != m_InteractableItems.size(); i++) {
                if (IsScreenPointInsideModel(point, m_InteractableItems[i])) {
                    m_InteractableItems[i]->SetItemHasFocus(false);
                }
            }
        }
        
        void UIInteractionModule::Event_ScreenInteractionClick(const Eegeo::v2& point)
        {
            for (int i = 0; i != m_InteractableItems.size(); i++) {
                if (IsScreenPointInsideModel(point, m_InteractableItems[i])) {
                    m_InteractableItems[i]->OnItemClicked();
                }
            }
        }
        
        const IUIInteractableItem* UIInteractionModule::GetItemAtScreenPoint(const Eegeo::v2& point)
        {
            for (int i = 0; i != m_InteractableItems.size(); i++) {
                if (IsScreenPointInsideModel(point, m_InteractableItems[i])) {
                    return m_InteractableItems[i];
                }
            }
            
            return NULL;
        }
        
        void UIInteractionModule::RegisterInteractableItem(IUIInteractableItem* interactableItem)
        {
            m_InteractableItems.push_back(interactableItem);
        }
        
        void UIInteractionModule::UnRegisterInteractableItem(IUIInteractableItem* interactableItem)
        {
            std::vector<IUIInteractableItem*>::iterator position = std::find(m_InteractableItems.begin(), m_InteractableItems.end(), interactableItem);
            if (position != m_InteractableItems.end())
                m_InteractableItems.erase(position);
        }
    }
}


