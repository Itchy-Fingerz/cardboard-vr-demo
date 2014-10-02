// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#ifndef __ExampleApp__EnvironmentNotifierExample__
#define __ExampleApp__EnvironmentNotifierExample__

#include <map>

#include "IExample.h"
#include "RenderContext.h"
#include "SphereMesh.h"
#include "LatLongAltitude.h"
#include "TerrainStreaming.h"
#include "IStreamingObserver.h"
#include "MortonKey.h"

namespace Examples
{
    struct SphereData
    {
        SphereData(const Eegeo::dv3& position, const Eegeo::v4& color)
        : m_spherePosition(position)
        , m_sphereColor(color)
        {
            
        }
        
        Eegeo::dv3 m_spherePosition;
        Eegeo::v4 m_sphereColor;
    };
    
    typedef std::map<Eegeo::Streaming::MortonKey, SphereData> TKeySphereMap;
    
class EnvironmentNotifierExampleTerrainStreamObserver : public Eegeo::Streaming::IStreamingObserver
{
private:
	TKeySphereMap& m_spheres;
    Eegeo::DebugRendering::DebugRenderer& m_debugRenderer;

	void AddSphere(const Eegeo::Streaming::MortonKey& key);
public:
	EnvironmentNotifierExampleTerrainStreamObserver(TKeySphereMap& spheres,
                                                    Eegeo::DebugRendering::DebugRenderer& debugRenderer)
    : m_spheres(spheres)
    , m_debugRenderer(debugRenderer)
	{

	}

	void AddedStreamingResourceToSceneGraph(const Eegeo::Streaming::MortonKey& key);
	void RemovedStreamingResourceFromSceneGraph(const Eegeo::Streaming::MortonKey& key);
};

class EnvironmentNotifierExample : public IExample
{
private:
	Eegeo::Resources::Terrain::TerrainStreaming& m_terrainStreaming;
	EnvironmentNotifierExampleTerrainStreamObserver* m_pObserver;
    Eegeo::Camera::GlobeCamera::GlobeCameraController& m_cameraController;
	GlobeCameraStateRestorer m_globeCameraStateRestorer;
    Eegeo::DebugRendering::DebugRenderer& m_debugRenderer;

	TKeySphereMap m_spheres;


public:
	EnvironmentNotifierExample(Eegeo::DebugRendering::DebugRenderer& debugRenderer,
	                           Eegeo::Resources::Terrain::TerrainStreaming& terrainStreaming,
	                           Eegeo::Camera::GlobeCamera::GlobeCameraController& cameraController);

	static std::string GetName()
	{
		return "EnvironmentNotifierExample";
	}
	std::string Name() const
	{
		return GetName();
	}

	void Start();
	void Update(float dt);
	void Draw();
	void Suspend();
    const Eegeo::Camera::RenderCamera& GetRenderCamera() const;
};
}


#endif /* defined(__ExampleApp__EnvironmentNotifierExample__) */
