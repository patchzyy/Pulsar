#include <UI/ExtendedTeamSelect/ExtendedTeamManager.hpp>
#include <MarioKartWii/UI/Page/Other/FriendRoom.hpp>

namespace Pulsar {
namespace UI {

ExtendedTeamManager* ExtendedTeamManager::sInstance = nullptr;

ExtendedTeamManager::ExtendedTeamManager() {
    for (int i = 0; i < 12; i++) {
        this->players[i].idx = 0xFF;
        this->players[i].miiIdx = 0xFF;
        this->players[i].aid = 0xFF;
        this->players[i].playerIdOnConsole = 0xFF;
        this->players[i].team = static_cast<ExtendedTeamID>(i % TEAM_COUNT);
    }

    this->isHost = false;
    this->hasSentStartRacePacket = false;
    this->status = STATUS_NONE;
}

void ExtendedTeamManager::CreateInstance(ExtendedTeamManager* obj) {
    sInstance = obj;
}

void ExtendedTeamManager::DestroyInstance() {
    if (sInstance) {
        sInstance = nullptr;
    }
}

void ExtendedTeamManager::SendStartRacePacket() {
    Pages::FriendRoomManager *friendRoomManager = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    RKNet::ControllerSub* sub = &controller->subs[controller->currentSub];

    if (sub->localAid == sub->hostAid) {
        friendRoomManager->lastMessageId++;

        RKNet::ROOMPacket packet;
        packet.messageType = MSG_TYPE_START_RACE;
        packet.message = 0;
        packet.unknown_0x3 = friendRoomManager->lastMessageId;
            
         for (int i = 0; i < 12; ++i) {
            if (i != sub->localAid) {
                RKNet::ROOMHandler::sInstance->toSendPackets[i] = packet;
            }
        }

        friendRoomManager->lastSentPacket = packet;
        friendRoomManager->localAid = sub->localAid;

        this->status = STATUS_WAITING_POST;
    }
}

void ExtendedTeamManager::SendUpdateTeamsPacket() {
    Pages::FriendRoomManager *friendRoomManager = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    RKNet::ControllerSub* sub = &controller->subs[controller->currentSub];

    if (sub->localAid == sub->hostAid) {
        friendRoomManager->lastMessageId++;

        RKNet::ROOMPacket packet;
        packet.messageType = MSG_TYPE_UPDATE_TEAMS;
        packet.message = 0;
        packet.unknown_0x3 = friendRoomManager->lastMessageId;
            
        for (int i = 0; i < 12; ++i) {
            if (i != sub->localAid) {
                RKNet::ROOMHandler::sInstance->toSendPackets[i] = packet;
            }
        }

        friendRoomManager->lastSentPacket = packet;
        friendRoomManager->localAid = sub->localAid;
    }
}

void ExtendedTeamManager::SendPingPacket() {
    Pages::FriendRoomManager *friendRoomManager = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    RKNet::ControllerSub* sub = &controller->subs[controller->currentSub];

    friendRoomManager->lastMessageId++;

    RKNet::ROOMPacket packet;
    packet.messageType = MSG_TYPE_PING;
    packet.message = 0;
    packet.unknown_0x3 = friendRoomManager->lastMessageId;

    RKNet::ROOMHandler::sInstance->toSendPackets[sub->hostAid] = packet;

    friendRoomManager->lastSentPacket = packet;
    friendRoomManager->localAid = sub->localAid;
}

void ExtendedTeamManager::SendAckStartRacePacket() {
    Pages::FriendRoomManager *friendRoomManager = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    RKNet::ControllerSub* sub = &controller->subs[controller->currentSub];

    friendRoomManager->lastMessageId++;

    RKNet::ROOMPacket packet;
    packet.messageType = MSG_TYPE_ACK_START_RACE;
    packet.message = 0;
    packet.unknown_0x3 = friendRoomManager->lastMessageId;

    RKNet::ROOMHandler::sInstance->toSendPackets[sub->hostAid] = packet;

    friendRoomManager->lastSentPacket = packet;
    friendRoomManager->localAid = sub->localAid;
}

bool ExtendedTeamManager::AreAllOtherPlayersActive(u8 localAid) {
    for (int i = 0; i < 12; i++) {
        if (this->players[i].idx != 0xFF && this->players[i].aid != localAid && !this->players[i].active) {
            return false;
        }
    }
    return true;
}

bool ExtendedTeamManager::AreAllOtherPlayersDone(u8 localAid) {
    for (int i = 0; i < 12; i++) {
        if (this->players[i].idx != 0xFF && this->players[i].aid != localAid && !this->players[i].done) {
            return false;
        }
    }
    return true;
}

void ExtendedTeamManager::Update() {
    Pages::FriendRoomManager *friendRoomManager = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    this->isHost = RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST;
    u8 localAid = RKNet::Controller::sInstance->subs[RKNet::Controller::sInstance->currentSub].localAid;
    if (!this->isHost) {
        if (status == STATUS_NONE) {
            if (friendRoomManager->friendRoomIsEnding) {
                status = STATUS_SELECTING;
            }
        } else if (status == STATUS_SELECTING) {
            if (!this->lastUpdateTimer.isActive) {
                this->lastUpdateTimer.SetInitial(0.666f);
                this->lastUpdateTimer.isActive = true;

                this->SendPingPacket();
            }
        } else if (status == STATUS_DONE) {
            if (!this->lastUpdateTimer.isActive) {
                this->lastUpdateTimer.SetInitial(0.666f);
                this->lastUpdateTimer.isActive = true;

                this->SendAckStartRacePacket();
            }
        }
    } else {
        if (status == STATUS_NONE) {
            if (friendRoomManager->friendRoomIsEnding) {
                status = STATUS_WAITING_PRE;
                this->waitingTimer.SetInitial(5.0f);
                this->waitingTimer.isActive = true;
            }
        } else if (status == STATUS_WAITING_PRE) {
            if (!this->waitingTimer.isActive) {
                status = STATUS_SELECTING;
            } else {
                if (this->AreAllOtherPlayersActive(localAid)) {
                    status = STATUS_SELECTING;
                    this->waitingTimer.isActive = false;
                }
            }
        } else if (status == STATUS_SELECTING) {
            // ...
        } else if (status == STATUS_WAITING_POST) {
            if (!this->waitingTimer.isActive) {
                status = STATUS_DONE;
            } else {
                if (this->AreAllOtherPlayersDone(localAid)) {
                    status = STATUS_DONE;
                    this->waitingTimer.isActive = false;
                }
            }
        }
    }

    this->lastUpdateTimer.Update();
    this->waitingTimer.Update();
}

} // namespace UI
} // namespace Pulsar