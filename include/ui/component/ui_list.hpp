#pragma once

#include <string>
#include <vector>

#include "ui/component/ui_component.hpp"
#include "ui/ui_types.hpp" // UiKey

class UiList : public UiComponent
{
    private:
        static inline unsigned int m_instanceCount;
    
    protected:
        std::vector<UiKey> m_itemsKey;
    
    public:
        UiList(UiController& uiController, const std::string& uiFilepath);

        unsigned int GetInstanceCount() override;
        void Open() override;
        
        std::vector<UiKey> GetItemsKey() const;
};