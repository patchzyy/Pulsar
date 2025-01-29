#include <UI/ExtendedTeamSelect/ExtendedTeamSelect.hpp>
#include <MarioKartWii/UI/Page/Other/FriendRoom.hpp>
#include <RetroRewind.hpp>
#include <core/nw4r/lyt/ArcResourceAccessor.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <MarioKartWii/RKNet/ROOM.hpp>
#include <MarioKartWii/UI/Page/Other/SELECTStageMgr.hpp>
#include <MarioKartWii/UI/Page/Other/VR.hpp>
#include <MarioKartWii/UI/Page/Other/Votes.hpp>
#include <MarioKartWii/UI/Ctrl/CtrlRace/CtrlRace2DMap.hpp>
#include <MarioKartWii/UI/Ctrl/CtrlRace/CtrlRaceBalloon.hpp>

namespace Pulsar {
namespace UI {

void PrepareOnlinePages(Pages::FriendRoomWaiting* _this) {
    _this->StartRoom();

    Pages::FriendRoomManager* friendRoomManager = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    if (System::sInstance->IsContext(PULSAR_EXTENDEDTEAMS) && friendRoomManager->startedGameMode == 0) {
        _this->countdown.SetInitial(86400.0f);
        _this->AddPageLayer(static_cast<PageId>(PULPAGE_EXTENDEDTEAMSELECT), 0);
    } else {
        _this->AddPageLayer(PAGE_CHARACTER_SELECT, 0);
    }
}

kmWrite32(0x8064be64, 0x38000000 + PULPAGE_EXTENDEDTEAMSELECT); // Patch regional button for easier debug
kmWriteNop(0x805de76c); // Remove the call that opens the character selection screen
kmCall(0x805dddb0, PrepareOnlinePages);

// This function is called when the host presses one of the start button in the Friend Room
// If Extended Teams is enabled, it will send a "Regular VS" message instead of a "Team VS" message
// Because I only patched the regular VS UIs for this mode
//
// The function looks odd because the compiler kept thinking that the packet was a pointer if you try
// to replace the "u32 pkt" by a "RKNet::ROOMPacket pkt", might be a skill issue on my own though
void SetBroadcastROOMPacket(RKNet::ROOMHandler* _this, u32 pkt) {
    
    u8 messageType = (pkt >> 24) & 0xFF;
    u16 message = (pkt >> 8) & 0xFFFF;
    u8 messageSequence = pkt & 0xFF;

    RKNet::Controller* controller = RKNet::Controller::sInstance;
    RKNet::ControllerSub* sub = &controller->subs[controller->currentSub];

    UI::ExtendedTeamSelect* extendedTeamSelect = SectionMgr::sInstance->curSection->Get<UI::ExtendedTeamSelect>();
    Pages::FriendRoomManager* friendRoomBackPage = SectionMgr::sInstance->curSection->Get<Pages::FriendRoomManager>();
    if (System::sInstance->IsContext(PULSAR_EXTENDEDTEAMS) && messageType == 1 && message == 1) {
        message = 0;
    }

    pkt = (messageType << 24) | (message << 8) | messageSequence;
    for (int i = 0; i < 12; ++i) {
        if (i != sub->localAid) {
            _this->toSendPackets[i] = *(RKNet::ROOMPacket*)&pkt;
        }
    }

    friendRoomBackPage->lastSentPacket = *(RKNet::ROOMPacket*)&pkt;
}

kmWriteNop(0x805dce38); // friendRoomBackPage->lastSentPacket = packet;
kmCall(0x805dce34, SetBroadcastROOMPacket);

void RecvRoomPacket(UnkFriendRoomManager* _this, u8 playerId, u8 myAid, RKNet::ROOMPacket& packet) {
    if (packet.messageType == ExtendedTeamManager::MSG_TYPE_START_RACE) {
        UI::ExtendedTeamManager::sInstance->SetStatusExternal(ExtendedTeamManager::STATUS_DONE);
    }

    _this->HandleROOMPacket(playerId, myAid, packet);
}
kmCall(0x805db0f8, RecvRoomPacket);
kmCall(0x805db1dc, RecvRoomPacket);

bool menuShouldPatchExtendedTeamsUI() {
    RKNet::RoomType roomType = RKNet::Controller::sInstance->roomType;
    return System::sInstance->IsContext(PULSAR_EXTENDEDTEAMS) && Racedata::sInstance->menusScenario.settings.gamemode == MODE_PRIVATE_VS && (roomType == RKNet::ROOMTYPE_FROOM_HOST || roomType == RKNet::ROOMTYPE_FROOM_NONHOST); 
}

void VotingVRPage_SetControlState(Pages::VR* _this, u32 idx, u32 playerId, Team team, u32 type, bool isLocalPlayer) {
    _this->FillVRControl(idx, playerId, team, type, isLocalPlayer);
    if (menuShouldPatchExtendedTeamsUI()) {
        Pages::SELECTStageMgr* selectStageMgr = SectionMgr::sInstance->curSection->Get<Pages::SELECTStageMgr>();
        u8 aid = selectStageMgr->infos[idx].aid;
        u8 playerIdOnConsole = selectStageMgr->infos[idx].hudSlotid;

        ExtendedTeamSelect::ChangeVRButtonColors(_this->vrControls[idx], ExtendedTeamManager::sInstance->GetPlayerTeamByAID(aid, playerIdOnConsole));
    }
}

kmCall(0x8064aa78, VotingVRPage_SetControlState);
kmCall(0x8064a9f0, VotingVRPage_SetControlState);

bool PageVote_FillVoteControl(Pages::Vote* _this, u32 playerId) {
    bool res = _this->FillVoteControl(playerId);
    if (menuShouldPatchExtendedTeamsUI() && res) {
        Pages::SELECTStageMgr* selectStageMgr = SectionMgr::sInstance->curSection->Get<Pages::SELECTStageMgr>();
        u8 aid = selectStageMgr->infos[playerId].aid;
        u8 playerIdOnConsole = selectStageMgr->infos[playerId].hudSlotid;

        VoteControl& control = _this->votes[_this->lastHandledVote - 1];
        control.animator.GetAnimationGroupById(3).PlayAnimationAtFrame(2, 0.0f);
        ExtendedTeamSelect::ChangeVRButtonColors(control, ExtendedTeamManager::sInstance->GetPlayerTeamByAID(aid, playerIdOnConsole));
    }

    return res;
}

kmCall(0x80643b3c, PageVote_FillVoteControl);

void CtrlRace2DMapCharacter_CalcTransform(CtrlRace2DMapCharacter* _this, const Vec3& kartPosition, Vec2& dest, u32 r6) {
    _this->CalculatePosition(kartPosition, dest, r6);
    if (menuShouldPatchExtendedTeamsUI()) {
        //_this->animator.GetAnimationGroupById(1).PlayAnimationAtFrameAndDisable(1, 0.0f); // shadow::red
        u8 r, g, b;
        ExtendedTeamSelect::GetTeamColor(ExtendedTeamManager::sInstance->GetPlayerTeam(_this->playerId), r, g, b);

        _this->SetPaneVisibility("chara_shadow_0_0", true);
        _this->SetPaneVisibility("chara_shadow_0_1", true);
        nw4r::lyt::Material* mat1 = _this->layout.GetPaneByName("chara_shadow_0_0")->GetMaterial();
        nw4r::lyt::Material* mat2 = _this->layout.GetPaneByName("chara_shadow_0_1")->GetMaterial();

        for (int i = 0; i < 2; i++) {
            mat1->tevColours[i].r = mat2->tevColours[i].r = r;
            mat1->tevColours[i].g = mat2->tevColours[i].g = g;
            mat1->tevColours[i].b = mat2->tevColours[i].b = b;
            mat1->tevColours[i].a = mat2->tevColours[i].a = 255;
        }

        // TODO: If self team, make the character icon non-transparent and appear on top of others
    }
}

void CtrlRace2DMapCharacter_PlayAnimationAtFrameAndDisable(AnimationGroup* _this, u32 id, float frame) {
    if (menuShouldPatchExtendedTeamsUI()) {
        _this->isActive = false;
    } else {
        _this->PlayAnimationAtFrameAndDisable(id, frame);
    }
}

kmCall(0x807eb308, CtrlRace2DMapCharacter_CalcTransform);
kmCall(0x807eaf1c, CtrlRace2DMapCharacter_PlayAnimationAtFrameAndDisable);
kmCall(0x807eb9d4, CtrlRace2DMapCharacter_PlayAnimationAtFrameAndDisable);

void CtrlRaceNameBalloon_refresh(CtrlRaceNameBalloon* _this, u8 playerId) {
    _this->UpdateInfo(playerId);
    if (menuShouldPatchExtendedTeamsUI()) {
        //_this->animator.GetAnimationGroupById(1).PlayAnimationAtFrameAndDisable(1, 0.0f); // shadow::red
        u8 r, g, b;
        ExtendedTeamSelect::GetTeamColor(ExtendedTeamManager::sInstance->GetPlayerTeam(playerId), r, g, b);
        nw4r::lyt::TextBox* characterName = (nw4r::lyt::TextBox*)_this->layout.GetPaneByName("chara_name");

        characterName->color1[0] = characterName->color1[1] = nw4r::ut::Color(r, g, b, 255);
    }
}

void CtrlRaceNameBalloon_PlayAnimationAtFrameAndDisable(AnimationGroup* _this, u32 id, float frame) {
    if (menuShouldPatchExtendedTeamsUI()) {
        _this->isActive = false;
    } else {
        _this->PlayAnimationAtFrameAndDisable(id, frame);
    }
}

kmCall(0x807f0c48, CtrlRaceNameBalloon_refresh);
kmCall(0x807f00f8, CtrlRaceNameBalloon_PlayAnimationAtFrameAndDisable);
kmCall(0x807efe5c, CtrlRaceNameBalloon_PlayAnimationAtFrameAndDisable);

void CtrlRaceResult_FillResult(CtrlRaceResult* _this, u8 position, u8 playerId) {
    _this->Fill(position, playerId);
    if (true) {
        _this->SetPaneVisibility("team_color_c", true);
        _this->SetPaneVisibility("select_base", true);

        nw4r::lyt::Material* mat;
        if (Racedata::sInstance->racesScenario.players[playerId].playerType == PLAYER_REAL_LOCAL) {
            //_this->animator.GetAnimationGroupById(4).PlayAnimationAtFrame(4, 0.0f);
            mat = _this->layout.GetPaneByName("select_base")->GetMaterial();
        } else {
            
            mat = _this->layout.GetPaneByName("select_base")->GetMaterial();
        }

        u8 r, g, b;
        ExtendedTeamSelect::GetTeamColor((ExtendedTeamID)(playerId % 6), r, g, b);

        for (int i = 0; i < 2; i++) {
            mat->tevColours[i].r = r;
            mat->tevColours[i].g = g;
            mat->tevColours[i].b = b;
            mat->tevColours[i].a = 255;
        }

        AnimationGroup& animGroup = _this->animator.GetAnimationGroupById(4);
        OS::Report("Setting color %02x%02x%02x for Player %02d (%d %d %.1f)\n", r, g, b, playerId, animGroup.curAnimation, animGroup.isActive, animGroup.curFrame);
    }
}

void CtrlRaceResult_PlayAnimationAtFrame(AnimationGroup* _this, u32 id, float frame) {
    if (true) {
        _this->isActive = false;
    } else {
        _this->PlayAnimationAtFrame(id, frame);
    }
}

kmCall(0x8085c8cc, CtrlRaceResult_FillResult);
kmCall(0x8085cbd8, CtrlRaceResult_FillResult);
kmCall(0x8085d2fc, CtrlRaceResult_FillResult);

kmCall(0x807f63f8, CtrlRaceResult_PlayAnimationAtFrame);
kmCall(0x807f63a4, CtrlRaceResult_PlayAnimationAtFrame);
kmCall(0x807f6378, CtrlRaceResult_PlayAnimationAtFrame);
kmCall(0x807f4f8c, CtrlRaceResult_PlayAnimationAtFrame);

kmCall(0x807f63c0, CtrlRaceResult_PlayAnimationAtFrame);
kmCall(0x807f6410, CtrlRaceResult_PlayAnimationAtFrame);
kmCall(0x807f6318, CtrlRaceResult_PlayAnimationAtFrame);
kmCall(0x807f4f74, CtrlRaceResult_PlayAnimationAtFrame);

} // namespace UI
} // namespace Pulsar


asmFunc InstantFinish() {
    ASM(
  lis       r12, 0x3;
  ori       r12, r12, 0x387;
  stw       r12, 0x24(r30);
  lha       r3, 0x24(r30);
    )
}
kmCall(0x80535508, InstantFinish);