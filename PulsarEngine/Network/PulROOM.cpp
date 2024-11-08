#include <kamek.hpp>
#include <MarioKartWii/RKNet/ROOM.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <Settings/UI/SettingsPanel.hpp>
#include <Settings/Settings.hpp>
#include <Network/Network.hpp>
#include <Network/PacketExpansion.hpp>

namespace Pulsar {
namespace Network {

// Implements the ability for a host to send a message, allowing for custom host settings

// Converts a PulROOM packet to system data
static void ConvertROOMPacketToData(const PulROOM& packet) {
    System* system = System::sInstance;
    system->netMgr.hostContext = packet.hostSystemContext;
    system->netMgr.racesPerGP = packet.raceCount;
}

// Handles logic before sending a ROOM packet
static void BeforeROOMSend(RKNet::PacketHolder<PulROOM>* packetHolder, PulROOM* src, u32 len) {
    packetHolder->Copy(src, len); // default copy

    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
    Pulsar::System* system = Pulsar::System::sInstance;
    PulROOM* destPacket = packetHolder->packet;

    if (destPacket->messageType == 1 && sub.localAid == sub.hostAid) {
        packetHolder->packetSize += sizeof(PulROOM) - sizeof(RKNet::ROOMPacket); // adjust packet size
        const Settings::Mgr& settings = Settings::Mgr::Get();

        const u8 koSetting = settings.GetSettingValue(Settings::SETTINGSTYPE_KO, SETTINGKO_ENABLED);
        const u8 ottOnline = settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_ONLINE);
        const u8 charRestrict = settings.GetUserSettingValue(Settings::SETTINGSTYPE_RR, SETTINGRR_RADIO_CHARSELECT);
        const u8 kartRestrict = settings.GetUserSettingValue(Settings::SETTINGSTYPE_RR, SETTINGRR_RADIO_KARTSELECT);
        const u8 itemMode = settings.GetUserSettingValue(Settings::SETTINGSTYPE_RR, SETTINGRR_SCROLLER_ITEMMODE);

        destPacket->hostSystemContext = (ottOnline != OTTSETTING_OFFLINE_DISABLED) << PULSAR_MODE_OTT // ott
            | (ottOnline == OTTSETTING_ONLINE_FEATHER) << PULSAR_FEATHER // ott feather
            | (settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_ALLOWUMTS) ^ true) << PULSAR_UMTS // ott umts
            | koSetting << PULSAR_MODE_KO
            | (settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_ALLOW_MIIHEADS) ^ true) << PULSAR_MIIHEADS
            | charRestrict << PULSAR_CHARRESTRICT
            | kartRestrict << PULSAR_KARTRESTRICT
            | itemMode << PULSAR_ITEMMODE
            | settings.GetUserSettingValue(Settings::SETTINGSTYPE_RR2, SETTINGRR2_RADIO_THUNDERCLOUD) << PULSAR_THUNDERCLOUD
            | settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_RADIO_CC) << PULSAR_500
            | settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_RADIO_HOSTWINS) << PULSAR_HAW;

        u8 raceCount;
        if (koSetting == KOSETTING_ENABLED) {
            raceCount = 0xFE;
        } else {
            switch (settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_SCROLL_GP_RACES)) {
                case 0x2: raceCount = 7; break;
                case 0x4: raceCount = 11; break;
                case 0x6: raceCount = 23; break;
                case 0x8: raceCount = 31; break;
                case 0xA: raceCount = 63; break;
                case 0xC: raceCount = 1; break;
                default: raceCount = 3;
            }
        }
        destPacket->raceCount = raceCount;
        ConvertROOMPacketToData(*destPacket);
    }
}
kmCall(0x8065b15c, BeforeROOMSend);

kmWrite32(0x8065add0, 0x60000000);

// Handles logic after receiving a ROOM packet
static void AfterROOMReception(const RKNet::PacketHolder<PulROOM>* packetHolder, const PulROOM& src, u32 len) {
    register RKNet::ROOMPacket* packet;
    asm(mr packet, r28;);
    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];

    if (src.messageType == 1 && sub.localAid != sub.hostAid && packetHolder->packetSize == sizeof(PulROOM)) {
        ConvertROOMPacketToData(src);

        // Exit the settings page to prevent graphical artifacts
        Page* topPage = SectionMgr::sInstance->curSection->GetTopLayerPage();
        PageId topId = topPage->pageId;
        if (topId == UI::SettingsPanel::id) {
            UI::SettingsPanel* panel = static_cast<UI::SettingsPanel*>(topPage);
            panel->OnBackPress(0);
        }
    }
    memcpy(packet, &src, sizeof(RKNet::ROOMPacket)); // default copy
}
kmCall(0x8065add8, AfterROOMReception);

// Implements that setting
kmCall(0x806460B8, System::GetRaceCount);
kmCall(0x8064f51c, System::GetRaceCount);

} // namespace Network
} // namespace Pulsar