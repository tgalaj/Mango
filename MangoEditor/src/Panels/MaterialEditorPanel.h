#pragma once

namespace mango
{
    class MaterialEditorPanel
    {
    public:
        MaterialEditorPanel()  = default;
        ~MaterialEditorPanel() = default;
        
        void onGui();

    public:
        bool isOpen = true;

    private:
    };
}