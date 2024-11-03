#include <RetroRewind.hpp>
#include <MarioKartWii/Archive/ArchiveMgr.hpp>

namespace RetroRewind {
void *GetCustomKartParam(ArchiveMgr *archive, ArchiveSource type, const char *name, u32 *length){
    if (static_cast<Pulsar::Transmission>(Pulsar::Settings::Mgr::Get().GetSettingValue(static_cast<Pulsar::Settings::Type>(Pulsar::Settings::SETTINGSTYPE_RR), Pulsar::SETTINGRR_RADIO_TRANSMISSION)) == Pulsar::TRANSMISSION_INSIDEALL)
    {
        name="kartParamAll.bin";
    }
    else if (static_cast<Pulsar::Transmission>(Pulsar::Settings::Mgr::Get().GetSettingValue(static_cast<Pulsar::Settings::Type>(Pulsar::Settings::SETTINGSTYPE_RR), Pulsar::SETTINGRR_RADIO_TRANSMISSION)) == Pulsar::TRANSMISSION_INSIDEBIKE)
    {
        name="kartParamBike.bin";
    }
    else if (static_cast<Pulsar::Transmission>(Pulsar::Settings::Mgr::Get().GetSettingValue(static_cast<Pulsar::Settings::Type>(Pulsar::Settings::SETTINGSTYPE_RR), Pulsar::SETTINGRR_RADIO_TRANSMISSION)) == Pulsar::TRANSMISSION_OUTSIDE)
    {
        name="kartParamOut.bin";
    }
    return archive->GetFile(type, name, length);
}
kmCall(0x80591a30, GetCustomKartParam);

void *GetCustomKartAIParam(ArchiveMgr *archive, ArchiveSource type, const char *name, u32 *length){
    const GameMode gameMode = Racedata::sInstance->racesScenario.settings.gamemode;
    if (static_cast<Pulsar::HardAI>(Pulsar::Settings::Mgr::Get().GetSettingValue(static_cast<Pulsar::Settings::Type>(Pulsar::Settings::SETTINGSTYPE_RR), Pulsar::SETTINGRR_RADIO_HARDAI)) == Pulsar::HARDAI_ENABLED)
    {
        name="kartAISpdParamRR.bin";
    }
    
    return archive->GetFile(type, name, length);
}
kmCall(0x8073ae9c, GetCustomKartAIParam);
}