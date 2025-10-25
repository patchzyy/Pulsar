#ifndef _PUL_DYNAMIC_SETTINGS_PAGE_
#define _PUL_DYNAMIC_SETTINGS_PAGE_

#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Menu/Menu.hpp>
#include <MarioKartWii/UI/Ctrl/PushButton.hpp>
#include <UI/DynamicUI/DynamicLayout.hpp>
#include <UI/UI.hpp>

namespace Pulsar {
namespace UI {

// A fully dynamic settings page that doesn't use any brlyt/brctr files
class DynamicSettingsPage : public Pages::MenuInteractable {
public:
    static const PulPageId id = PULPAGE_DYNAMIC_SETTINGS;
    
    DynamicSettingsPage();
    ~DynamicSettingsPage() override;
    
    void OnInit() override;
    void OnActivate() override;
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;
    int GetActivePlayerBitfield() const override;
    int GetPlayerBitfield() const override;
    ManipulatorManager& GetManipulatorManager() override;
    void BeforeControlUpdate() override;
    
    PageId prevPageId;
    
private:
    void OnBackPress(u32 hudSlotId);
    void OnOptionButtonClick(PushButton& button, u32 hudSlotId);
    void OnButtonSelect(PushButton& button, u32 hudSlotId);
    void OnButtonDeselect(PushButton& button, u32 hudSlotId);
    void BuildDynamicLayout();
    
    PtmfHolder_1A<DynamicSettingsPage, void, u32> onBackPressHandler;
    PtmfHolder_2A<DynamicSettingsPage, void, PushButton&, u32> onOptionButtonClickHandler;
    PtmfHolder_2A<DynamicSettingsPage, void, PushButton&, u32> onButtonSelectHandler;
    PtmfHolder_2A<DynamicSettingsPage, void, PushButton&, u32> onButtonDeselectHandler;
    
    DynamicLayoutControl dynamicLayout;
    PushButton optionButtons[5];
    
    int selectedOption;
};

} // namespace UI
} // namespace Pulsar

#endif
