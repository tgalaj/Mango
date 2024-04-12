#pragma once 

#include "Mango/Core/Base.h"
#include "Mango/Scene/Entity.h"
#include "Mango/Rendering/Material.h"

namespace mango
{
    class SelectionManager
    {
    public:
        static ref<SelectionManager> create()
        {
            s_selectionManager = createRef<SelectionManager>();
            return s_selectionManager;
        }

        static ref<SelectionManager> getSelection() { return s_selectionManager; }

        static void selectEntity  (Entity entity)                 { MG_VERIFY(s_selectionManager); s_selectionManager->m_selectedEntity   = entity; }
        static void selectMaterial(const ref<Material>& material) { MG_VERIFY(s_selectionManager); s_selectionManager->m_selectedMaterial = material; }

        static Entity&        getSelectedEntity  () { MG_VERIFY(s_selectionManager); return s_selectionManager->m_selectedEntity; }
        static ref<Material>& getSelectedMaterial() { MG_VERIFY(s_selectionManager); return s_selectionManager->m_selectedMaterial; }

        static void resetEntitySelection()
        {
            MG_VERIFY(s_selectionManager);
            s_selectionManager->m_selectedEntity = {};
        }

        static void resetMaterialSelection()
        {
            MG_VERIFY(s_selectionManager);
            s_selectionManager->m_selectedMaterial = nullptr;
        }

        static void resetAll() 
        {
            MG_VERIFY(s_selectionManager);

            s_selectionManager->m_selectedEntity   = {};
            s_selectionManager->m_selectedMaterial = nullptr;
        }

    private:
        Entity        m_selectedEntity;
        ref<Material> m_selectedMaterial;
        
        inline static ref<SelectionManager> s_selectionManager;
    };
}