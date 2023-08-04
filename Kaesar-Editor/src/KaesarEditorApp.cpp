#include "krpch.h"
#include <Kaesar.h>

#include "EditorLayer.h"
#include "Kaesar/Core/EntryPoint.h"

namespace Kaesar {
    class KaesarEditorApp : public Application
    {
    public:
        KaesarEditorApp()
            : Application()
        {
            PushLayer(new EditorLayer());
        }

        ~KaesarEditorApp()
        {

        }
    };

    Application* CreateApplication()
    {
        return new KaesarEditorApp();
    }
}