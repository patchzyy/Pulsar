#include <Gamemodes/LapKO/LapKOMgr.hpp>

namespace Pulsar {
namespace LapKO {

static void FrameUpdate() {
    System* system = System::sInstance;
    if (system == nullptr) return;
    if (!system->IsContext(PULSAR_MODE_LAPKO)) return;
    system->lapKoMgr->UpdateFrame();
}

static RaceFrameHook lapKoFrameHook(FrameUpdate);

}  // namespace LapKO
}  // namespace Pulsar