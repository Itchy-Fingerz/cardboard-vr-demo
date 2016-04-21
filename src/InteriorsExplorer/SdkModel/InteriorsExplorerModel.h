// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#pragma once

#include "Types.h"
#include "Interiors.h"
#include "ICallback.h"
#include "InteriorsExplorerExitMessage.h"
#include "InteriorsExplorerSelectFloorMessage.h"
#include "InteriorsExplorerFloorSelectionDraggedMessage.h"
#include "InteriorId.h"
#include "InteriorsExplorer.h"

    namespace InteriorsExplorer
    {
        namespace SdkModel
        {
            class InteriorsExplorerModel : private Eegeo::NonCopyable
            {
            public:
                InteriorsExplorerModel(Eegeo::Resources::Interiors::InteriorInteractionModel& interiorInteractionModel,
                                       Eegeo::Resources::Interiors::InteriorSelectionModel& interiorSelectionModel
//                                       VisualMap::SdkModel::IVisualMapService& visualMapService,
//                                       ExampleAppMessaging::TMessageBus& messageBus,
//                                       Metrics::IMetricsService& metricsService
                                       );
                ~InteriorsExplorerModel();
                
                void SelectFloor(int floor);
                
                void ShowInteriorExplorer();
                void ShowInteriorExplorer(bool fromWithinAnotherInterior);
                void HideInteriorExplorer();
                void Exit();
                
                void ChangeToInteriorMapState();
                void ResumePreviousMapState();
                
                void InsertInteriorExplorerEnteredCallback(Eegeo::Helpers::ICallback0& callback);
                void RemoveInteriorExplorerEnteredCallback(Eegeo::Helpers::ICallback0& callback);
                void InsertInteriorExplorerExitedCallback(Eegeo::Helpers::ICallback0& callback);
                void RemoveInteriorExplorerExitedCallback(Eegeo::Helpers::ICallback0& callback);

            private:
                
                void HandleInteractionModelStateChanged();
                
                void OnExit(const InteriorsExplorerExitMessage& message);
                void OnSelectFloor(const InteriorsExplorerSelectFloorMessage& message);
                void OnFloorSelectionDragged(const InteriorsExplorerFloorSelectionDraggedMessage& message);
                
                void PublishInteriorExplorerStateChange();

                Eegeo::Resources::Interiors::InteriorInteractionModel& m_interiorInteractionModel;
                Eegeo::Resources::Interiors::InteriorSelectionModel& m_interiorSelectionModel;
//                VisualMap::SdkModel::IVisualMapService& m_visualMapService;
//
//                ExampleAppMessaging::TMessageBus& m_messageBus;
//                Metrics::IMetricsService& m_metricsService;
                
                Eegeo::Helpers::TCallback0<InteriorsExplorerModel> m_interactionModelStateChangedCallback;
                
                Eegeo::Helpers::TCallback1<InteriorsExplorerModel, const InteriorsExplorerExitMessage&> m_exitCallback;
                Eegeo::Helpers::TCallback1<InteriorsExplorerModel, const InteriorsExplorerSelectFloorMessage&> m_selectFloorCallback;
                Eegeo::Helpers::TCallback1<InteriorsExplorerModel, const InteriorsExplorerFloorSelectionDraggedMessage&> m_floorSelectionDraggedCallback;
                
                int m_currentInteriorFloorIndex;
                bool m_interiorExplorerEnabled;
                Eegeo::Helpers::CallbackCollection0 m_interiorExplorerEnteredCallbacks;
                Eegeo::Helpers::CallbackCollection0 m_interiorExplorerExitedCallbacks;
            };
        }
}
