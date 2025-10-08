#ifndef _PUL_LAPKOMGR_
#define _PUL_LAPKOMGR_

#include <kamek.hpp>
#include <MarioKartWii/Race/RaceInfo/RaceInfo.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <Gamemodes/PositionCounter.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {
namespace LapKO {

class Mgr {
   public:
    Mgr();
    ~Mgr();

    void InitForRace();
    void ResetRound();
    void OnLapComplete(u8 playerId, RaceinfoPlayer& player);
    void OnPlayerFinished(u8 playerId);
    void OnPlayerDisconnected(u8 playerId);
    void UpdateFrame();
    void ApplyRemoteEvent(u8 seq, u8 eliminatedId, u8 roundIndex, u8 activeCount);

    bool IsActive(u8 playerId) const { return this->active[playerId]; }
    bool IsSpectatingLocal() const { return this->isSpectating; }
    u8 GetActiveCount() const { return this->activeCount; }
    u8 GetRoundIndex() const { return this->roundIndex; }
    u8 GetCurrentRoundEliminationCount() const {
        if (this->activeCount <= 1) return 0;
        const u8 idx = (this->roundIndex == 0) ? 0 : static_cast<u8>(this->roundIndex - 1);
        u8 elim = 1;
        if (idx < 8) {
            u8 planned = this->eliminationPlan[idx];
            if (planned != 0) elim = planned;
        }
        if (elim >= this->activeCount) elim = static_cast<u8>(this->activeCount - 1);
        return elim;
    }

    u8 GetPendingSequence() const { return this->pendingSequence; }
    u8 GetPendingElimination() const { return this->pendingElimination; }
    u8 GetPendingRound() const { return this->pendingRound; }
    u8 GetPendingActiveCount() const { return this->pendingActiveCount; }

    void ClearPendingEvent();

   private:
    void ComputeEliminationPlan();
    void ProcessEliminationInternal(u8 playerId, const char* reason, bool fromNetwork, bool suppressRoundAdvance);
    void StartNewRound();
    void TryResolveRound();
    void ProcessElimination(u8 playerId, const char* reason, bool fromNetwork);
    void EnterSpectateIfLocal(u8 playerId);
    void ConcludeRace(u8 winnerId);
    void BroadcastEvent(u8 playerId, u8 concludedRound);
    void LogState(const char* tag, u8 value) const;
    u8 FindNextSpectateTarget(u8 eliminatedId) const;

    u8 eliminationPlan[8];
    u8 totalRounds;

    bool active[12];
    bool crossed[12];
    u8 crossOrder[12];
    u16 lastLapValue[12];
    u8 orderCursor;
    u8 activeCount;
    u8 playerCount;
    u8 roundIndex;
    u8 eventSequence;
    u8 appliedSequence;
    u8 pendingSequence;
    u8 pendingElimination;
    u8 pendingRound;
    u8 pendingActiveCount;
    bool hasPendingEvent;
    bool isSpectating;
    bool isHost;
    u8 hostAid;
    u16 pendingTimer;
    u32 lastAvailableAids;
    bool raceFinished;
    bool raceInitDone;
    u16 lastRaceFrames;
};

}  // namespace LapKO
}  // namespace Pulsar

#endif