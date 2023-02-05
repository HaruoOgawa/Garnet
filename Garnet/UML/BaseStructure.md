```mermaid
classDiagram
    class WebMain{
        + g_WebApp
        + int main()
    }
    
    class CWebAppManager{
        - m_pGraphicAPI
        - m_pWindow
        + bool Initialize()
        + bool RunLoop()
        + void OnKeyDown(std::string key)
    }
    
    WebMain <|.. CWebAppManager
    
    class DescMain{
        + g_DescApp
        + int main()
    }
    
    class CDescAppManager{
        - m_pGraphicAPI
        - m_pWindow
        + bool Initialize()
        + bool RunLoop()
    }
    
    DescMain <|.. CDescAppManager
    
    class IApp{
        + bool Initialize()
        + bool Update()
        + bool Draw()
    }
    
    CWebAppManager <|.. IApp
    CDescAppManager <|.. IApp
    
    class CScriptApp{
        + bool Initialize()
        + bool Update()
        + bool Draw()
    }
    
    IApp <|-- CScriptApp
    
    class CEditorApp{
        + bool Initialize()
        + bool Update()
        + bool Draw()
    }
    
    IApp <|-- CEditorApp
    
    class CMainApp{
        + bool Initialize()
        + bool Update()
        + bool Draw()
    }
    
    IApp <|-- CMainApp
    CEditorApp .. CMainApp
```
