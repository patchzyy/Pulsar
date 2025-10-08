#include <Gamemodes/LapKO/LapKOMgr.hpp>
#include <MarioKartWii/3D/Camera/CameraMgr.hpp>
#include <MarioKartWii/Race/RaceData.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <Network/PacketExpansion.hpp>

namespace Pulsar {
namespace LapKO {

static const u16 pendingBroadcastFrames = 120;

Mgr::Mgr()
    : orderCursor(0),
      activeCount(0),
      playerCount(0),
      roundIndex(1),
    totalRounds(0),
      eventSequence(0),
      appliedSequence(0),
      pendingSequence(0),
      pendingElimination(0xFF),
      pendingRound(0),
      pendingActiveCount(0),
      hasPendingEvent(false),
      isSpectating(false),
      isHost(true),
      hostAid(0xFF),
      pendingTimer(0),
    raceFinished(false),
    raceInitDone(false) {
    for (int i = 0; i < 12; ++i) {
        this->active[i] = false;
        this->crossed[i] = false;
        this->crossOrder[i] = 0xFF;
        this->lastLapValue[i] = 0;
    }
    this->lastAvailableAids = 0;
    this->lastRaceFrames = 0xFFFF;
}

Mgr::~Mgr() {}

void Mgr::InitForRace() {
    this->raceInitDone = true;
    OS::Report("LapKO: InitForRace start\n");
    const System* system = System::sInstance;
    const RKNet::Controller* controller = RKNet::Controller::sInstance;

    this->playerCount = system->nonTTGhostPlayersCount;
    if (this->playerCount == 0) {
        const Racedata* racedata = Racedata::sInstance;
        if (racedata != nullptr) this->playerCount = racedata->racesScenario.playerCount;
    }
    if (this->playerCount == 0) this->playerCount = 12;

    Raceinfo* raceinfo = Raceinfo::sInstance;

    for (int i = 0; i < 12; ++i) {
        this->active[i] = (i < this->playerCount);
        this->crossed[i] = false;
        this->crossOrder[i] = 0xFF;
        this->lastLapValue[i] = 0;
    }

    this->activeCount = this->playerCount;
    this->orderCursor = 0;
    this->roundIndex = 1;
    this->totalRounds = 0;
    this->eventSequence = 0;
    this->appliedSequence = 0;
    this->pendingSequence = 0;
    this->pendingElimination = 0xFF;
    this->pendingRound = 0;
    this->pendingActiveCount = 0;
    this->pendingTimer = 0;
    this->hasPendingEvent = false;
    this->isSpectating = false;
    this->raceFinished = false;
    this->raceInitDone = true;

    if (controller != nullptr && controller->roomType != RKNet::ROOMTYPE_NONE) {
        const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
        this->hostAid = sub.hostAid;
        this->isHost = (sub.localAid == sub.hostAid);
        this->lastAvailableAids = sub.availableAids;
    } else {
        this->hostAid = 0xFF;
        this->isHost = true;
        this->lastAvailableAids = 0;
    }

    Racedata* raceDataMutable = Racedata::sInstance;
    if (raceDataMutable != nullptr) {
        raceDataMutable->menusScenario.settings.lapCount = raceDataMutable->racesScenario.settings.lapCount;
    }

    // Build elimination plan so final is 1v1 within <=8 laps
    this->ComputeEliminationPlan();
    for (u8 i = 0; i < this->totalRounds; ++i) OS::Report("%u", this->eliminationPlan[i]);
    OS::Report("\nLapKO: InitForRace done players=%u host=%u\n", this->playerCount, this->isHost);
}


void Mgr::ResetRound() {
    this->orderCursor = 0;
    for (int i = 0; i < 12; ++i) {
        this->crossed[i] = false;
        this->crossOrder[i] = 0xFF;
    }
}

void Mgr::OnLapComplete(u8 playerId, RaceinfoPlayer& player) {
    if (playerId >= 12) return;
    if (!this->active[playerId]) return;
    if (this->crossed[playerId]) return;
    if (player.currentLap <= this->roundIndex) return;
    this->crossed[playerId] = true;
    if (this->orderCursor < 12) {
        this->crossOrder[this->orderCursor] = playerId;
        ++this->orderCursor;
    }
    this->TryResolveRound();
}

void Mgr::OnPlayerFinished(u8 playerId) {
    if (playerId >= 12) return;
    if (!this->active[playerId]) return;
    // Treat race finish as lap completion as well
    Raceinfo* raceinfo = Raceinfo::sInstance;
    if (raceinfo != nullptr) {
        RaceinfoPlayer* infoPlayer = raceinfo->players[playerId];
        if (infoPlayer != nullptr) {
            this->OnLapComplete(playerId, *infoPlayer);
        }
    }
}

void Mgr::OnPlayerDisconnected(u8 playerId) {
    if (playerId >= 12) return;
    if (!this->active[playerId]) return;
    OS::Report("LapKO: Disconnect player=%u\n", playerId);
    this->ProcessElimination(playerId, "disconnect", false);
}

void Mgr::TryResolveRound() {
    if (this->raceFinished) return;
    if (this->activeCount <= 1) return;

    // Determine number of eliminations this round
    u8 planIdx = (this->roundIndex - 1);
    if (planIdx >= this->totalRounds) return;
    u8 toEliminate = (planIdx < 8) ? this->eliminationPlan[planIdx] : 0;
    if (toEliminate == 0) return;  // no eliminations scheduled this round
    if (toEliminate >= this->activeCount) toEliminate = static_cast<u8>(this->activeCount - 1);

    const u8 requiredCrossings = static_cast<u8>(this->activeCount - toEliminate);
    if (this->orderCursor < requiredCrossings) return;  // wait until all safe players crossed

    // Collect elimination targets = remaining active players who haven't crossed OR tail of order if overflow
    u8 eliminatedList[2];
    u8 elimCount = 0;
    // First try find not crossed players
    for (u8 i = 0; i < 12 && elimCount < toEliminate; ++i) {
        if (!this->active[i]) continue;
        if (this->crossed[i]) continue;
        eliminatedList[elimCount++] = i;
    }
    // If still need more (all crossed because of lag), pick from the slowest finishers (end of crossOrder)
    for (int idx = this->orderCursor - 1; elimCount < toEliminate && idx >= 0; --idx) {
        u8 pid = this->crossOrder[idx];
        bool already = false;
        for (u8 c = 0; c < elimCount; ++c) if (eliminatedList[c] == pid) already = true;
        if (!already) eliminatedList[elimCount++] = pid;
    }
    if (elimCount == 0) return;

    // Apply eliminations sequentially but avoid advancing round until last in batch
    for (u8 i = 0; i < elimCount; ++i) {
        const bool lastOne = (i == elimCount - 1);
        this->ProcessEliminationInternal(eliminatedList[i], "round", false, !lastOne);
    }
}

void Mgr::ProcessElimination(u8 playerId, const char* reason, bool fromNetwork) {
    this->ProcessEliminationInternal(playerId, reason, fromNetwork, false);
}


void Mgr::ProcessEliminationInternal(u8 playerId, const char* reason, bool fromNetwork, bool suppressRoundAdvance) {
    if (playerId >= 12) return;
    if (!this->active[playerId]) return;
    if (this->raceFinished) return;
    const u8 concludedRound = this->roundIndex;

    this->active[playerId] = false;

    if (this->activeCount > 0) --this->activeCount;
    if (this->isHost && !fromNetwork) {
        this->BroadcastEvent(playerId, concludedRound);
    }

    this->EnterSpectateIfLocal(playerId);

    Raceinfo* raceinfo = Raceinfo::sInstance;
    if (raceinfo != nullptr) {
        RaceinfoPlayer* infoPlayer = raceinfo->players[playerId];
        if (infoPlayer != nullptr) {
            infoPlayer->Vanish();
        }
    }

    if (!suppressRoundAdvance) {
        this->ResetRound();
    }

    if (this->activeCount <= 1) {
        u8 winnerId = 0xFF;
        for (u8 i = 0; i < 12; ++i) {
            if (this->active[i]) {
                winnerId = i;
                break;
            }
        }
        OS::Report("LapKO: Winner=%u\n", winnerId);
        this->ConcludeRace(winnerId);
        return;
    }

    if (!suppressRoundAdvance) {
        ++this->roundIndex;
        OS::Report("LapKO: NextRound=%u active=%u\n", this->roundIndex, this->activeCount);
    }
}

void Mgr::EnterSpectateIfLocal(u8 playerId) {
    Racedata* racedata = Racedata::sInstance;
    if (racedata == nullptr) return;

    RacedataScenario& scenario = racedata->racesScenario;
    const u8 localCount = scenario.localPlayerCount;
    for (u8 i = 0; i < localCount; ++i) {
        if (scenario.settings.hudPlayerIds[i] == playerId) {
            // If we are offline (no active room) with bots, end the race immediately with correct placements.
            const RKNet::Controller* controller = RKNet::Controller::sInstance;
            const bool isOffline = (controller == nullptr) || (controller->roomType == RKNet::ROOMTYPE_NONE);
            if (isOffline) {
                Raceinfo* raceinfo = Raceinfo::sInstance;
                if (raceinfo == nullptr) return;
                // Finish all players in their current position order so the local gets their proper placement.
                const u8 total = this->playerCount > 0 ? this->playerCount : 12;
                if (raceinfo->playerIdInEachPosition != nullptr && raceinfo->players != nullptr) {
                    for (u8 pos = 0; pos < total && pos < 12; ++pos) {
                        const u8 pid = raceinfo->playerIdInEachPosition[pos];
                        if (pid >= 12) continue;
                        RaceinfoPlayer* p = raceinfo->players[pid];
                        if (p == nullptr) continue;
                        // Mark player as finished and commit placement.
                        p->EndRace(*p->raceFinishTime, false, 0);
                        raceinfo->EndPlayerRace(pid);
                    }
                } else {
                    // Fallback: end at least the local player to avoid softlocks.
                    RaceinfoPlayer* p = raceinfo->players[playerId];
                    if (p != nullptr) {
                        p->EndRace(*p->raceFinishTime, false, 0);
                        raceinfo->EndPlayerRace(playerId);
                    }
                }
                this->raceFinished = true;
            } else {
                this->isSpectating = true;
                scenario.settings.gametype = GAMETYPE_ONLINE_SPECTATOR;
                racedata->menusScenario.settings.gametype = GAMETYPE_ONLINE_SPECTATOR;
                const u8 focusId = this->FindNextSpectateTarget(playerId);
                RaceCameraMgr* cameraMgr = RaceCameraMgr::sInstance;
                if (cameraMgr != nullptr) {
                    cameraMgr->isOnlineSpectating = true;
                    if (focusId < 12) cameraMgr->focusedPlayerIdx = focusId;
                }
            }
            break;
        }
    }
}

u8 Mgr::FindNextSpectateTarget(u8 eliminatedId) const {
    for (u8 i = 0; i < 12; ++i) {
        if (!this->active[i]) continue;
        if (i == eliminatedId) continue;
        return i;
    }
    return 0xFF;
}

void Mgr::ConcludeRace(u8 winnerId) {
    if (this->raceFinished) return;
    this->raceFinished = true;

    Raceinfo* raceinfo = Raceinfo::sInstance;
    if (raceinfo == nullptr) return;

    u8 finishId = winnerId;
    if (finishId >= 12) {
        finishId = 0xFF;
        if (raceinfo->players != nullptr) {
            for (u8 i = 0; i < 12; ++i) {
                if (raceinfo->players[i] != nullptr) {
                    finishId = i;
                    break;
                }
            }
        }
    }

    if (finishId < 12) {
        raceinfo->EndPlayerRace(finishId);
        raceinfo->CheckEndRaceOnline(finishId);
    }
}

void Mgr::BroadcastEvent(u8 playerId, u8 concludedRound) {
    this->pendingSequence = static_cast<u8>((this->eventSequence + 1) & 0xFF);
    if (this->pendingSequence == 0) this->pendingSequence = 1;
    this->eventSequence = this->pendingSequence;
    this->pendingElimination = playerId;
    this->pendingRound = concludedRound;
    this->pendingActiveCount = this->activeCount;
    this->pendingTimer = pendingBroadcastFrames;
    this->hasPendingEvent = true;

    OS::Report("LapKO: Broadcast seq=%u player=%u round=%u active=%u\n", this->pendingSequence, playerId, concludedRound, this->pendingActiveCount);
}

void Mgr::ClearPendingEvent() {
    this->pendingSequence = 0;
    this->pendingElimination = 0xFF;
    this->pendingRound = 0;
    this->pendingActiveCount = 0;
    this->pendingTimer = 0;
    this->hasPendingEvent = false;
}

void Mgr::ApplyRemoteEvent(u8 seq, u8 eliminatedId, u8 roundIdx, u8 activeCnt) {
    if (seq == 0 || eliminatedId >= 12) return;
    if (seq == this->appliedSequence) return;

    this->appliedSequence = seq;
    this->roundIndex = roundIdx;
    this->ProcessElimination(eliminatedId, "network", true);
    this->activeCount = activeCnt;
}

void Mgr::UpdateFrame() {
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    Raceinfo* raceinfo = Raceinfo::sInstance;
    if (controller == nullptr || raceinfo == nullptr) return;

    // Observe raceFrames to detect new-race boundary and re-init at start
    if (raceinfo->players != nullptr) {
        const u16 rf = raceinfo->raceFrames;
        // If frames decreased (scene changed) mark init as not done and clear finished flag
        if (this->lastRaceFrames != 0xFFFF && rf < this->lastRaceFrames) {
            this->raceInitDone = false;
            this->raceFinished = false;
            this->playerCount = 0;
        }
        if (!this->raceInitDone && rf == 0) {
            this->InitForRace();
        }
        this->lastRaceFrames = rf;
    }

    if (this->raceFinished && !this->hasPendingEvent) return;

    const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];

    if (this->isHost && controller->roomType != RKNet::ROOMTYPE_NONE) {
        const u32 availableAids = sub.availableAids;
        if (this->lastAvailableAids != 0) {
            const u32 lost = this->lastAvailableAids & ~availableAids;
            if (lost != 0) {
                for (u8 playerId = 0; playerId < 12; ++playerId) {
                    if (!this->active[playerId]) continue;
                    const u8 aid = controller->aidsBelongingToPlayerIds[playerId];
                    if (aid >= 12) continue;
                    if ((lost & (1 << aid)) != 0) {
                        this->ProcessElimination(playerId, "disconnect", false);
                    }
                }
            }
        }
        this->lastAvailableAids = availableAids;
    }

    for (u8 playerId = 0; playerId < this->playerCount && playerId < 12; ++playerId) {
        RaceinfoPlayer* infoPlayer = raceinfo->players[playerId];
        if (infoPlayer == nullptr) continue;
        const u16 lapValue = infoPlayer->currentLap;
        if (lapValue != this->lastLapValue[playerId]) {
            if (lapValue > this->lastLapValue[playerId]) {
                this->OnLapComplete(playerId, *infoPlayer);
            }
            this->lastLapValue[playerId] = lapValue;
        }
    }

    if (this->isHost) {
        if (!this->hasPendingEvent) return;
        for (int aid = 0; aid < 12; ++aid) {
            if (aid == sub.localAid) continue;
            if ((sub.availableAids & (1 << aid)) == 0) continue;
            RKNet::PacketHolder<Network::PulRH1>* holder = controller->GetSendPacketHolder<Network::PulRH1>(aid);
            if (holder == nullptr) continue;
            if (holder->packetSize < sizeof(Network::PulRH1)) holder->packetSize = sizeof(Network::PulRH1);
            Network::PulRH1* packet = holder->packet;
            packet->lapKoEventSeq = this->pendingSequence;
            packet->lapKoEliminatedId = this->pendingElimination;
            packet->lapKoRoundIndex = this->pendingRound;
            packet->lapKoActiveCount = this->pendingActiveCount;
        }
        if (this->pendingTimer > 0) {
            --this->pendingTimer;
            if (this->pendingTimer == 0) this->ClearPendingEvent();
        }
    } else {
        if (this->hostAid >= 12) return;
        const u32 bufferIdx = controller->lastReceivedBufferUsed[this->hostAid][RKNet::PACKET_RACEHEADER1];
        RKNet::SplitRACEPointers* split = controller->splitReceivedRACEPackets[bufferIdx][this->hostAid];
        if (split == nullptr) return;
        const RKNet::PacketHolder<Network::PulRH1>* holder = split->GetPacketHolder<Network::PulRH1>();
        if (holder == nullptr) return;
        if (holder->packetSize != sizeof(Network::PulRH1)) return;
        const Network::PulRH1* packet = holder->packet;
        if (packet->lapKoEventSeq == 0) return;
        if (packet->lapKoEventSeq == this->appliedSequence) return;
        this->ApplyRemoteEvent(packet->lapKoEventSeq, packet->lapKoEliminatedId, packet->lapKoRoundIndex, packet->lapKoActiveCount);
    }
}

void Mgr::ComputeEliminationPlan() {
    for (int i = 0; i < 8; ++i) this->eliminationPlan[i] = 0;
    if (this->playerCount < 2) { this->totalRounds = 0; return; }
    u8 remainingPlayers = this->playerCount;
    u8 round = 0;
    // While more than 2 players and room for another round
    while (remainingPlayers > 2 && round < 8) {
        // Decide elimination count this round
        u8 elim = 1;
        if (remainingPlayers > 6) {
            // If we eliminate 2 now, remainingPlayers-2 >= 2
            elim = 2;
        } else {
            u8 afterSingle = remainingPlayers - 1;
            u8 neededIfSingle = afterSingle - 1;
            u8 potentialTotalRounds = static_cast<u8>(round + 1 + neededIfSingle);
            if (potentialTotalRounds > 8 && remainingPlayers > 3) elim = 2;
        }

        if (elim >= remainingPlayers - 1) elim = 1;
        this->eliminationPlan[round] = elim;
        remainingPlayers -= elim;
        ++round;
    }
    // If we ended with two players and still have room, schedule a final 1v1 elimination
    if (remainingPlayers == 2 && round < 8) {
        this->eliminationPlan[round] = 1;
        ++round;
        remainingPlayers -= 1;
    }
    this->totalRounds = round;
}

}  // namespace LapKO
}  // namespace Pulsar