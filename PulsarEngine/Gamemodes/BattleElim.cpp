#include <RetroRewind.hpp>
#include <runtimeWrite.hpp>
#include <MarioKartWii/Race/Racedata.hpp>
#include <MarioKartWii/Kart/KartManager.hpp>
#include <MarioKartWii/Race/RaceInfo/RaceInfo.hpp>
#include <MarioKartWii/UI/Ctrl/CtrlRace/CtrlRaceTime.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>

namespace Pulsar {
namespace BattleElim {

extern "C" volatile unsigned int gBattleElimFlag = 0;
extern "C" volatile unsigned int gBattleElimMakeInvisible = 0;
extern "C" volatile unsigned int gBattleElimRemaining = 0;
extern "C" volatile void* gBattleElimStorePtr = 0;
extern "C" volatile unsigned short gBattleElimElimOrder[12] = {0};
extern "C" volatile unsigned short gBattleElimEliminations = 0;
extern "C" volatile unsigned int gBattleElimWinnersAssigned = 0;

static void ResetBattleElimState() {
    const RacedataScenario& scenario = Racedata::sInstance->menusScenario;
    const GameMode mode = scenario.settings.gamemode;
    bool isElim = RACESETTING_ELIMINATION_DISABLED;
    if ((RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_NONE) && (mode == MODE_BATTLE || mode == MODE_PRIVATE_BATTLE || mode == MODE_PUBLIC_BATTLE) && System::sInstance->IsContext(PULSAR_TEAM_BATTLE) == BATTLE_TEAMS_DISABLED) {
        isElim = Pulsar::System::sInstance->IsContext(PULSAR_ELIMINATION) ? RACESETTING_ELIMINATION_ENABLED : RACESETTING_ELIMINATION_DISABLED;
    }
    if ((isElim || (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_BT_REGIONAL && System::sInstance->netMgr.region == 0x0F)) && RKNet::Controller::sInstance->roomType != RKNet::ROOMTYPE_VS_REGIONAL && RKNet::Controller::sInstance->roomType != RKNet::ROOMTYPE_VS_WW) {
        gBattleElimFlag = 0;
        gBattleElimMakeInvisible = 0;
        u8 playerCount = 0;
        if (System::sInstance) {
            gBattleElimRemaining = System::sInstance->nonTTGhostPlayersCount;
            playerCount = System::sInstance->nonTTGhostPlayersCount;
        } else {
            gBattleElimRemaining = 0;
        }
        (void)playerCount;
        gBattleElimEliminations = 0;
        gBattleElimWinnersAssigned = 0;
        for (int i = 0; i < 12; ++i) gBattleElimElimOrder[i] = 0;
    } else {
        return;
    }
}
static RaceLoadHook sBattleElimResetHook(ResetBattleElimState);

asmFunc ForceInvisible() {
    ASM(
        nofralloc;
        mflr r11;
        mr r12, r3;
        lis r9, gBattleElimStorePtr @ha;
        lwz r9, gBattleElimStorePtr @l(r9);
        cmpwi r9, 0;
        beq do_original;
        lwz r10, 0x48(r9);
        cmpwi r10, 0;
        bne do_original;
        bl loc_after;
        cmpw cr7, r0, r0;
        cmpw cr7, r0, r0;
        cmpw cr7, r0, r0;
        loc_after : mflr r3;
        lfs f0, 0(r3);
        mtlr r11;
        blr;
        do_original : lfs f0, 0(r12);
        mtlr r11;
        blr;)
}

static void BattleElimRemainingUpdate() {
    const RacedataScenario& scenario = Racedata::sInstance->menusScenario;
    const GameMode mode = scenario.settings.gamemode;
    bool isElim = RACESETTING_ELIMINATION_DISABLED;
    if ((RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_NONE) && (mode == MODE_BATTLE || mode == MODE_PRIVATE_BATTLE || mode == MODE_PUBLIC_BATTLE) && System::sInstance->IsContext(PULSAR_TEAM_BATTLE) == BATTLE_TEAMS_DISABLED) {
        isElim = Pulsar::System::sInstance->IsContext(PULSAR_ELIMINATION) ? RACESETTING_ELIMINATION_ENABLED : RACESETTING_ELIMINATION_DISABLED;
    }
    if ((isElim || (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_BT_REGIONAL && System::sInstance->netMgr.region == 0x0F)) && RKNet::Controller::sInstance->roomType != RKNet::ROOMTYPE_VS_REGIONAL && RKNet::Controller::sInstance->roomType != RKNet::ROOMTYPE_VS_WW) {
        const System* sys = System::sInstance;
        Raceinfo* ri = Raceinfo::sInstance;
        if (!sys || !ri) return;
        const u8 total = sys->nonTTGhostPlayersCount;
        if (total <= 1) return;

        const RKNet::Controller* controller = RKNet::Controller::sInstance;
        const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
        const u32 availableAids = sub.availableAids;
        u32 activeNotFinished = 0;
        u32 actualFinished = 0;
        u32 disconnectedCount = 0;

        for (u8 pid = 0; pid < total; ++pid) {
            const bool playerFinished = CtrlRaceTime::HasPlayerFinished(pid);
            const u8 aid = controller->aidsBelongingToPlayerIds[pid];
            const bool disconnected = (aid >= 12) || ((availableAids & (1 << aid)) == 0);
            if (playerFinished) {
                ++actualFinished;
                if (gBattleElimElimOrder[pid] == 0) {
                    unsigned short elimNum = ++gBattleElimEliminations;  // first elimination becomes 1, etc.
                    gBattleElimElimOrder[pid] = elimNum;
                    if (ri && ri->players && ri->players[pid]) ri->players[pid]->battleScore = elimNum - 1;  // freeze at pre-elim survivor count
                }
            } else if (disconnected) {
                ++disconnectedCount;
                if (gBattleElimElimOrder[pid] == 0) {
                    unsigned short elimNum = ++gBattleElimEliminations;
                    gBattleElimElimOrder[pid] = elimNum;
                    if (ri && ri->players && ri->players[pid]) ri->players[pid]->battleScore = elimNum - 1;
                }
            } else {
                ++activeNotFinished;
            }
        }

        gBattleElimRemaining = activeNotFinished;

        if (ri && ri->players) {
            for (u8 pid = 0; pid < total; ++pid) {
                RaceinfoPlayer* pl = ri->players[pid];
                if (!pl) continue;
                unsigned short elimOrd = gBattleElimElimOrder[pid];
                if (elimOrd > 0) {
                    pl->battleScore = elimOrd - 1;
                } else {
                    pl->battleScore = gBattleElimEliminations;
                }
            }
        }

        if (activeNotFinished <= 1 && actualFinished > 0) {
            gBattleElimFlag = 1;
        }
    } else {
        return;
    }
}
static RaceFrameHook sBattleElimRemainingHook(BattleElimRemainingUpdate);

asmFunc OnBattleRespawn() {
    ASM(
        lis r12, gBattleElimFlag @ha;
        lwz r11, gBattleElimFlag @l(r12);
        cmpwi r11, 0;
        bne alreadySet;
        li r11, 1;
        stw r11, gBattleElimFlag @l(r12);
        alreadySet :;
        lha r4, 0x02d6(r31);
        blr;)
}

asmFunc ForceTimerOnStore() {
    ASM(
        lis r12, gBattleElimFlag @ha;
        lwz r11, gBattleElimFlag @l(r12);
        cmpwi r11, 0;
        beq original;
        li r11, 1;
        stb r11, 0x40(r29);
        li r0, 0;
        stw r0, 0x48(r29);
        lis r12, gBattleElimStorePtr @ha;  // save store pointer for ForceInvisible
        stw r29, gBattleElimStorePtr @l(r12);
        lis r12, gBattleElimFlag @ha;
        li r10, 0;
        stw r10, gBattleElimFlag @l(r12);
        blr;
        original : stw r0, 0x48(r29);
        lis r12, gBattleElimStorePtr @ha;
        stw r29, gBattleElimStorePtr @l(r12);  // still record pointer
        blr;)
}

asmFunc BattleElimScore() {
    ASM(
        mflr r12;

        lis r11, gBattleElimElimOrder @ha;
        lbz r9, 0x8(r3);
        addi r11, r11, gBattleElimElimOrder @l;
        slwi r10, r9, 1;
        lhzx r8, r11, r10;
        cmpwi r8, 0;
        beq checkWinner;
        addi r8, r8, -1;
        sth r8, 0x22(r3);
        b finish;
        checkWinner : lis r4, gBattleElimEliminations @ha;
        lhz r4, gBattleElimEliminations @l(r4);
        sth r4, 0x22(r3);
        b finish;
        writeOriginal : sth r0, 0x22(r3);
        finish : mtlr r12;
        blr;)
}

asmFunc ForceBalloonBattle() {
    ASM(
        oris r0, r0, 0x8000;
        xoris r0, r0, 0;
        stw r0, 0x8(r1);
        lwz r3, 0x0(r31);)
}

kmRuntimeUse(0x80579C1C);  // OnBattleRespawn
kmRuntimeUse(0x80535C7C);  // ForceTimerOnStore
kmRuntimeUse(0x806619AC);  // ForceBalloonBattle
kmRuntimeUse(0x80539878);  // BattleElimScore
kmRuntimeUse(0x8058CB7C);  // ForceInvisible
void BattleElim() {
    kmRuntimeWrite32A(0x80579C1C, 0xa89f02d6);
    kmRuntimeWrite32A(0x80535C7C, 0x901d0048);
    kmRuntimeWrite32A(0x806619AC, 0x807f0000);
    kmRuntimeWrite32A(0x80539878, 0xB0040022);
    kmRuntimeWrite32A(0x8058CB7C, 0xc0030000);
    const RacedataScenario& scenario = Racedata::sInstance->menusScenario;
    const GameMode mode = scenario.settings.gamemode;
    bool isElim = RACESETTING_ELIMINATION_DISABLED;
    if ((RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_NONE) && (mode == MODE_BATTLE || mode == MODE_PRIVATE_BATTLE || mode == MODE_PUBLIC_BATTLE) && System::sInstance->IsContext(PULSAR_TEAM_BATTLE) == BATTLE_TEAMS_DISABLED) {
        isElim = Pulsar::System::sInstance->IsContext(PULSAR_ELIMINATION) ? RACESETTING_ELIMINATION_ENABLED : RACESETTING_ELIMINATION_DISABLED;
    }
    if ((isElim || (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_BT_REGIONAL && System::sInstance->netMgr.region == 0x0F)) && RKNet::Controller::sInstance->roomType != RKNet::ROOMTYPE_VS_REGIONAL && RKNet::Controller::sInstance->roomType != RKNet::ROOMTYPE_VS_WW) {
        kmRuntimeCallA(0x80579C1C, OnBattleRespawn);
        kmRuntimeCallA(0x80535C7C, ForceTimerOnStore);
        kmRuntimeCallA(0x806619AC, ForceBalloonBattle);
        kmRuntimeCallA(0x80539878, BattleElimScore);
        kmRuntimeCallA(0x8058CB7C, ForceInvisible);
    }
}
static PageLoadHook BattleElimHook(BattleElim);

}  // namespace BattleElim
}  // namespace Pulsar