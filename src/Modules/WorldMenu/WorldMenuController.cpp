// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "WorldMenuController.h"
#include "WorldMenuItemView.h"
#include "WorldMenuItem.h"
#include "WorldMenuUpView.h"
#include "Logger.h"
#include <vector>

namespace Eegeo
{
    namespace UI
    {
        namespace WorldMenu
        {
            
            WorldMenuController::WorldMenuController(IWorldMenuItemObservable& worldMenuItemObservable
                                                     , IWorldMenuItemViewFactory& viewFactory
                                                     , Eegeo::UI::IUIInteractionObservable& uiInteractionObservable
                                                     , Eegeo::UI::IUICameraProvider& uiCameraProvider
                                                     , Eegeo::UI::IUIQuadFactory& quadFactory
                                                     , Eegeo::UI::IUIRenderableFilter& uiRenderableFilter
                                                     , std::string texture
                                                     , int topSpriteID
                                                     , int numTilesAlong1Axes)
            : m_worldMenuItemRepository(worldMenuItemObservable)
            , m_viewFactory(viewFactory)
            , m_uiInteractionObservable(uiInteractionObservable)
            , m_uiCameraProvider(uiCameraProvider)
            {
                m_pWorldMenuUpView = Eegeo_NEW(WorldMenuUpView)(quadFactory, uiRenderableFilter, texture, topSpriteID, numTilesAlong1Axes);
                m_menuItemsShouldRender = true;
                m_isMenuShown = false;
                m_worldMenuItemRepository.AddWorldMenuObserver(this);
                Eegeo::v2 size(numTilesAlong1Axes,numTilesAlong1Axes);
                Eegeo::v2 outMin;
                Eegeo::v2 outMax;
                Eegeo::UI::CalculateUV(size, 20, outMin, outMax);

                Eegeo::v2 dimension = Eegeo::v2(50.f,50.f);
                m_pSelectedArrow = Eegeo_NEW(Eegeo::UI::UISprite)(uiRenderableFilter,
                                                                  quadFactory.CreateUIQuad(texture,
                                                                                           dimension,
                                                                                           outMin,
                                                                                           outMax,
                                                                                           Eegeo::Rendering::LayerIds::Values::AfterAll),
                                                                  dimension);
                m_pSelectedArrow->SetMaxAlpha(0.5f);
            }
            
            WorldMenuController::~WorldMenuController()
            {
                Eegeo_DELETE(m_pSelectedArrow);
                Eegeo_DELETE(m_pWorldMenuUpView);
                m_worldMenuItemRepository.RemoveWorldMenuObserver(this);
                for(TViewsByModel::iterator it = m_viewsByModel.begin(); it != m_viewsByModel.end(); ++it)
                {
                    WorldMenuItemView* pView = it->second;
                    m_uiInteractionObservable.UnRegisterInteractableItem(pView);
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
                    m_pSelectedArrow->SetItemShouldRender(menuItemsShouldRender);
                }

            }
            
            void WorldMenuController::Update(float deltaTime)
            {
                PositionItems();
                
                m_pSelectedArrow->Update(deltaTime);
                Eegeo::m33 headTrackedOrientation  = m_uiCameraProvider.GetOrientation();
                Eegeo::v3 top(headTrackedOrientation.GetRow(1));
                Eegeo::v3 forward(headTrackedOrientation.GetRow(2));
                
                Eegeo::dv3 center = m_uiCameraProvider.GetRenderCameraForUI().GetEcefLocation();
                Eegeo::dv3 position(center + (forward*100)+ (top*(Eegeo::Math::Tan(m_uiCameraProvider.GetRenderCameraForUI().GetFOV()/2.3f)*100)));
                
                m_pWorldMenuUpView->SetEcefPosition(position);
                
                if(!m_menuItemsShouldRender || m_isMenuShown)
                {
                    m_pWorldMenuUpView->HideView();
                }
                else
                {
                    m_pWorldMenuUpView->ShowView();
                }
                
                m_pWorldMenuUpView->Update(deltaTime);
            
                for(TViewsByModel::iterator it = m_viewsByModel.begin(); it != m_viewsByModel.end(); ++it)
                {
                    WorldMenuItemView* pView = it->second;
                    pView->SetItemShouldRender(m_menuItemsShouldRender && m_isMenuShown);
                    m_pSelectedArrow->SetItemShouldRender(m_menuItemsShouldRender && m_isMenuShown);
                    pView->Update(deltaTime);
                }
                
            }
            
            void WorldMenuController::SetSelectedMenuItemId(int menuItemId)
            {
                
                m_menuItemId = menuItemId;
                
                for(TViewsByModel::iterator it = m_viewsByModel.begin(); it != m_viewsByModel.end(); ++it)
                {
                    WorldMenuItemView* pView = it->second;
                    if(pView->GetWorldMenuItem().GetId()==m_menuItemId)
                    {
                        pView->SetItemSelected();
                    }
                    else
                    {
                        pView->SetItemUnselected();
                    }
                    
                }
            }
            
            void WorldMenuController::OnWorldMenuItemAdded(WorldMenuItem& worldMenuItem)
            {
                Eegeo_ASSERT(!HasViewForModel(worldMenuItem), "Attempt to add duplicate model to WorldMenuController.");
                
                WorldMenuItemView* pView = m_viewFactory.CreateViewForWorldMenuItem(worldMenuItem);
                m_viewsByModel[&worldMenuItem] = pView;
                
                m_uiInteractionObservable.RegisterInteractableItem(pView);
                m_isMenuShown = false;
            }
            
            void WorldMenuController::OnWorldMenuItemRemoved(WorldMenuItem& worldMenuItem)
            {
                Eegeo_ASSERT(HasViewForModel(worldMenuItem), "Attempt to remove unknown model from WorldMenuController.");
                WorldMenuItemView* pView = GetViewForModel(worldMenuItem);
                
                m_viewsByModel.erase(&worldMenuItem);
                m_uiInteractionObservable.UnRegisterInteractableItem(pView);
                
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
                Eegeo::v3 forward(headTrackedOrientation.GetRow(2));
                Eegeo::v3 top(center.ToSingle().Norm());
                Eegeo::v3 right(Eegeo::v3::Cross(top, forward));
                
                Eegeo::v3 vA = center.ToSingle();
                Eegeo::v3 vB = m_uiCameraProvider.GetOrientation().GetRow(2);
                float angle = Eegeo::Math::Rad2Deg(Eegeo::Math::ACos(Eegeo::v3::Dot(vA, vB)/(vA.Length()*vB.Length())));
                
                
                const float MarginAngle = 85;
                const int PositionMultiplier = 600;
                
                bool shouldUpdatePosition = false;
                
                if(!m_isMenuShown && angle<=MarginAngle)
                {
                    m_isMenuShown = true;
                    shouldUpdatePosition = true;
                    m_cachedHeadTracker = m_uiCameraProvider.GetHeadTrackerOrientation();
                    m_cachedCenter = center;
                }
                else if(m_isMenuShown && angle>MarginAngle)
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
                            m_pSelectedArrow->SetItemShouldRender(false);
                            continue;
                        }
                        margin += pView->GetWorldMenuItem().GetMarginRight();
                        
                        Eegeo::dv3 position(center + (forward*PositionMultiplier) + (top*45) + ((right*55*halfCount)-(right*margin)));
                        pView->SetEcefPosition(position);
                        pView->SetItemShouldRender(true);
                        m_pSelectedArrow->SetItemShouldRender(true);
                        if(m_menuItemId==pView->GetWorldMenuItem().GetId())
                        {
                            m_pSelectedArrow->SetEcefPosition(center + (forward*PositionMultiplier) + (top*92) + (right*55*halfCount)-(right*margin));
                        }
                        halfCount-=1;
                        
                        margin += pView->GetWorldMenuItem().GetMarginLeft();
                        pView->SetItemShouldRender(m_menuItemsShouldRender);
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
