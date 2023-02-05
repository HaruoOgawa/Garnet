```mermaid
classDiagram
    class WebMain{
        + g_WebApp
        + int main()
    }
    
    class CWebAppManager{
        + bool Initialize()
        + bool RunLoop()
        + void OnKeyDown(std::string key)
    }
    
    WebMain <|-- CWebAppManager
    
    class DescMain{
        + g_DescApp
        + int main()
    }
    
    class CDescAppManager{
        + bool Initialize()
        + bool RunLoop()
    }
    
    DescMain <|-- CDescAppManager
    
    class CApp{
        + bool Initialize()
        + bool Update()
        + bool Draw()
    }
    
    CWebAppManager <|-- CApp
    CDescAppManager <|-- CApp
    
    class CMainApp{
    
    }
    
    CApp <|-- CMainApp
    
    class CScriptApp{
    
    }
    
    CApp <|-- CScriptApp
    
    class CEditorApp{
    
    }
    
    CApp <|-- CEditorApp
    
```
