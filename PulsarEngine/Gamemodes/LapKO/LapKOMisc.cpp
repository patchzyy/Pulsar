#include <Gamemodes/LapKO/LapKOMgr.hpp>
#include <runtimeWrite.hpp>

namespace Pulsar {
namespace LapKO {

static void FrameUpdate() {
    System* system = System::sInstance;
    if (system == nullptr) return;
    if (!system->IsContext(PULSAR_MODE_LAPKO)) return;
    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    if (controller->roomType != RKNet::ROOMTYPE_NONE && controller->roomType != RKNet::ROOMTYPE_FROOM_NONHOST && controller->roomType != RKNet::ROOMTYPE_FROOM_HOST) return;
    system->lapKoMgr->UpdateFrame();
}
static RaceFrameHook lapKoFrameHook(FrameUpdate);

kmRuntimeUse(0x8053F3B8); // Wifi Time Limit Expansion [Chadderz]
kmRuntimeUse(0x8053F3BC);
static void WifiTimeLimit() {
    // Default is 5 minutes (300k Milliseconds)
    kmRuntimeWrite32A(0x8053F3B8, 0x3C600005);
    kmRuntimeWrite32A(0x8053F3BC, 0x388393E0);

    System* system = System::sInstance;
    if (system == nullptr) return;
    if (!system->IsContext(PULSAR_MODE_LAPKO)) return;
    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    if (controller->roomType != RKNet::ROOMTYPE_NONE && controller->roomType != RKNet::ROOMTYPE_FROOM_NONHOST && controller->roomType != RKNet::ROOMTYPE_FROOM_HOST) return;

    // 15 minutes if LapKO is enabled (900k Milliseconds)
    kmRuntimeWrite32A(0x8053F3B8, 0x3C60000D);
    kmRuntimeWrite32A(0x8053F3BC, 0x6064BBA0);
}
static SectionLoadHook wifiTimeLimitHook(WifiTimeLimit);

}  // namespace LapKO
}  // namespace Pulsar