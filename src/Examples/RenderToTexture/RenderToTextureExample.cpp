// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include <vector>
#include "RenderTexture.h"
#include "RenderContext.h"
#include "ShaderIdGenerator.h"
#include "MaterialIdGenerator.h"
#include "RenderableFilters.h"
#include "GlBufferPool.h"
#include "VertexBindingPool.h"
#include "Mesh.h"
#include "VertexLayout.h"
#include "PostProcessVignetteShader.h"
#include "PostProcessVignetteMaterial.h"
#include "PostProcessVignetteRenderer.h"
#include "PostProcessVignetteRenderable.h"
#include "Quad.h"
#include "RenderToTextureExample.h"
#include "ScreenProperties.h"

namespace Examples
{
    //Give the effect a 10 frames per second intensity update to give it an old-timey movie vibe...
    const float RenderToTextureExample::SecondsBetweenEffectUpdates = 0.1f;
    
    RenderToTextureExample::RenderToTextureExample(Eegeo::Camera::GlobeCamera::GlobeCameraController& cameraController,
                                                   const Eegeo::Rendering::ScreenProperties& screenProperties,
                                                   Eegeo::Rendering::VertexLayouts::VertexLayoutPool& vertexLayoutPool,
                                                   Eegeo::Rendering::VertexLayouts::VertexBindingPool& vertexBindingPool,
                                                   Eegeo::Rendering::Shaders::ShaderIdGenerator& shaderIdGenerator,
                                                   Eegeo::Rendering::Materials::MaterialIdGenerator& materialIdGenerator,
                                                   Eegeo::Rendering::RenderableFilters& renderableFilters,
                                                   Eegeo::Rendering::GlBufferPool& glBufferPool)
    :m_globeCameraStateRestorer(cameraController)
    ,m_screenProperties(screenProperties)
    ,m_vertexLayoutPool(vertexLayoutPool)
    ,m_vertexBindingPool(vertexBindingPool)
    ,m_shaderIdGenerator(shaderIdGenerator)
    ,m_materialIdGenerator(materialIdGenerator)
    ,m_renderableFilters(renderableFilters)
    ,m_glBufferPool(glBufferPool)
    ,m_pVignetteShader(NULL)
    ,m_pVignetteMaterial(NULL)
    ,m_pRenderableMesh(NULL)
    ,m_pRenderable(NULL)
    ,m_pVignetteRenderer(NULL)
    ,m_secondsSinceLastEffectUpate(0.f)
    ,m_cameraController(cameraController)
    {
    }
    
    void RenderToTextureExample::Start()
    {
        // Allocate a full screen sized texture to render into, with depth and stencil attachments.
        //
        // This example assumes that the hardware is capable of allocating a RenderBuffer with a colour,
        // depth and stencil attachment, which should always be the case for iOS.
        //
        
        const bool needsDepthStencilBuffers = true;
        m_pRenderTexture = Eegeo_NEW(Eegeo::Rendering::RenderTexture)(static_cast<u32>(m_screenProperties.GetScreenWidth() * m_screenProperties.GetPixelScale()),
                                                                      static_cast<u32>(m_screenProperties.GetScreenHeight() * m_screenProperties.GetPixelScale()),
                                                                      needsDepthStencilBuffers);

        m_pVignetteShader = PostProcessVignetteShader::Create(m_shaderIdGenerator.GetNextId());
        
        m_pVignetteMaterial = Eegeo_NEW(PostProcessVignetteMaterial)(m_materialIdGenerator.GetNextId(),
                                                                     "PostProcessVignetteMaterial",
                                                                     *m_pVignetteShader,
                                                                     *m_pRenderTexture);
        
        m_pRenderableMesh = Eegeo::Rendering::Geometry::CreatePositionUVViewportQuad(m_glBufferPool, m_vertexLayoutPool, 1.f);
        
        const Eegeo::Rendering::VertexLayouts::VertexLayout& vertexLayout = m_pRenderableMesh->GetVertexLayout();
        const Eegeo::Rendering::VertexLayouts::VertexAttribs& vertexAttributes = m_pVignetteShader->GetVertexAttributes();
        const Eegeo::Rendering::VertexLayouts::VertexBinding& vertexBinding = m_vertexBindingPool.GetVertexBinding(vertexLayout, vertexAttributes);
        
        m_pRenderable = Eegeo_NEW(PostProcessVignetteRenderable)(Eegeo::Rendering::LayerIds::AfterWorld,
                                                                 m_pVignetteMaterial,
                                                                 vertexBinding,
                                                                 m_pRenderableMesh);
        
        m_pVignetteRenderer = Eegeo_NEW(PostProcessVignetteRenderer)(*m_pRenderable);
        
        m_renderableFilters.AddRenderableFilter(*m_pVignetteRenderer);
        
        UpdateEffect();
    }
    
    void RenderToTextureExample::Suspend()
    {
        m_renderableFilters.RemoveRenderableFilter(*m_pVignetteRenderer);
        
        Eegeo_DELETE m_pVignetteRenderer;
        m_pVignetteRenderer = NULL;
        
        Eegeo_DELETE m_pRenderable;
        m_pRenderable = NULL;
        
        Eegeo_DELETE m_pVignetteMaterial;
        m_pVignetteMaterial = NULL;
        
        Eegeo_DELETE m_pVignetteShader;
        m_pVignetteShader = NULL;
        
        Eegeo_DELETE m_pRenderTexture;
        m_pRenderTexture = NULL;
    }
    
    void RenderToTextureExample::PreWorldDraw()
    {
        // Before the world is rendered, we should switch to rendering into our texture...
        m_pRenderTexture->BeginRendering();
    }
    
    void RenderToTextureExample::Update(float dt)
    {
        m_secondsSinceLastEffectUpate += dt;
        if (m_secondsSinceLastEffectUpate > SecondsBetweenEffectUpdates)
        {
            UpdateEffect();
            m_secondsSinceLastEffectUpate = 0.f;
        }
    }
    
    void RenderToTextureExample::UpdateEffect()
    {
        // Set the vignette effect data for this frame (the colour and a radial intensity change)...
        m_pRenderable->SetVignetteColour(Eegeo::v3(0.9f, 0.8f, 0.6f));
        const float radiusIntensityVariance = (2-(rand()%5))/10.f;
        m_pRenderable->SetVignetteRadiusModifier(3.6f + radiusIntensityVariance);
    }
    
    const Eegeo::Camera::RenderCamera& RenderToTextureExample::GetRenderCamera() const
    {
        return *m_cameraController.GetCamera();
    }
}

