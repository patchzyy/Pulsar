#include <kamek.hpp>
#include <MarioKartWii/Race/RaceInfo/RaceInfo.hpp>
#include <MarioKartWii/3D/Model/ModelDirector.hpp>
#include <MarioKartWii/Kart/KartValues.hpp>
#include <MarioKartWii/Kart/KartMovement.hpp>
#include <MarioKartWii/Item/Obj/ObjProperties.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <MarioKartWii/File/StatsParam.hpp>
#include <Race/200ccParams.hpp>
#include <PulsarSystem.hpp>
#include <RetroRewind.hpp>
#include <Settings/Settings.hpp>
#include <Settings/SettingsParam.hpp>

namespace Pulsar {
namespace Race {
// Mostly a port of MrBean's version with better hooks and arguments documentation
bool IsLapKOEnabled(const System* system) {
    if (system == nullptr) return false;
    if (system->IsContext(PULSAR_MODE_LAPKO)) return true;
    if (system->lapKoMgr != nullptr) return true;
    if ((system->netMgr.hostContext & (1 << PULSAR_MODE_LAPKO)) != 0) return true;
    return false;
}

u8 GetLapKOTargetCount(const System* system, const Racedata* racedata, u8 fallback) {
    u8 playerCount = 0;
    if (system != nullptr) playerCount = system->nonTTGhostPlayersCount;
    if (playerCount == 0) playerCount = fallback;
    if (playerCount < 2) playerCount = 2;
    if (playerCount > 9) playerCount = 9;
    return playerCount;
}

RaceinfoPlayer* LoadCustomLapCount(RaceinfoPlayer* player, u8 id) {
    System* system = System::sInstance;
    Racedata* racedata = Racedata::sInstance;
    u8 lapCount = KMP::Manager::sInstance->stgiSection->holdersArray[0]->raw->lapCount;

    const bool lapKoActive = IsLapKOEnabled(system);
    if (lapKoActive) {
        // Base KO lap count (existing behaviour)
        const u8 basePlayers = GetLapKOTargetCount(system, racedata, 1);
        u8 koLapCount = static_cast<u8>(basePlayers - 1);

        // Detect usual track lap count from KMP (1, 2, or 3+)
        const u8 usualTrackLaps = KMP::Manager::sInstance->stgiSection->holdersArray[0]->raw->lapCount;

        // Special cases for LapKO
        if (usualTrackLaps <= 1) {
            // 1-lap tracks: always 1 lap
            lapCount = 1; // ensure non-zero; display hook will handle proper labeling
        } else if (usualTrackLaps == 2) {
            // 2-lap tracks: keep same logic for 2 players, but for 3+ players halve the KO lap count
            if (basePlayers >= 3) {
                // Halve, rounding down, but ensure at least 1 in the internal representation
                u8 halved = static_cast<u8>(koLapCount / 2);
                if (halved == 0) halved = 1; // safety floor
                lapCount = halved;
            } else {
                lapCount = koLapCount;
            }
        } else {
            // Normal tracks: existing KO behaviour
            lapCount = koLapCount;
        }
    }

    if (racedata != nullptr) {
        racedata->racesScenario.settings.lapCount = lapCount;
        if (lapKoActive) racedata->menusScenario.settings.lapCount = lapCount;
    }
    return new (player) RaceinfoPlayer(id, lapCount);
}
kmCall(0x805328d4, LoadCustomLapCount);

// kmWrite32(0x80723d64, 0x7FA4EB78);
void DisplayCorrectLap(AnmTexPatHolder* texPat) {  // This Anm is held by a ModelDirector in a Lakitu::Player
    register u32 maxLap;
    asm(mr maxLap, r29;);
    texPat->UpdateRateAndSetFrame((float)(maxLap - 2));
    return;
}
kmCall(0x80723d70, DisplayCorrectLap);

// Moved speed modifier to Race/StatChanges.cpp
kmWrite32(0x805336B8, 0x60000000);
kmWrite32(0x80534350, 0x60000000);
kmWrite32(0x80534BBC, 0x60000000);
kmWrite32(0x80723D10, 0x281D0009);
kmWrite32(0x80723D40, 0x3BA00009);

kmWrite24(0x808AAA0C, 'PUL');  // time_number -> time_numPUL
}  // namespace Race
}  // namespace Pulsar