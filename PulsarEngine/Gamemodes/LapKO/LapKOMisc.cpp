#include <Gamemodes/LapKO/LapKOMgr.hpp>

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

}  // namespace LapKO
}  // namespace Pulsar