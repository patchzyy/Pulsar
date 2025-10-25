#include <UI/UI.hpp>
#include <Settings/UI/ExpOptionsPage.hpp>
#include <Settings/UI/SettingsPanel.hpp>
#include <Settings/UI/CodeDrivenPage.hpp>

namespace Pulsar {
namespace UI {

kmWrite32(0x805fd754, 0x60000000);  // nop the InitControl call in the init func

ExpOptions::ExpOptions() { this->onButtonClickHandler.ptmf = &ExpOptions::ExpandedOnButtonClick; }

void ExpOptions::OnInit() {
    this->InitControlGroup(5 + 2);
    Options::OnInit();
    this->AddControl(this->controlGroup.controlCount - 2, settingsButton, 0);
    this->AddControl(this->controlGroup.controlCount - 1, codeDrivenButton, 0);

    this->settingsButton.Load(UI::buttonFolder, "SettingsButton", "Settings", 1, 0, false);
    this->settingsButton.buttonId = 5;
    this->settingsButton.SetOnClickHandler(this->onButtonClickHandler, 0);
    this->settingsButton.SelectInitial(0);

    this->codeDrivenButton.Load(UI::buttonFolder, "SettingsButton", "Settings", 1, 0, false);
    this->codeDrivenButton.buttonId = 6;
    this->codeDrivenButton.SetOnClickHandler(this->onButtonClickHandler, 0);
    this->codeDrivenButton.SelectInitial(0);
}

void ExpOptions::ExpandedOnButtonClick(PushButton& pushButton, u32 hudSlotId) {
    if (pushButton.buttonId == 5) {
        this->nextPageId = static_cast<PageId>(SettingsPanel::id);
        this->EndStateAnimated(0, pushButton.GetAnimationFrameSize());
    } else if (pushButton.buttonId == 6) {
        this->nextPageId = static_cast<PageId>(CodeDrivenPage::id);
        this->EndStateAnimated(0, pushButton.GetAnimationFrameSize());
    } else {
        this->OnButtonClick(pushButton, hudSlotId);
    }
}

static void PatchOptionsBRCTR(PushButton* button, const char* folderName, const char* ctrName, const char* variant, u32 playerCount, u32 r8, bool inaccessible) {
    button->Load(folderName, "SettingsButton", variant, playerCount, r8, inaccessible);
}
kmCall(0x805fd7bc, PatchOptionsBRCTR);  // so that the positions are correct
kmCall(0x805fd80c, PatchOptionsBRCTR);
kmCall(0x805fd858, PatchOptionsBRCTR);
}  // namespace UI
}  // namespace Pulsar