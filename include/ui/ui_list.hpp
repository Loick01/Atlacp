#pragma once

#include <vector>

#include "ui/ui_component.hpp"
#include "ui/ui_types.hpp"

class UiList : public UiComponent
{
    private:
        static inline unsigned int m_instanceCount;
    
    protected:
        std::vector<ElementKey> m_itemsKey;
    
    public:
        UiList(UiController& uiController, const std::string& uiFilepath);

        unsigned int GetInstanceCount() override;
        void Open() override;
        
        std::vector<ElementKey> GetItemsKey() const;
};