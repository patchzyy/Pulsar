#include <RetroRewind.hpp>
#include <MarioKartWii/Archive/ArchiveMgr.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <Network/SHA256.hpp>
#include <core/rvl/os/OS.hpp>

namespace RetroRewind {

struct FileHash {
    const char* filename;
    const char* expectedHash;
};

void CalculateKartParamHash(const void* data, u32 length, u8* hashOut) {
    SHA256Context ctx;
    SHA256Init(&ctx);
    SHA256Update(&ctx, data, length);
    memcpy(hashOut, SHA256Final(&ctx), SHA256_DIGEST_SIZE);
}

bool GetKartParamFileHash(ArchiveSource type, const char* filename, u8* hashOut) {
    u32 length;
    void* data = ArchiveMgr::sInstance->GetFile(type, filename, &length);
    if (!data) return false;
    
    CalculateKartParamHash(data, length, hashOut);
    return true;
}

void HashToString(const u8* hash, char* strOut, size_t strLen) {
    if (strLen < (SHA256_DIGEST_SIZE * 2 + 1)) return;
    
    static const char* hexChars = "0123456789abcdef";
    for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
        strOut[i * 2] = hexChars[hash[i] >> 4];
        strOut[i * 2 + 1] = hexChars[hash[i] & 0xf];
    }
    strOut[SHA256_DIGEST_SIZE * 2] = '\0';
}

// Known good hashes for each file
static const FileHash KART_PARAM_HASHES[] = {
    {"kartParam.bin", "fa3509c53a7f31d87db6c6632d88cbefe68bb7cfabf8cbb9fdbd3c0b7015c2f3"},
    {"kartParamAll.bin", "c76cb97c622bbe76cf228b3174d779d2a2af3f9c863d4ebdc3b93ba2925b693c"},
    {"kartParamOut.bin", "33e8bdda4a168dea424236d53cc35e520cb836d2a1f203ec770e713436e770b8"},
    {"kartParamBike.bin", "1c879da253baf8d46d48facc69adbd323870bcbdb8504afcfddf949340404ba1"}
};

static const FileHash ITEM_SLOT_HASHES[] = {
    {"ItemSlot.bin", "a0ff1c22bc0024e3746c6fb75538e512070d52aa72a65f7a2a27f24ab2c93175"},
    {"ItemSlotRR.bin", "e6ad1cde303054149c37ae45f0df247f95d9ee78d98c1126c879500d913a49f7"},
    {"ItemSlotRandom.bin", "113696914c23ddfc4d130fca10cc513e8bb5186961a00d3f764bd6de6169b351"},
    {"ItemSlotBlast.bin", "3aaa3af84f50b7ea0ffe34e37840346587d23ee3aba90167bf2d13061a3ff12d"}
};

bool VerifyFileHash(const char* filename, ArchiveSource type, const char* expectedHash) {
    u8 hash[SHA256_DIGEST_SIZE];
    char hashStr[SHA256_DIGEST_SIZE * 2 + 1];
    
    if (!GetKartParamFileHash(type, filename, hash)) {
        return false;
    }
    
    HashToString(hash, hashStr, sizeof(hashStr));
    return strcmp(hashStr, expectedHash) == 0;
}

void VerifyKartParamHashes() {
    for (size_t i = 0; i < sizeof(KART_PARAM_HASHES) / sizeof(KART_PARAM_HASHES[0]); i++) {
        const FileHash& file = KART_PARAM_HASHES[i];
        if (!VerifyFileHash(file.filename, ARCHIVE_HOLDER_COMMON, file.expectedHash)) {
            Pulsar::Debug::FatalError("Invalid kartParam hash detected. File integrity check failed.");
        }
    }
}
static RaceLoadHook KartParamHashCheck(VerifyKartParamHashes);

void VerifyItemSlotHashes() {
    for (size_t i = 0; i < sizeof(ITEM_SLOT_HASHES) / sizeof(ITEM_SLOT_HASHES[0]); i++) {
        const FileHash& file = ITEM_SLOT_HASHES[i];
        if (!VerifyFileHash(file.filename, ARCHIVE_HOLDER_COMMON, file.expectedHash)) {
            Pulsar::Debug::FatalError("Invalid ItemSlot hash detected. File integrity check failed.");
        }
    }
}
static RaceLoadHook ItemSlotHashCheck(VerifyItemSlotHashes);

void *GetCustomKartParam(ArchiveMgr *archive, ArchiveSource type, const char *name, u32 *length) {
    bool insideAll = Pulsar::HOSTSETTING_FORCE_TRANSMISSION_DEFAULT;
    bool outsideAll = Pulsar::HOSTSETTING_FORCE_TRANSMISSION_DEFAULT;
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST) {
        insideAll = System::sInstance->IsContext(Pulsar::PULSAR_TRANSMISSIONINSIDE) ? Pulsar::HOSTSETTING_FORCE_TRANSMISSION_INSIDE : Pulsar::HOSTSETTING_FORCE_TRANSMISSION_DEFAULT;
        outsideAll = System::sInstance->IsContext(Pulsar::PULSAR_TRANSMISSIONOUTSIDE) ? Pulsar::HOSTSETTING_FORCE_TRANSMISSION_OUTSIDE : Pulsar::HOSTSETTING_FORCE_TRANSMISSION_DEFAULT;
    }
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_VS_WW || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_BT_WW) {
        name = "kartParam.bin";
    } else {
        if (insideAll == Pulsar::HOSTSETTING_FORCE_TRANSMISSION_INSIDE) {
            name = "kartParamAll.bin";
        }
        else if (outsideAll == Pulsar::HOSTSETTING_FORCE_TRANSMISSION_OUTSIDE) {
            name = "kartParamOut.bin";
        }
        else if (static_cast<Pulsar::Transmission>(Pulsar::Settings::Mgr::Get().GetUserSettingValue(static_cast<Pulsar::Settings::UserType>(Pulsar::Settings::SETTINGSTYPE_RR), Pulsar::SETTINGRR_RADIO_TRANSMISSION)) == Pulsar::TRANSMISSION_INSIDEALL)
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
    }
    VerifyKartParamHashes();
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
    const RacedataScenario& scenario = Racedata::sInstance->racesScenario;
    const GameMode mode = scenario.settings.gamemode;
    bool itemModeRandom = Pulsar::GAMEMODE_DEFAULT;
    bool itemModeBlast = Pulsar::GAMEMODE_DEFAULT;
    bool itemModeNone = Pulsar::GAMEMODE_DEFAULT;
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST || mode == MODE_VS_RACE || mode == MODE_BATTLE) {
        itemModeRandom = System::sInstance->IsContext(Pulsar::PULSAR_ITEMMODERANDOM) ? Pulsar::GAMEMODE_RANDOM : Pulsar::GAMEMODE_DEFAULT;
        itemModeBlast = System::sInstance->IsContext(Pulsar::PULSAR_ITEMMODEBLAST) ? Pulsar::GAMEMODE_BLAST : Pulsar::GAMEMODE_DEFAULT;
        itemModeNone = System::sInstance->IsContext(Pulsar::PULSAR_ITEMMODENONE) ? Pulsar::GAMEMODE_NONE : Pulsar::GAMEMODE_DEFAULT;
    }
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_VS_WW || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_BT_WW) {
        itemModeNone = Pulsar::GAMEMODE_NONE;
    }
    if (itemModeRandom == Pulsar::GAMEMODE_DEFAULT || itemModeBlast == Pulsar::GAMEMODE_DEFAULT)
    {
        name="ItemSlotRR.bin";
    }
    if (itemModeNone == Pulsar::GAMEMODE_NONE)
    {
        name="ItemSlot.bin";
    }
    if (itemModeRandom == Pulsar::GAMEMODE_RANDOM)
    {
        name="ItemSlotRandom.bin";
    }
    if (itemModeBlast == Pulsar::GAMEMODE_BLAST)
    {
        name="ItemSlotBlast.bin";
    }
    VerifyItemSlotHashes();
    return archive->GetFile(type, name, length);
}
kmCall(0x807bb128, GetCustomItemSlot);
kmCall(0x807bb030, GetCustomItemSlot);
kmCall(0x807bb200, GetCustomItemSlot);
kmCall(0x807bb53c, GetCustomItemSlot);
kmCall(0x807bbb58, GetCustomItemSlot);
} // namespace RetroRewind