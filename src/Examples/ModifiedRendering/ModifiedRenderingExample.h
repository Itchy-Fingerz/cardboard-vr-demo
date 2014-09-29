// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#ifndef __ExampleApp__ModifiedRenderingExample__
#define __ExampleApp__ModifiedRenderingExample__

#include "IExample.h"

#include <vector>
#include "RenderContext.h"
#include "ShaderCompiler.h"
#include "GlobalLighting.h"
#include "Location.h"
#include "Rendering.h"
#include "SceneElementRepository.h"
#include "IRenderableFilter.h"
#include "GlState.h"
#include "PackedRenderable.h"
#include "GlHelpers.h"

namespace Examples
{
typedef Eegeo::Rendering::Renderables::PackedRenderable TRenderable;
typedef TRenderable* TRenderablePtr;
typedef Eegeo::Rendering::Scene::SceneElement<TRenderable> TSceneElement;
typedef TSceneElement* TSceneElementPtr;
typedef std::vector<TSceneElementPtr> TSceneElementPtrVec;

class MyRenderable : public Eegeo::Rendering::RenderableBase
{
public:
	MyRenderable(TRenderable& originalRenderable, const Eegeo::Rendering::Materials::IMaterial* pMaterial)
		: Eegeo::Rendering::RenderableBase(Eegeo::Rendering::LayerIds::Buildings, originalRenderable.GetEcefPosition(), pMaterial)
		, m_originalRenderable(originalRenderable)
	{
	}

	void Update(const Eegeo::Rendering::RenderContext& renderContext)
	{
		m_originalRenderable.CalcUnpackMVP(renderContext, 1.0f);
		m_originalRenderable.SetVisible();
	}

	void Render(Eegeo::Rendering::GLState& glState) const
	{
		m_originalRenderable.Render(glState);
	}

private:
	TRenderable& m_originalRenderable;
};

typedef Eegeo::Rendering::Scene::ISceneElementObserver<Eegeo::Rendering::Renderables::PackedRenderable> TSceneElementObserver;

class ModifiedRenderingExample : public IExample, TSceneElementObserver, Eegeo::Rendering::IRenderableFilter
{
private:

	struct MyPoolFilterCriteria : Eegeo::Rendering::Scene::ISceneElementFilterCriteria<TRenderable>
	{
		ModifiedRenderingExample* m_pOwner;
	public:
		MyPoolFilterCriteria(ModifiedRenderingExample* owner):m_pOwner(owner) {}
		virtual bool FiltersOut(const TSceneElement& item) const;
	};

	MyPoolFilterCriteria* m_pCriteria;

	Eegeo::Lighting::GlobalLighting& m_lighting;
	Eegeo::Streaming::IStreamingVolume& m_visibleVolume;
	Eegeo::Rendering::Scene::SceneElementRepository<Eegeo::Rendering::Renderables::PackedRenderable>& m_buildingRepository;
	Eegeo::Rendering::Filters::PackedRenderableFilter& m_buildingFilter;
	Eegeo::Rendering::RenderableFilters& m_renderableFilters;
	Eegeo::Rendering::Shaders::ShaderIdGenerator& m_shaderIdGenerator;
	Eegeo::Rendering::Materials::MaterialIdGenerator& m_materialIdGenerator;
	const Eegeo::Helpers::GLHelpers::TextureInfo& m_placeHolderTexture;
	GlobeCameraStateRestorer m_globeCameraStateRestorer;
    Eegeo::Camera::GlobeCamera::GlobeCameraController& m_cameraController;

	Eegeo::Lighting::GlobalLighting* m_pAlternativeLighting;
	Eegeo::Rendering::Shaders::PackedDiffuseShader* m_pAlternativeShader;
	Eegeo::Rendering::Materials::PackedDiffuseMaterial* m_pAlternativeMaterial;
	typedef std::map<TSceneElementPtr, MyRenderable*> TSceneElementToRenderablePtrMap;
	TSceneElementToRenderablePtrMap m_alternativeRenderables;

	int m_counter;

	bool IsToBeReplacedWithAlternative(const TSceneElement* pSceneElement)  const;
    void AddAlternativeRenderable(TSceneElement& sceneElement);
    void PopulateAlternativeRenderablesFromInitialSceneGraph();

public:
	ModifiedRenderingExample(Eegeo::Streaming::IStreamingVolume& visibleVolume,
	                         Eegeo::Lighting::GlobalLighting& lighting,
	                         Eegeo::Rendering::Scene::SceneElementRepository<Eegeo::Rendering::Renderables::PackedRenderable>& buildingRepository,
	                         Eegeo::Rendering::Filters::PackedRenderableFilter& buildingFilter,
	                         Eegeo::Rendering::RenderableFilters& renderableFilters,
	                         Eegeo::Rendering::Shaders::ShaderIdGenerator& shaderIdGenerator,
	                         Eegeo::Rendering::Materials::MaterialIdGenerator& materialIdGenerator,
	                         const Eegeo::Helpers::GLHelpers::TextureInfo& placeHolderTexture,
	                         Eegeo::Camera::GlobeCamera::GlobeCameraController& cameraController
	                        );

	//ISceneElementObserver interface.
	typedef Eegeo::Rendering::Scene::SceneElement<Eegeo::Rendering::Renderables::PackedRenderable> TMySceneElement;
	void OnSceneElementAdded(TMySceneElement& sceneElement);
	void OnSceneElementRemoved(TMySceneElement& sceneElement);

	//IRenderableFilter interface.
	void EnqueueRenderables(const Eegeo::Rendering::RenderContext& renderContext, Eegeo::Rendering::RenderQueue& renderQueue);

	static std::string GetName()
	{
		return "ModifiedRenderingExample";
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

#endif /* defined(__ExampleApp__ModifiedRenderingExample__) */
