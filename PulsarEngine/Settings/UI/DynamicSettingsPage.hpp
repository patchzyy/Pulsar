#ifndef _PUL_DYNAMIC_SETTINGS_PAGE_
#define _PUL_DYNAMIC_SETTINGS_PAGE_

#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Menu/Menu.hpp>
#include <MarioKartWii/UI/Page/Menu/VSSettings.hpp>
#include <MarioKartWii/UI/Ctrl/CountDown.hpp>
#include <UI/UI.hpp>
#include <UI/Dynamic/DynamicCodeLayout.hpp>
#include <Settings/Settings.hpp>
#include <core/rvl/KPAD.hpp>

namespace Pulsar {
namespace UI {

// A code-only settings page that demonstrates building UI fully in code.
class DynamicSettingsPage : public Pages::MenuInteractable {
public:
    static const PulPageId id = PULPAGE_DYNAMIC_SETTINGS;

    DynamicSettingsPage();
    ~DynamicSettingsPage() override;

    void OnInit() override;
    void OnActivate() override;
    void BeforeControlUpdate() override;

    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override { return Pages::VSSettings::typeInfo; }

private:
    void OnBackPress(u32 hudSlotId);

    // Demo model: a few boolean toggles on the Misc settings page
    enum ItemId { ITEM_FASTMENUS = 0, ITEM_ITEMRAIN, ITEM_200CC, ITEM_COUNT };
    int selectedIndex;

    CodeCanvas canvas;
};

} // namespace UI
} // namespace Pulsar

#endif
