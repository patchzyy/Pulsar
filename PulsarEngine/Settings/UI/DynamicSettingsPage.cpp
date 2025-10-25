#include <Settings/UI/DynamicSettingsPage.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>

namespace Pulsar {
namespace UI {

DynamicSettingsPage::DynamicSettingsPage() : selectedIndex(0) {
    externControlCount = 0; // code-only page, no standard buttons
    internControlCount = 0;
    hasBackButton = false;
    activePlayerBitfield = 1;
    movieStartFrame = -1;
    extraControlNumber = 0;
    isLocked = false;
    controlCount = 0;
    nextSection = SECTION_NONE;
    controlSources = 1;

    onBackPressHandler.subject = this;
    onBackPressHandler.ptmf = &DynamicSettingsPage::OnBackPress;
}

DynamicSettingsPage::~DynamicSettingsPage() {}

void DynamicSettingsPage::OnInit() {
    MenuInteractable::OnInit();
    // Add our code-only canvas as a child control so Draw gets called in the usual UI pipeline
    this->AddControl(this->controlCount++, this->canvas, 0);
    // Hook back press to exit
    this->controlsManipulatorManager.Init(1, false);
    this->SetManipulatorManager(this->controlsManipulatorManager);
    this->controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, this->onBackPressHandler, false, false);
}

void DynamicSettingsPage::OnActivate() {
    // Set a title and a bottom hint using existing BMG
    this->titleBmg = BMG_SETTINGS_TITLE; // reuse generic title
    this->bottomText->SetMessage(BMG_SETTINGS_BOTTOM);
}

void DynamicSettingsPage::OnBackPress(u32) {
    // Save a subset of settings and go back
    Settings::Mgr* mgr = Settings::Mgr::sInstance;
    mgr->RequestSave();
    PushButton& fakeButton = this->backButton; // reuse animation duration
    this->LoadPrevPage(fakeButton);
}

// Poll controller and update our simple code-drawn layout
void DynamicSettingsPage::BeforeControlUpdate() {
    // Read primary KPAD channel 0
    KPAD::Status buff[1];
    KPAD::Read(0, buff, 1);
    const KPAD::Status& s = buff[0];

    // Navigate selection
    if (s.trig & WPAD::WPAD_BUTTON_DOWN) {
        selectedIndex = (selectedIndex + 1) % ITEM_COUNT;
    } else if (s.trig & WPAD::WPAD_BUTTON_UP) {
        selectedIndex = (selectedIndex - 1 + ITEM_COUNT) % ITEM_COUNT;
    }

    // Toggle values left/right/A
    if ((s.trig & (WPAD::WPAD_BUTTON_LEFT | WPAD::WPAD_BUTTON_RIGHT | WPAD::WPAD_BUTTON_A)) != 0) {
        Settings::Mgr* mgr = Settings::Mgr::sInstance;
        // In this demo, map to a few user-visible settings (user pages)
        switch (selectedIndex) {
            case ITEM_FASTMENUS: {
                // Menu -> Fast Menus radio
                u8 cur = mgr->GetUserSettingValue(Settings::SETTINGSTYPE_MENU, 0);
                mgr->SetUserSettingValue(Settings::SETTINGSTYPE_MENU, 0, cur ? 0 : 1);
                break;
            }
            case ITEM_ITEMRAIN: {
                // Friend Room 2 -> Item Box Respawn radio (demo)
                u8 cur = mgr->GetUserSettingValue(Settings::SETTINGSTYPE_FROOM2, 0);
                mgr->SetUserSettingValue(Settings::SETTINGSTYPE_FROOM2, 0, cur ? 0 : 1);
                break;
            }
            case ITEM_200CC: {
                // Friend Room 1 -> CC radio (demo)
                u8 cur = mgr->GetUserSettingValue(Settings::SETTINGSTYPE_FROOM1, 0);
                mgr->SetUserSettingValue(Settings::SETTINGSTYPE_FROOM1, 0, cur ? 0 : 1);
                break;
            }
            default: break;
        }
        mgr->Update();
    }

    // Rebuild canvas
    canvas.Clear();
    // Simple vertical list with highlighted selection
    const float startX = -180.0f; // UI space
    const float startY = -80.0f;
    const float w = 380.0f;
    const float h = 28.0f;
    for (int i = 0; i < ITEM_COUNT; ++i) {
        u32 col = (i == selectedIndex) ? 0x66ccffff : 0x224444ff;
        canvas.AddRect(startX, startY + i * (h + 8.0f), w, h, col);
    }
}

} // namespace UI
} // namespace Pulsar
