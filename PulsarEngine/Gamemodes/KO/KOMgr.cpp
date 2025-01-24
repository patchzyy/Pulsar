#include <MarioKartWii/Race/Raceinfo/Raceinfo.hpp>
#include <MarioKartWii/UI/Page/Page.hpp>
#include <MarioKartWii/Input/InputManager.hpp>
#include <GameModes/KO/KOMgr.hpp>
#include <Network/PacketExpansion.hpp>
#include <Gamemodes/KO/KORaceEndPage.hpp>
#include <Settings/Settings.hpp>

namespace Pulsar {
namespace KO {

// Constructor/Destructor
Mgr::Mgr() 
    : winnerPlayerId(0xFF)
    , isSpectating(false)
    , hasSwapped(false) {
    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
    this->baseLocPlayerCount = sub.localPlayerCount;
    
    // Initialize all players to normal status
    for(int aid = 0; aid < 12; ++aid) {
        this->status[aid][0] = NORMAL;
        this->status[aid][1] = NORMAL;
    }
    this->ResetRace();
}

Mgr::~Mgr() {
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    // Restore original player counts
    controller->subs[0].localPlayerCount = this->baseLocPlayerCount;
    controller->subs[1].localPlayerCount = this->baseLocPlayerCount;
    if(this->GetIsSwapped()) this->SwapControllersAndUI();
}

// Race Statistics Management
void Mgr::AddRaceStats() {
    const RacedataScenario& scenario = Racedata::sInstance->racesScenario;
    const u8 localPlayerCount = scenario.localPlayerCount;
    
    if(localPlayerCount > 2) return;
    
    const u8 playerCount = System::sInstance->nonTTGhostPlayersCount;
    if(playerCount == 0) return;
    
    // Update stats for each local player
    for(int hudSlot = 0; hudSlot < localPlayerCount; ++hudSlot) {
        Stats& stats = this->stats[hudSlot];
        
        // Check if player was in danger of being KO'd
        if(stats.boolCountArray >= arbitraryAlmostDied) {
            ++stats.final.almostKOdCounter;
        }
        
        // Calculate position percentage
        const u8 pos = Raceinfo::sInstance->players[scenario.settings.hudPlayerIds[hudSlot]]->position;
        stats.percentageSum += static_cast<float>(pos) / static_cast<float>(playerCount);
        stats.final.finalPercentageSum = static_cast<u8>(stats.percentageSum * 100);
    }
    
    this->ResetRace();
}

void Mgr::CalcWouldBeKnockedOut() {
    const Raceinfo* raceInfo = Raceinfo::sInstance;
    const u8 playerCount = System::sInstance->nonTTGhostPlayersCount;
    const RacedataScenario& scenario = Racedata::sInstance->menusScenario;
    const u8* pointsArray = &Racedata::pointsRoom[playerCount - 1][0];

    // Initialize player positions array
    PlayerPosition players[12];
    for (u8 curPlayerId = 0; curPlayerId < playerCount; ++curPlayerId) {
        this->wouldBeOut[curPlayerId] = false;
        players[curPlayerId].playerId = curPlayerId;
        
        // Calculate position/points based on race mode
        if (this->racesPerKO > 1) {
            const u8 wouldBePoints = pointsArray[raceInfo->players[curPlayerId]->position - 1];
            players[curPlayerId].position = scenario.players[curPlayerId].score + wouldBePoints;
        } else {
            players[curPlayerId].position = raceInfo->players[curPlayerId]->position;
        }
    }

    // Special handling for 1v1 finals
    if (playerCount == 2) {
        for (int i = 0; i < playerCount; ++i) {
            this->wouldBeOut[i] = (raceInfo->players[i]->position != 1);
        }
        return;
    }

    // Check if this is a KO race
    const bool force1v1Final = System::sInstance->IsContext(Pulsar::PULSAR_KOFINAL) == KOSETTING_FINAL_ALWAYS;
    const u32 currentRaceCount = SectionMgr::sInstance->sectionParams->onlineParams.currentRaceNumber + 1;
    const bool isKoRace = currentRaceCount % this->racesPerKO == 0;

    if (isKoRace) {
        // Calculate how many players to KO
        s32 roundKOs = this->koPerRace;
        const s32 remainingPlayersAfter = playerCount - roundKOs;

        if (remainingPlayersAfter < 2 && force1v1Final) {
            roundKOs = playerCount - 2;
        }

        // Sort players by score/position (ascending)
        qsort(players, playerCount, sizeof(PlayerPosition), 
              reinterpret_cast<int(*)(const void*, const void*)>(SortPlayersByPosition));
    
        // Mark players for KO starting with lowest scores
        s32 assignedKOs = 0;
        for (s32 idx = 0; idx < playerCount && assignedKOs < roundKOs; ++idx) {
            if (this->racesPerKO == 1 && raceInfo->players[players[idx].playerId]->position == 1) {
                continue;
            }
            if (this->racesPerKO > 1 && idx == playerCount - 1) {
                continue;
            }
            this->wouldBeOut[players[idx].playerId] = true;
            ++assignedKOs;
        }
    }
}

void Mgr::ProcessKOs(Pages::GPVSLeaderboardUpdate::Player* playerArr, size_t nitems, size_t size, int (*compar)(const void*, const void*)) {
    qsort(playerArr, nitems, size, compar);

    const System* system = System::sInstance;
    if(!system->IsContext(PULSAR_MODE_KO)) return;

    Mgr* self = system->koMgr;
    RacedataScenario& scenario = Racedata::sInstance->menusScenario;
    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
    const u8 playerCount = system->nonTTGhostPlayersCount;

    // Handle disconnected players first
    u8 disconnectedKOs = 0;
    for(int playerId = 0; playerId < playerCount; ++playerId) {
        const u8 aid = controller->aidsBelongingToPlayerIds[playerId];
        if(aid >= 12) continue;
        
        if((1 << aid & sub.availableAids) == 0 && !self->IsKOdPlayerId(playerId)) {
            self->SetKOd(playerId);
            ++disconnectedKOs;
        }
    }

    // Calculate number of KOs for this round
    u8 theoreticalKOs = self->koPerRace;
    if(playerCount - self->koPerRace == 1 && 
       System::sInstance->IsContext(Pulsar::PULSAR_KOFINAL) == KOSETTING_FINAL_ALWAYS) {
        --theoreticalKOs; // Remove 1 KO if we need to keep 2 players for finals
    }

    s8 realKOCount = theoreticalKOs - disconnectedKOs;
    bool hasTies = false;

    // Adjust KO count for special cases
    if (self->koPerRace >= 2 && System::sInstance->IsContext(Pulsar::PULSAR_KOFINAL) == KOSETTING_FINAL_ALWAYS 
        && playerCount > 2) {
        if (playerCount == 3) {
            realKOCount = 1;
        } else if (playerCount == 4 && self->koPerRace >= 2) {
            realKOCount = 2;
        }
    } else {
        if (playerCount == 3 && self->koPerRace >= 3) {
            realKOCount = 2;
        } else if (playerCount == 4 && self->koPerRace >= 4) {
            realKOCount = 3;
        }
    }

    const Raceinfo* raceinfo = Raceinfo::sInstance;

    // Handle 1v1 finals or single player remaining
    if(playerCount == 2 || (playerCount - disconnectedKOs) == 1) {
        self->winnerPlayerId = raceinfo->playerIdInEachPosition[0];
        self->SetKOd(raceinfo->playerIdInEachPosition[1]);
    }
    // Process normal KOs
    else if(realKOCount > 0 && 
            (SectionMgr::sInstance->sectionParams->onlineParams.currentRaceNumber + 1) % self->racesPerKO == 0) {

        // Handle ties in multi-race KOs
        if (self->racesPerKO > 1) {
            u32 highestKOPosition = playerCount - realKOCount;
            u32 tieScore = playerArr[highestKOPosition].totalScore;
            
            // Count players involved in tie
            int tiedPlayersCount = 0;
            int playersInKOPosition = 0;
            int playersNotInKOPosition = 0;
            
            for (int position = 0; position < playerCount; ++position) {
                if (playerArr[position].totalScore == tieScore) {
                    ++tiedPlayersCount;
                    if (position >= playerCount - realKOCount) {
                        ++playersInKOPosition;
                    } else {
                        ++playersNotInKOPosition;
                    }
                }
            }

            // Handle tie resolution
            if (playersInKOPosition > 0 && playersNotInKOPosition > 0) {
                for (int position = 0; position < playerCount; ++position) {
                    if (playerArr[position].totalScore == tieScore) {
                        self->SetTie(playerArr[position].playerId, playerArr[highestKOPosition].playerId);
                        hasTies = true;
                    }
                }
                if (hasTies) {
                    realKOCount = 0;
                    --SectionMgr::sInstance->sectionParams->onlineParams.currentRaceNumber;
                }
            } else if (tiedPlayersCount == realKOCount) {
                for (int position = 0; position < playerCount; ++position) {
                    if (playerArr[position].totalScore == tieScore) {
                        self->SetKOd(playerArr[position].playerId);
                    }
                }
            }
        }

        // Reset scores if no ties and multiple races per KO
        if (realKOCount > 0 && self->racesPerKO > 1 && !hasTies) {
            for (int idx = 0; idx < 12; ++idx) {
                scenario.players[idx].score = 0;
                scenario.players[idx].previousScore = 0;
            }
        }

        // Process final KOs
        for(int idx = 0; idx < realKOCount; ++idx) {
            u8 playerId;
            u32 position = (playerCount - 1) - disconnectedKOs - idx;
            
            if(self->racesPerKO == 1) {
                playerId = raceinfo->playerIdInEachPosition[position];
                // Skip winner/first place, try next position
                if (playerId == self->winnerPlayerId || raceinfo->players[playerId]->position == 1) {
                    if(position > 0) {
                        playerId = raceinfo->playerIdInEachPosition[position - 1];
                        if(playerId != self->winnerPlayerId && raceinfo->players[playerId]->position != 1) {
                            self->SetKOd(playerId);
                        }
                    }
                    continue;
                }
            } else {
                playerId = playerArr[position].playerId;
            }
            
            if (self->racesPerKO > 1 && playerCount > 2) {
                if (playerId == self->winnerPlayerId) continue;
            }
            self->SetKOd(playerId);
        }
    }

    // Check for overall winner
    int notKOdCount = 0;
    u8 potentialWinner = 0xFF;
    for (int playerId = 0; playerId < playerCount; ++playerId) {
        if (!self->IsKOdPlayerId(playerId)) {
            ++notKOdCount;
            potentialWinner = playerId;
        }
    }
    if (notKOdCount == 1) {
        self->winnerPlayerId = potentialWinner;
    }
    
    self->AddRaceStats();
}
kmCall(0x8085cb94, Mgr::ProcessKOs);

void Mgr::Update() {
    const System* system = System::sInstance;
    if(!system->IsContext(PULSAR_MODE_KO)) return;

    Mgr* self = System::sInstance->koMgr;
    self->CalcWouldBeKnockedOut();
    
    const RacedataScenario& scenario = Racedata::sInstance->racesScenario;
    // Update danger status for each local player
    for(int hudSlot = 0; hudSlot < scenario.localPlayerCount; ++hudSlot) {
        const bool wouldBeOut = self->GetWouldBeKnockedOut(scenario.settings.hudPlayerIds[hudSlot]);
        const u32 idx = Raceinfo::sInstance->raceFrames % 300;

        Stats& stats = self->stats[hudSlot];
        if(wouldBeOut) ++stats.final.timeInDanger;
        
        // Update danger frame counters
        if(stats.isInDangerFrames[idx] == false && wouldBeOut == true) {
            ++stats.boolCountArray;
        } else if(stats.isInDangerFrames[idx] == true && wouldBeOut == false) {
            --stats.boolCountArray;
        }
        stats.isInDangerFrames[idx] = wouldBeOut;
    }

    // Handle winner packet sending
    const u8 winnerPlayerId = self->winnerPlayerId;
    if(winnerPlayerId != 0xFF) {
        const RKNet::Controller* controller = RKNet::Controller::sInstance;
        const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
        
        if(controller->aidsBelongingToPlayerIds[winnerPlayerId] == sub.localAid) {
            for(int aid = 0; aid < 12; ++aid) {
                if((1 << aid & sub.availableAids) == 0 || aid == sub.localAid) continue;

                Stats& stats = self->stats[0];
                RKNet::PacketHolder<Network::PulRH1>* holder = 
                    controller->GetSendPacketHolder<Network::PulRH1>(aid);

                Network::PulRH1* dest = holder->packet;
                dest->timeInDanger = stats.final.timeInDanger;
                dest->almostKOdCounter = stats.final.almostKOdCounter;
                dest->finalPercentageSum = stats.final.finalPercentageSum;
            }
        }
    }
}
RaceFrameHook koUpdate(Mgr::Update);

/*
void Mgr::UpdateStillInCount() {
    u32 stillIn = 0;
    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const u32 availableAids =  controller->subs[controller->currentSub].availableAids;
    for(int aid = 0; aid < 12; ++aid) {
        u8 aid = controller->aidsBelongingToPlayerIds[aid];
        if((1 << aid & availableAids) == 0) { //the playerId is disconnected
            this->SetMgrd(aid);
        }
        if(this->IsMgrd(aid)) continue;
        ++stillIn;
    }
    this->stillInCount = stillIn;
}
*/

void Mgr::PatchAids(RKNet::ControllerSub& sub) const {
    u32 availableAids = sub.availableAids;
    const u8 localAid = sub.localAid;
    
    for(u8 aid = 0; aid < 12; ++aid) {
        bool isConsoleOut = false;
        const bool isMainOut = this->IsKOdAid(aid, 0);
        u8 aidPlayerCount = aid == localAid ? 
            sub.localPlayerCount : sub.connectionUserDatas[aid].playersAtConsole;

        if(aidPlayerCount <= 1) {
            isConsoleOut = isMainOut;
        } else if(aidPlayerCount == 2) {
            const bool isGuestOut = this->IsKOdAid(aid, 1);
            if(isMainOut && isGuestOut) {
                isConsoleOut = true;
            } else if(isMainOut != isGuestOut) {
                aidPlayerCount = 1;
            }
        }

        if(isConsoleOut) {
            availableAids &= ~(1 << aid);
            aidPlayerCount = 0;
        }

        if(aid == localAid) {
            sub.localPlayerCount = aidPlayerCount;
        } else {
            sub.connectionUserDatas[aid].playersAtConsole = aidPlayerCount;
        }
    }
    sub.availableAids = availableAids;
}

u32 Mgr::GetAidAndSlotFromPlayerId(u8 playerId) const {
    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
    const u8 aid = controller->aidsBelongingToPlayerIds[playerId];
    const u8 localAid = sub.localAid;
    u8 slot = 0;

    if((aid == localAid && sub.localPlayerCount == 2) || 
       (aid != localAid && sub.connectionUserDatas[aid].playersAtConsole == 2)) {
        if(playerId > 0 && controller->aidsBelongingToPlayerIds[playerId - 1] == aid) {
            slot = 1;
        } else if(playerId < 11 && this->status[aid][0] == KOD && 
                 controller->aidsBelongingToPlayerIds[playerId + 1] != aid) {
            slot = 1;
        }
    }
    return (slot << 16) | aid;
}

SectionId Mgr::GetSectionAfterKO(SectionId defaultId) const {
    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
    
    if(this->baseLocPlayerCount == 2) {
        if(this->IsKOdAid(sub.localAid, 0) != this->IsKOdAid(sub.localAid, 1)) {
            if(defaultId == SECTION_P2_WIFI_FROOM_VS_VOTING) {
                defaultId = SECTION_P1_WIFI_FROOM_VS_VOTING;
            } else if(defaultId == SECTION_P1_WIFI_FROM_FROOM_RACE || 
                     defaultId == SECTION_P2_WIFI_FROM_FROOM_RACE) {
                defaultId = SECTION_P2_WIFI_FROM_FROOM_RACE;
                SectionMgr::sInstance->sectionParams->localPlayerCount = 2;
            }
        }
    }
    return defaultId;
}

void OnDisconnectKO(SectionMgr* sectionMgr, SectionId id) {
    const System* system = System::sInstance;
    if(system->IsContext(PULSAR_MODE_KO)) {
        id = system->koMgr->GetSectionAfterKO(id);
    }
    sectionMgr->SetNextSection(id, 0);
}
kmCall(0x80651814, OnDisconnectKO);

PageId Mgr::KickPlayersOut(PageId defaultId) {
    PageId ret = defaultId;
    const System* system = System::sInstance;
    Mgr* mgr = system->koMgr;
    RacedataScenario& scenario = Racedata::sInstance->racesScenario;
    const bool isMainOut = mgr->IsKOdPlayerId(scenario.settings.hudPlayerIds[0]);

    if(system->nonTTGhostPlayersCount > 2) {
        if(scenario.localPlayerCount == 1) {
            const RKNet::Controller* controller = RKNet::Controller::sInstance;
            const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
            if(isMainOut) {
                if(sub.localAid == sub.hostAid) {
                    mgr->isSpectating = true;
                } else {
                    ret = static_cast<PageId>(RaceEndPage::id);
                }
            }
        } else {
            const bool isGuestOut = mgr->IsKOdPlayerId(scenario.settings.hudPlayerIds[1]);
            if(isMainOut != isGuestOut) {
                SectionMgr::sInstance->sectionParams->localPlayerCount = 1;
            }
            if(isMainOut && !isGuestOut) {
                memcpy(&mgr->stats[0], &mgr->stats[1], sizeof(Mgr::Stats));
            } else if(isMainOut && isGuestOut) {
                ret = static_cast<PageId>(RaceEndPage::id);
            }
        }
    }
    return ret;
}

void Mgr::SwapControllersAndUI() {
    // Swap controllers
    Input::Manager* input = Input::Manager::sInstance;
    char mainController[sizeof(Input::RealControllerHolder)];
    memcpy(&mainController, &input->realControllerHolders[0], sizeof(Input::RealControllerHolder));
    memcpy(&input->realControllerHolders[0], &input->realControllerHolders[1], sizeof(Input::RealControllerHolder));
    memcpy(&input->realControllerHolders[1], &mainController, sizeof(Input::RealControllerHolder));

    // Swap pad info
    SectionMgr* sectionMgr = SectionMgr::sInstance;
    SectionPad& pad = sectionMgr->pad;
    PadInfo& main = pad.padInfos[0];
    PadInfo& guest = pad.padInfos[1];
    u32 oldMain = main.controllerID;
    u32 oldGuest = guest.controllerID;
    main.controllerID = oldGuest;
    guest.controllerID = oldMain;
    main.controllerIDActive = main.controllerID;
    guest.controllerIDActive = guest.controllerID;

    // Swap UI characters
    SectionParams* params = sectionMgr->sectionParams;
    CharacterId mainChar = params->characters[0];
    KartId mainKart = params->karts[0];
    params->characters[0] = params->characters[1];
    params->karts[0] = params->karts[1];
    params->characters[1] = mainChar;
    params->karts[1] = mainKart;
    memcpy(&params->combos[0], &params->combos[1], sizeof(PlayerCombo));
    
    this->hasSwapped = !this->hasSwapped;
}
}//namespace KO
}//namespace Pulsar