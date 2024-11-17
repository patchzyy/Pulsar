#include <RetroRewind.hpp>
#include <MarioKartWii/Archive/ArchiveMgr.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>

namespace RetroRewind {
void *GetCustomKartParam(ArchiveMgr *archive, ArchiveSource type, const char *name, u32 *length){
    if (static_cast<Pulsar::Transmission>(Pulsar::Settings::Mgr::Get().GetUserSettingValue(static_cast<Pulsar::Settings::UserType>(Pulsar::Settings::SETTINGSTYPE_RR), Pulsar::SETTINGRR_RADIO_TRANSMISSION)) == Pulsar::TRANSMISSION_INSIDEALL)
    {
        name="kartParamAll.bin";
    }
    else if (static_cast<Pulsar::Transmission>(Pulsar::Settings::Mgr::Get().GetUserSettingValue(static_cast<Pulsar::Settings::UserType>(Pulsar::Settings::SETTINGSTYPE_RR), Pulsar::SETTINGRR_RADIO_TRANSMISSION)) == Pulsar::TRANSMISSION_INSIDEBIKE)
    {
        name="kartParamBike.bin";
    }
    else if (static_cast<Pulsar::Transmission>(Pulsar::Settings::Mgr::Get().GetUserSettingValue(static_cast<Pulsar::Settings::UserType>(Pulsar::Settings::SETTINGSTYPE_RR), Pulsar::SETTINGRR_RADIO_TRANSMISSION)) == Pulsar::TRANSMISSION_OUTSIDE)
    {
        name="kartParamOut.bin";
    }
    return archive->GetFile(type, name, length);
}
kmCall(0x80591a30, GetCustomKartParam);

void *GetCustomKartAIParam(ArchiveMgr *archive, ArchiveSource type, const char *name, u32 *length){
    const GameMode gameMode = Racedata::sInstance->racesScenario.settings.gamemode;
    if (static_cast<Pulsar::HardAI>(Pulsar::Settings::Mgr::Get().GetUserSettingValue(static_cast<Pulsar::Settings::UserType>(Pulsar::Settings::SETTINGSTYPE_RR), Pulsar::SETTINGRR_RADIO_HARDAI)) == Pulsar::HARDAI_ENABLED)
    {
        name="kartAISpdParamRR.bin";
    }
    
    return archive->GetFile(type, name, length);
}
kmCall(0x8073ae9c, GetCustomKartAIParam);

void *GetCustomItemSlot(ArchiveMgr *archive, ArchiveSource type, const char *name, u32 *length){
    bool itemModeRandom = Pulsar::GAMEMODE_DEFAULT;
    bool itemModeBlast = Pulsar::GAMEMODE_DEFAULT;
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST) {
        itemModeRandom = System::sInstance->IsContext(Pulsar::PULSAR_ITEMMODERANDOM) ? Pulsar::GAMEMODE_RANDOM : Pulsar::GAMEMODE_DEFAULT;
        itemModeBlast = System::sInstance->IsContext(Pulsar::PULSAR_ITEMMODEBLAST) ? Pulsar::GAMEMODE_BLAST : Pulsar::GAMEMODE_DEFAULT;
    }
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_VS_WW || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_BT_WW) {
        itemModeRandom = Pulsar::GAMEMODE_NONE;
        itemModeBlast = Pulsar::GAMEMODE_NONE;
    }
    if (itemModeRandom == Pulsar::GAMEMODE_DEFAULT || itemModeBlast == Pulsar::GAMEMODE_DEFAULT)
    {
        name="ItemSlotRR.bin";
    }
    if (itemModeRandom == Pulsar::GAMEMODE_RANDOM)
    {
        name="ItemSlotRandom.bin";
    }
    if (itemModeBlast == Pulsar::GAMEMODE_BLAST)
    {
        name="ItemSlotBlast.bin";
    }
    return archive->GetFile(type, name, length);

}
kmCall(0x807bb128, GetCustomItemSlot);
kmCall(0x807bb030, GetCustomItemSlot);
kmCall(0x807bb200, GetCustomItemSlot);
kmCall(0x807bb53c, GetCustomItemSlot);
kmCall(0x807bbb58, GetCustomItemSlot);
}