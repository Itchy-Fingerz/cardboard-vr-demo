// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "WorldMenuController.h"
#include "WorldMenuItemView.h"
#include "WorldMenuItem.h"
#include "Logger.h"
#include <vector>

namespace Eegeo
{
    namespace UI
    {
        namespace WorldMenu
        {
            
            WorldMenuController::WorldMenuController(IWorldMenuItemObservable& WorldMenuItemObservable, IWorldMenuItemViewFactory& viewFactory, Eegeo::UI::IUIInteractionObservable& uiInteractionObservable , Eegeo::UI::IUICameraProvider& uiCameraProvider)
            : m_WorldMenuItemRepository(WorldMenuItemObservable)
            , m_viewFactory(viewFactory)
            , m_pIUIInteractionObservable(uiInteractionObservable)
            , m_uiCameraProvider(uiCameraProvider)
            {
                m_menuItemsShouldRender = true;
                m_isMenuShown = false;
                m_WorldMenuItemRepository.AddWorldMenuObserver(this);
            }
            
            WorldMenuController::~WorldMenuController()
            {
                m_WorldMenuItemRepository.RemoveWorldMenuObserver(this);
                for(TViewsByModel::iterator it = m_viewsByModel.begin(); it != m_viewsByModel.end(); ++it)
                {
                    WorldMenuItemView* pView = it->second;
                    m_pIUIInteractionObservable.UnRegisterInteractableItem(pView);
                }
                m_viewsByModel.clear();
            }
            
            void WorldMenuController::SetMenuItemsShouldRender(bool menuItemsShouldRender)
            {
                
                m_menuItemsShouldRender = menuItemsShouldRender;
                for(TViewsByModel::iterator it = m_viewsByModel.begin(); it != m_viewsByModel.end(); ++it)
                {
                    WorldMenuItemView* pView = it->second;
                    pView->SetItemShouldRender(menuItemsShouldRender);
                }
            }
            
            void WorldMenuController::Update(float deltaTime)
            {
                PositionItems();
                for(TViewsByModel::iterator it = m_viewsByModel.begin(); it != m_viewsByModel.end(); ++it)
                {
                    WorldMenuItemView* pView = it->second;
                    pView->SetItemShouldRender(m_menuItemsShouldRender & m_isMenuShown);
                    pView->Update(deltaTime);
                }
                
            }
            
            void WorldMenuController::SetSelectedFloorId(int floorId)
            {
                
                m_floorId = floorId;
                
                for(TViewsByModel::iterator it = m_viewsByModel.begin(); it != m_viewsByModel.end(); ++it)
                {
                    WorldMenuItemView* pView = it->second;
                    if(pView->GetWorldMenuItem().GetId()==m_floorId)
                    {
                        pView->SetItemSelected();
                    }
                    else
                    {
                        pView->SetItemUnselected();
                    }
                    
                }
            }
            
            void WorldMenuController::OnWorldMenuItemAdded(WorldMenuItem& WorldMenuItem)
            {
                Eegeo_ASSERT(!HasViewForModel(WorldMenuItem), "Attempt to add duplicate model to WorldMenuController.");
                
                WorldMenuItemView* pView = m_viewFactory.CreateViewForWorldMenuItem(WorldMenuItem);
                m_viewsByModel[&WorldMenuItem] = pView;
                
                m_pIUIInteractionObservable.RegisterInteractableItem(pView);
                m_isMenuShown = false;
            }
            
            void WorldMenuController::OnWorldMenuItemRemoved(WorldMenuItem& WorldMenuItem)
            {
                Eegeo_ASSERT(HasViewForModel(WorldMenuItem), "Attempt to remove unknown model from WorldMenuController.");
                WorldMenuItemView* pView = GetViewForModel(WorldMenuItem);
                
                m_viewsByModel.erase(&WorldMenuItem);
                m_pIUIInteractionObservable.UnRegisterInteractableItem(pView);
                
            }
            
            
            void WorldMenuController::PositionItems()
            {
                
                if(!m_menuItemsShouldRender)
                    return;
                
                Eegeo::m33 headTrackedOrientation;
                Eegeo::m33::Mul(headTrackedOrientation, m_uiCameraProvider.GetBaseOrientation(), m_cachedHeadTracker);
                
                float halfCount = m_viewsByModel.size()/2;
                if(m_viewsByModel.size()%2==0)
                    halfCount-=0.5f;
                
                Eegeo::dv3 center = m_uiCameraProvider.GetRenderCameraForUI().GetEcefLocation();
                
                Eegeo::v3 top(headTrackedOrientation.GetRow(1));
                Eegeo::v3 forward(headTrackedOrientation.GetRow(2));
                Eegeo::v3 right(headTrackedOrientation.GetRow(0));
                
                Eegeo::v3 vA = center.ToSingle();
                Eegeo::v3 vB = m_uiCameraProvider.GetOrientation().GetRow(2);
                float angle = Eegeo::Math::Rad2Deg(Eegeo::Math::ACos(Eegeo::v3::Dot(vA, vB)/(vA.Length()*vB.Length())));
                
                
                float marginAngle = 90;
                int positionMultiplier = 600;
                
                bool shouldUpdatePosition = false;
                
                if(!m_isMenuShown && angle<=marginAngle)
                {
                    m_isMenuShown = true;
                    shouldUpdatePosition = true;
                    m_cachedHeadTracker = m_uiCameraProvider.GetHeadTrackerOrientation();
                    m_cachedCenter = center;
                }
                else if(m_isMenuShown && angle>marginAngle)
                {
                    m_isMenuShown = false;
                    shouldUpdatePosition = true;
                }
                
                if ((m_cachedCenter - center).LengthSq() > 1) {
                    m_cachedCenter = center;
                    shouldUpdatePosition = true;
                }
                
                if(shouldUpdatePosition || m_isMenuShown)
                {
                    
                    std::vector<WorldMenuItemView*> items;
                    for(TViewsByModel::iterator it = m_viewsByModel.begin(); it != m_viewsByModel.end(); ++it)
                    {
                        std::vector<WorldMenuItemView*>::iterator itItems = items.begin();
                        for(; itItems != items.end(); ++itItems)
                        {
                            if((*itItems)->GetWorldMenuItem().GetId() < it->second->GetWorldMenuItem().GetId())
                            {
                                break;
                            }
                        }
                        items.insert(itItems, it->second);
                    }
                    
                    float margin = 0.f;
                    
                    for(std::vector<WorldMenuItemView*>::iterator it = items.begin(); it != items.end(); ++it)
                    {
                        WorldMenuItemView* pView = *it;
                        if(!m_isMenuShown)
                        {
                            pView->SetItemShouldRender(false);
                            continue;
                        }
                        margin += pView->GetWorldMenuItem().GetMarginRight();
                        
                        Eegeo::dv3 position(center + (forward*positionMultiplier) + (top*45) + ((right*55*halfCount)-(right*margin)));
                        pView->SetEcefPosition(position);
                        pView->SetItemShouldRender(true);
                        halfCount-=1;
                        
                        margin += pView->GetWorldMenuItem().GetMarginLeft();
                        
                    }
                    
                    items.clear();
                    m_lastCameraPosition = center;
                }
            }
            
            
            WorldMenuItemView* WorldMenuController::GetViewForModel(const WorldMenuItem& jp) const
            {
                TViewsByModel::const_iterator foundJP = m_viewsByModel.find(const_cast<WorldMenuItem*>(&jp));
                
                if(foundJP != m_viewsByModel.end())
                {
                    return foundJP->second;
                }
                
                return NULL;
            }
            
            bool WorldMenuController::HasViewForModel(const WorldMenuItem& jp) const
            {
                return (GetViewForModel(jp) != NULL);
            }
            
        }
    }
}
