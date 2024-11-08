#include <RetroRewind.hpp>
#include <MarioKartWii/UI/Ctrl/Menu/CtrlMenuCharacterSelect.hpp>
#include <MarioKartWii/UI/Page/Menu/CharacterSelect.hpp>
#include <MarioKartWii/GlobalFunctions.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <MarioKartWii/UI/Ctrl/UIControl.hpp>
#include <Network/MatchCommand.hpp>

namespace RetroRewind {
namespace UI {

const char* cha_26_hatena = "cha_26_hatena";

// Uses the global function to get the character ID of the local player's Mii to determine its weight class.
System::WeightClass GetMiiWeightClass(Mii &mii) {
    CharacterId charId = GetMiiCharacterId(mii);
    if (charId < MII_M_A_MALE) {
        return System::LIGHTWEIGHT;
    } else if (charId < MII_L_A_MALE) {
        return System::MEDIUMWEIGHT;
    }
    return System::HEAVYWEIGHT;
}

// "Enables" all the buttons on the character select screen by setting the images to the character panes and making all the buttons accessible.
void EnableButtons(CtrlMenuCharacterSelect &charSelect) {
    for (u8 i = 0; i < 42; i++) {
        CtrlMenuCharacterSelect::ButtonDriver *buttonDriver = charSelect.GetButtonDriver(static_cast<CharacterId>(i));
        if (buttonDriver != nullptr) {
            const char* iconPaneName = GetCharacterIconPaneName(static_cast<CharacterId>(i));
            buttonDriver->SetPicturePane("chara", iconPaneName);
            buttonDriver->SetPicturePane("chara_shadow", iconPaneName);
            buttonDriver->SetPicturePane("chara_light_01", iconPaneName);
            buttonDriver->SetPicturePane("chara_light_02", iconPaneName);
            buttonDriver->SetPicturePane("chara_c_down", iconPaneName);
            buttonDriver->manipulator.inaccessible = false;
        }
    }
}

// Disables a specified button by setting the pane to a question mark and making the button inaccessible.
void DisableButton(CtrlMenuCharacterSelect::ButtonDriver *button) {
    if (button != nullptr) {
        button->SetPicturePane("chara", cha_26_hatena);
        button->SetPicturePane("chara_shadow", cha_26_hatena);
        button->SetPicturePane("chara_light_01", cha_26_hatena);
        button->SetPicturePane("chara_light_02", cha_26_hatena);
        button->SetPicturePane("chara_c_down", cha_26_hatena);
        button->manipulator.inaccessible = true;
    }
}

// Function to determine if a button should be restricted based on weight class
bool ShouldRestrictButton(int charRestrict, int buttonIndex) {
    if (charRestrict == Pulsar::CHAR_LIGHTONLY) {
        return (buttonIndex >= System::BUTTON_MARIO && buttonIndex < System::BUTTON_MII_A);
    } else if (charRestrict == Pulsar::CHAR_MEDIUMONLY) {
        return (buttonIndex >= System::BUTTON_BABY_MARIO && buttonIndex < System::BUTTON_MARIO) ||
               (buttonIndex >= System::BUTTON_WARIO && buttonIndex < System::BUTTON_MII_A);
    } else if (charRestrict == Pulsar::CHAR_HEAVYONLY) {
        return (buttonIndex >= System::BUTTON_BABY_MARIO && buttonIndex < System::BUTTON_WARIO);
    }
    return false;
}

void RestrictCharacterSelection(PushButton *button, u32 hudSlotId) {
    Pages::CharacterSelect *page = SectionMgr::sInstance->curSection->Get<Pages::CharacterSelect>();
    CtrlMenuCharacterSelect &charSelect = page->ctrlMenuCharSelect;
    SectionId curSection = SectionMgr::sInstance->curSection->sectionId;
    bool charRestrict = Pulsar::CHAR_DEFAULTSELECTION;

    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST) {
        charRestrict = System::sInstance->IsContext(Pulsar::PULSAR_CHARRESTRICT);
    }

    CtrlMenuCharacterSelect::ButtonDriver *driverButtons = charSelect.driverButtonsArray;
    System::WeightClass miiWeight = GetMiiWeightClass(page->localPlayerMiis[0]);

    EnableButtons(charSelect);

    // Disables the buttons that are not in the character weight class restriction.
    if (charRestrict != Pulsar::CHAR_DEFAULTSELECTION) {
        for (int i = System::BUTTON_BABY_MARIO; i < System::BUTTON_MII_A; i++) {
            if (ShouldRestrictButton(charRestrict, i)) {
                DisableButton(&driverButtons[i]);
            }
        }
    }

    // Disables the Miis in Local 2P no matter what.
    if (curSection == SECTION_P2_WIFI ||
        curSection == SECTION_P2_WIFI_FROOM_VS_VOTING ||
        curSection == SECTION_P2_WIFI_FROOM_TEAMVS_VOTING ||
        curSection == SECTION_P2_WIFI_FROOM_BALLOON_VOTING ||
        curSection == SECTION_P2_WIFI_FROOM_COIN_VOTING ||
        (charRestrict == Pulsar::CHAR_LIGHTONLY && miiWeight != System::LIGHTWEIGHT) ||
        (charRestrict == Pulsar::CHAR_MEDIUMONLY && miiWeight != System::MEDIUMWEIGHT) ||
        (charRestrict == Pulsar::CHAR_HEAVYONLY && miiWeight != System::HEAVYWEIGHT)) {
        DisableButton(&driverButtons[System::BUTTON_MII_A]);
        DisableButton(&driverButtons[System::BUTTON_MII_B]);
    }

    bool currentAccessible = !button->manipulator.inaccessible;

    if (!currentAccessible) {
        // Find the first available accessible button
        CtrlMenuCharacterSelect::ButtonDriver *newButton = nullptr;
        for (int i = System::BUTTON_BABY_MARIO; i < System::BUTTON_MII_A; i++) {
            if (!driverButtons[i].manipulator.inaccessible) {
                newButton = &driverButtons[i];
                break; // Select the first available character
            }
        }

        if (newButton) {
            // Deselect current button and select new button
            button->HandleDeselect(hudSlotId, -1);
            newButton->SelectInitial(hudSlotId);
            newButton->SetButtonColours(hudSlotId);
            page->OnButtonDriverSelect(newButton, newButton->buttonId, hudSlotId);
        }
    }
}

kmCall(0x807e33a8, RestrictCharacterSelection);

} // namespace UI
} // namespace RetroRewind