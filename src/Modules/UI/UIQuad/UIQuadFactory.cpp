//
//  UIQuadFactory.cpp
//  SDKSamplesApp
//
//  Created by Aqif Hamid on 6/1/16.
//
//

#include "UIQuadFactory.h"
#include "ShaderIdGenerator.h"
#include "MaterialIdGenerator.h"
#include "Logger.h"

namespace Eegeo
{
    namespace UI
    {
        
        UIQuadFactory::UIQuadFactory(const Eegeo::Modules::Core::RenderingModule& renderingModule,
                                     Eegeo::Helpers::ITextureFileLoader& textureFileLoader
)
        : m_GlBufferPool(renderingModule.GetGlBufferPool())
        , m_VertexBindingPool(renderingModule.GetVertexBindingPool())
        , m_VertexLayoutPool(renderingModule.GetVertexLayoutPool())
        , m_ShaderIdGenerator(renderingModule.GetShaderIdGenerator())
        , m_MaterialIdGenerator(renderingModule.GetMaterialIdGenerator())
        , m_TextureFileLoader(textureFileLoader)
        , m_Shader(NULL)
        {
            m_Shader = Eegeo::Rendering::Shaders::TexturedUniformColoredShader::Create(m_ShaderIdGenerator.GetNextId());
        }
        
        UIQuadFactory::~UIQuadFactory()
        {
            Eegeo_DELETE m_Shader;
            for(TMaterialMap::iterator it = m_MaterialMap.begin(); it != m_MaterialMap.end(); ++it)
            {
                Eegeo_DELETE it->second;
            }
            m_MaterialMap.clear();
        }
        
        UIQuad* UIQuadFactory::CreateUIQuad(const std::string& assetPath,
                             const Eegeo::v2& dimension,
                             const Eegeo::v2& uvMin,
                             const Eegeo::v2& uvMax,
                             const Eegeo::dv3& ecefPosition,
                             const Eegeo::v4& initialColor,
                             const Eegeo::Rendering::LayerIds::Values renderLayer){
            Eegeo::Rendering::Materials::TexturedUniformColoredMaterial* material = GetMaterialForAsset(assetPath);
            return new UIQuad(assetPath, *material, m_VertexBindingPool, m_GlBufferPool, dimension, uvMin, uvMax, ecefPosition, initialColor, renderLayer);
        }
        
        Eegeo::Rendering::Materials::TexturedUniformColoredMaterial* UIQuadFactory::GetMaterialForAsset(const std::string& assetPath)
        {
            TMaterialMap::const_iterator foundMaterial = m_MaterialMap.find(assetPath);
            
            if(foundMaterial != m_MaterialMap.end())
            {
                return foundMaterial->second;
            }
            
            Helpers::GLHelpers::TextureInfo textureInfo;
            textureInfo.textureId = 0;
            textureInfo.width = 0;
            textureInfo.height = 0;
            
            
            const bool generateMipmaps = true;
            bool success = m_TextureFileLoader.LoadTexture(textureInfo, assetPath, generateMipmaps);
            
            Eegeo_ASSERT(success, "failed to load texture");
            if (!success)
                return NULL;
            
            const v4& initialColor = v4::One();
            Eegeo::Rendering::Materials::TexturedUniformColoredMaterial* material = new (Eegeo::Rendering::Materials::TexturedUniformColoredMaterial) (
                                                                               m_MaterialIdGenerator.GetNextId(),
                                                                               "UIRectMaterial",
                                                                               *m_Shader,
                                                                               textureInfo.textureId,
                                                                               initialColor);
            m_MaterialMap[assetPath] = material;
            return material;
            
        }
        
        
        
    }
}
