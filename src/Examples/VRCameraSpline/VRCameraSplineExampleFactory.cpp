// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "VRCameraSplineExampleFactory.h"
#include "VRCameraSplineExample.h"
#include "EegeoWorld.h"
#include "ResourceCeilingProvider.h"
#include "DefaultCameraControllerFactory.h"
#include "MapModule.h"

namespace Examples
{

    VRCameraSplineExampleFactory::VRCameraSplineExampleFactory(Eegeo::EegeoWorld& world,
                                                               DefaultCameraControllerFactory&defaultCameraControllerFactory,
                                                               IVRHeadTracker& headTracker,
                                                               const IScreenPropertiesProvider& screenPropertiesProvider,
                                                               InteriorsExplorer::IInteriorsExplorerModule& interiorsExplorerModule,
                                                               Eegeo::UI::DeadZoneMenu::DeadZoneMenuItemRepository& deadZoneRepository)
    : m_world(world)
    , m_headTracker(headTracker)
    , m_screenPropertiesProvider(screenPropertiesProvider)
    , m_defaultCameraControllerFactory(defaultCameraControllerFactory)
    , m_interiorsExplorerModule(interiorsExplorerModule)
    , m_deadZoneRepository(deadZoneRepository)
{
    
}

IExample* VRCameraSplineExampleFactory::CreateExample() const
{
    Eegeo::Modules::Map::MapModule& mapModule = m_world.GetMapModule();
    
    const Eegeo::Rendering::ScreenProperties& initialScreenProperties = m_screenPropertiesProvider.GetScreenProperties();
    
    return new Examples::VRCameraSplineExample(m_world,
                                                mapModule.GetResourceCeilingProvider(),
                                                m_defaultCameraControllerFactory.Create(),
                                                m_headTracker,
                                                initialScreenProperties,
                                                m_interiorsExplorerModule,
                                                m_deadZoneRepository);
}

std::string VRCameraSplineExampleFactory::ExampleName() const
{
	return Examples::VRCameraSplineExample::GetName();
}
}
