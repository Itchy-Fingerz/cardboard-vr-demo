// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#pragma once

#include "Types.h"
#include "VectorMath.h"
#include "Bounds.h"
#include "DeadZoneMenu.h"
#include "../UI/UIImageButton.h"
#include "../UI/IUICameraProvider.h"

namespace Eegeo
{
    namespace UI
    {
        namespace DeadZoneMenu
        {
            /*!
             * \brief A view object representing a DeadZoneMenuItem in the world.
             *
             *  This class deals with the concerns of displaying a DeadZoneMenuItem.
             *
             */
            class DeadZoneMenuItemView : protected Eegeo::NonCopyable , public UIImageButton
            {
            public:
                DeadZoneMenuItemView(DeadZoneMenuItem& deadZoneMenuItem
                                     , UIQuad* quad
                                     , IUICameraProvider& p_UICameraProvider);
                
                /*! Retrieve the DeadZoneMenuItem model that the view represents.
                 * \return The view's DeadZoneMenuItem model.
                 */
                DeadZoneMenuItem& GetDeadZoneMenuItem() const;
                
            private:
                DeadZoneMenuItem& m_DeadZoneMenuItem;
                IUICameraProvider& m_UICameraProvider;
            };
        }
    }
}