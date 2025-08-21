#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Leaderboard/TeamLeaderboard.hpp>
#include <MarioKartWii/UI/Page/Page.hpp>
#include <runtimeWrite.hpp>

namespace Pulsar {
namespace BattleAuto {

typedef void (*BeforeCtrlUpdate_t)(Page*);
static BeforeCtrlUpdate_t sOrigBeforeCtrlUpdate = nullptr;

static const int AUTO_ADVANCE_FRAMES = 1;

extern "C" void BattleLeaderboardUpdate_BeforeControlUpdate(Pages::BattleLeaderboardUpdate* self) {
    const RacedataScenario& scenario = Racedata::sInstance->racesScenario;
    const GameMode mode = scenario.settings.gamemode;
    if (mode == MODE_PUBLIC_BATTLE) {
        if (sOrigBeforeCtrlUpdate != nullptr) {
            sOrigBeforeCtrlUpdate(reinterpret_cast<Page*>(self));
        }

        if (self->currentState == STATE_ACTIVE) {
            if (self->duration >= static_cast<u32>(AUTO_ADVANCE_FRAMES)) {
                self->EndStateAnimated(0, 0.0f);
            }
        } else {
            return;
        }
    }
}

kmRuntimeUse(0x808dad6c);
static void PatchBattleTeamResultsAutoAdvance() {
    const RacedataScenario& scenario = Racedata::sInstance->racesScenario;
    const GameMode mode = scenario.settings.gamemode;
    static bool patched = false;
    if (mode == MODE_PUBLIC_BATTLE) {
        if (patched) return;
        patched = true;

        u32 vtable = kmRuntimeAddr(0x808dad6c);
        u32* beforeCtrlSlot = reinterpret_cast<u32*>(vtable + 0x3C);  // index 15
        sOrigBeforeCtrlUpdate = reinterpret_cast<BeforeCtrlUpdate_t>(*beforeCtrlSlot);
        KamekRuntimeWrite::Write32(reinterpret_cast<u32>(beforeCtrlSlot), reinterpret_cast<u32>(&BattleLeaderboardUpdate_BeforeControlUpdate));
    } else {
        return;
    }
}

// Execute once when pages are loaded (REL addresses are valid at this point)
static PageLoadHook PatchBattleTeamResultsAutoAdvanceHook(PatchBattleTeamResultsAutoAdvance);

}  // namespace BattleAuto
}  // namespace Pulsar
