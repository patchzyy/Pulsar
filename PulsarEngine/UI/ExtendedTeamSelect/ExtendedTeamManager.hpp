#ifndef __EXTENDED_TEAMSELECT_HPP__
#define __EXTENDED_TEAMSELECT_HPP__

#include <UI/UI.hpp>
#include <MarioKartwii/UI/Ctrl/CountDown.hpp>

namespace Pulsar {
namespace UI {

enum ExtendedTeamID {
    TEAM_RED,
    TEAM_ORANGE,
    TEAM_YELLOW,
    TEAM_GREEN,
    TEAM_BLUE,
    TEAM_PURPLE,
    TEAM_COUNT
};
struct ExtendedTeamPlayer {
    u32 idx;
    u32 miiIdx;
    u32 aid;
    u32 playerIdOnConsole;
    u32 active;
    u32 done;
    ExtendedTeamID team;
};

class ExtendedTeamManager {
public:
    enum ExtendedROOMMessageType {
        MSG_TYPE_START_RACE = 0x81,
        MSG_TYPE_UPDATE_TEAMS = 0x82,
        MSG_TYPE_PING = 0x83,
        MSG_TYPE_ACK_START_RACE = 0x84
    };

    enum Status {
        STATUS_NONE,
        STATUS_WAITING_PRE,
        STATUS_SELECTING,
        STATUS_WAITING_POST,
        STATUS_DONE
    };

    static ExtendedTeamManager* sInstance;
    static void CreateInstance(ExtendedTeamManager* obj);
    static void DestroyInstance();

    ExtendedTeamManager();

    void SetPlayerTeam(u32 idx, ExtendedTeamID team) {
        this->players[idx].team = team;
    }

    void SetPlayerIndexes(u32 idx, u32 miiIdx, u32 aid, u32 playerIdOnConsole) {
        this->players[idx].idx = idx;
        this->players[idx].miiIdx = miiIdx;
        this->players[idx].aid = aid;
        this->players[idx].playerIdOnConsole = playerIdOnConsole;
    }

    ExtendedTeamID GetPlayerTeam(u32 idx) {
        return this->players[idx].team;
    }

    ExtendedTeamID GetPlayerTeamByAID(u8 aid, u8 playerIdOnConsole) {
        for (int i = 0; i < 12; i++) {
            if (this->players[i].aid == aid && this->players[i].playerIdOnConsole == playerIdOnConsole) {
                return this->players[i].team;
            }
        }
        return TEAM_COUNT;
    }

    u8 GetPlayerAID(u32 idx, u8 playerIdOnConsole) {
        return this->players[idx].aid;
    }

    const ExtendedTeamPlayer* GetPlayerInfo() const {
        return this->players;
    }

    void SendStartRacePacket();     // Host
    void SendUpdateTeamsPacket();   // Host
    void SendPingPacket();          // Non-Host
    void SendAckStartRacePacket();  // Non-Host

    void Update();

    bool IsWaitingStatus() {
        return this->status == STATUS_WAITING_PRE || this->status == STATUS_WAITING_POST;
    }

    bool IsSelectingStatus() {
        return this->status == STATUS_SELECTING;
    }

    bool IsDoneStatus() {
        return this->status == STATUS_DONE;
    }

    void SetActiveStatusForAID(u8 aid) {
        for (int i = 0; i < 12; i++) {
            if (this->players[i].aid == aid) {
                this->players[i].active = 1;
            }
        }
    }

    void SetDoneStatusForAID(u8 aid) {
        for (int i = 0; i < 12; i++) {
            if (this->players[i].aid == aid) {
                this->players[i].done = 1;
            }
        }
    }

    void SetStatusExternal(Status status) {
        this->status = status;
    }

private:

    bool AreAllOtherPlayersActive(u8 localAid);
    bool AreAllOtherPlayersDone(u8 localAid);

    ExtendedTeamPlayer players[12];
    CountDown waitingTimer;
    CountDown lastUpdateTimer;

    bool isHost;
    bool hasSentStartRacePacket;

    Status status;
};

} // namespace UI
} // namespace Pulsar

#endif