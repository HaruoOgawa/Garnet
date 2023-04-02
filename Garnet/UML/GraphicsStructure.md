```mermaid
classDiagram
  class IGraphicsAPI{
    + bool Initialize()
    + IRenderer + CreateRenderer(CRendererCreateInfo createInfo);
  }

  class CVulkanAPI{
    + bool Initialize()
    + CVulkanRenderer CreateRenderer(CRendererCreateInfo createInfo);
  }
  
  IGraphicsAPI <|-- CVulkanAPI
  
  class CWebGPUAPI{
    + bool Initialize()
    + CWebGPURenderer CreateRenderer(CRendererCreateInfo createInfo);
  }
  
  IGraphicsAPI <|-- CWebGPUAPI

  class IRenderer{
  
  }
  
  CPrimitive ..|> IRenderer
  IGraphicsAPI ..|> IRenderer
  
  class CVulkanRenderer{
    - m_GraphicsPipeline
  }
  
  IRenderer ..|> CVulkanRenderer
  
  class CWebGPURenderer{
  
  }
  
  IRenderer ..|> CWebGPURenderer

  class C3DObject{
    - std::vector<CNode> m_NodeList
  }

  class CNode{
  }
 
  CNode ..|> C3DObject
 
  class CMesh{
    - m_PrimitiveList
  }
  
  CMesh ..|> CNode
  
  class CPrimitive{
    - IRenderer m_Renderer
    - CMaterial m_Material
  }

  CPrimitive ..|> CMesh
  
  class CMaterial{
    - m_UniformBufferObject
  }

  CMaterial ..|> CPrimitive
  
  class CUniformBufferObject{
    - m_UniformBlockList
  }
  
  CUniformBufferObject ..|> CMaterial
  
  class CUniformBlock{
    - m_UniformList
  }
  
  CUniformBlock ..|> CUniformBufferObject
  
  class CUniformValue{
    - int m_Size
    - int m_Offset
    - int m_HashKey
    - string m_UniformName
  }
  
  CUniformValue ..|> CUniformBlock

```
