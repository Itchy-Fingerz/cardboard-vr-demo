// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#pragma once

#include "Types.h"
#include "IDeadZoneMenuItemObserver.h"
#include "IDeadZoneMenuItemObservable.h"
#include "DeadZoneMenuItemView.h"
#include "IDeadZoneMenuItemViewFactory.h"
#include <map>
#include <algorithm>
#include "IUIInteractionObservable.h"

namespace Eegeo
{
    namespace UI
    {
        namespace DeadZoneMenu
        {
            /*!
             * \brief Controller for a set of DeadZoneMenuItems.
             *
             */
            class DeadZoneMenuController : protected Eegeo::NonCopyable, public IDeadZoneMenuItemObserver
            {
            public:
                DeadZoneMenuController(IDeadZoneMenuItemObservable& deadZoneMenuItemObservable, IDeadZoneMenuItemViewFactory& viewFactory, IUIInteractionObservable& p_IUIInteractionObservable);
                
                ~DeadZoneMenuController();
                
                void Update(float deltaTime);
                
                virtual void OnDeadZoneMenuItemAdded(DeadZoneMenuItem& deadZoneMenuItem);
                
                virtual void OnDeadZoneMenuItemRemoved(DeadZoneMenuItem& deadZoneMenuItem);
                
            protected:
                virtual void UpdateViews();
                
            private:
                IDeadZoneMenuItemObservable& m_DeadZoneMenuItemRepository;
                IDeadZoneMenuItemViewFactory& m_viewFactory;
                IUIInteractionObservable& m_pIUIInteractionObservable;
                
                typedef std::map<DeadZoneMenuItem*, DeadZoneMenuItemView*> TViewsByModel;
                
                TViewsByModel m_viewsByModel;
                
                DeadZoneMenuItemView* GetViewForModel(const DeadZoneMenuItem& di) const;
                
                bool HasViewForModel(const DeadZoneMenuItem& di) const;
                
            };
        }
    }
}
