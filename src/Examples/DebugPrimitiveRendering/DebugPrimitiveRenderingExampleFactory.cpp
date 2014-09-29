// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "DebugPrimitiveRenderingExampleFactory.h"
#include "DebugPrimitiveRenderingExample.h"

#include "DebugRenderingModule.h"

using namespace Examples;

DebugPrimitiveRenderingExampleFactory::DebugPrimitiveRenderingExampleFactory(Eegeo::EegeoWorld& world,
        Eegeo::Camera::GlobeCamera::GlobeCameraController& globeCameraController)
	: m_world(world)
	, m_globeCameraController(globeCameraController)
{

}

IExample* DebugPrimitiveRenderingExampleFactory::CreateExample() const
{
    Eegeo::Modules::Core::DebugRenderingModule& debugRenderingModule = m_world.GetDebugRenderingModule();
    
	return new Examples::DebugPrimitiveRenderingExample(debugRenderingModule.GetDebugRenderer(),
	        m_globeCameraController);
}

std::string DebugPrimitiveRenderingExampleFactory::ExampleName() const
{
	return Examples::DebugPrimitiveRenderingExample::GetName();
}

