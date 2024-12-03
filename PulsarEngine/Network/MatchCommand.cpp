#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <MarioKartWii/UI/Ctrl/UIControl.hpp>
#include <Network/MatchCommand.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {
namespace Network {

ResvPacket::ResvPacket(const DWC::Reservation& src) {
    memcpy(this, &src, sizeof(DWC::Reservation));

    const System* system = System::sInstance;
    const Mgr& mgr = system->netMgr;
    pulInfo.statusData = mgr.ownStatusData;
    pulInfo.roomKey = system->GetInfo().GetKey();
    strncpy(pulInfo.modFolderName, system->GetModFolder(), IOS::ipcMaxFileName);
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_VS_WW || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_BT_WW) {
        u32 value = 123456789;

    // Split the value into the userInfo array
    pulInfo.userInfo.info[0] = (value & 0xFF);           // First 8 bits
    pulInfo.userInfo.info[1] = (value >> 8) & 0xFF;      // Next 8 bits
    pulInfo.userInfo.info[2] = (value >> 16) & 0xFF;     // Next 8 bits
    pulInfo.userInfo.info[3] = (value >> 24) & 0xFF;     // Last 8 bits
    }
}

asmFunc MoveSize() { //needed to get datasize later
    ASM(
        nofralloc;
    mr r25, r28;
    li r28, 255;
    blr;
        )
}
kmCall(0x800dc3bc, MoveSize);

DWC::MatchCommand Process(DWC::MatchCommand type, const void* data, u32 dataSize) {
    const RKNet::RoomType roomType = RKNet::Controller::sInstance->roomType;
    const bool isCustom = roomType == RKNet::ROOMTYPE_FROOM_NONHOST || roomType == RKNet::ROOMTYPE_FROOM_HOST
        || roomType == RKNet::ROOMTYPE_VS_REGIONAL || roomType == RKNet::ROOMTYPE_JOINING_REGIONAL;
    const bool isReg = roomType == RKNet::ROOMTYPE_VS_WW || roomType == RKNet::ROOMTYPE_JOINING_WW;

    Pulsar::System* system = Pulsar::System::sInstance;
    Mgr& mgr = system->netMgr;
    DenyType denyType = DENY_TYPE_NORMAL;

    if(type == DWC::MATCH_COMMAND_RESV_OK && isCustom) {
        const ResvPacket* packet = reinterpret_cast<const ResvPacket*>(data);
        if(dataSize != (sizeof(ResvPacket) / sizeof(u32)) || packet->pulInfo.roomKey != system->GetInfo().GetKey()
            || strcmp(packet->pulInfo.modFolderName, system->GetModFolder()) != 0
            || !system->CheckUserInfo(packet->pulInfo.userInfo)) {

            DenyType denyType = DENY_TYPE_BAD_PACK;
            if(roomType == RKNet::ROOMTYPE_VS_REGIONAL) mgr.deniesCount++;
            type = DWC::MATCH_COMMAND_RESV_DENY;
        }
        else if(roomType == RKNet::ROOMTYPE_VS_REGIONAL) {
            if(packet->pulInfo.statusData != mgr.ownStatusData) {
                denyType = DENY_TYPE_OTT;
                type = DWC::MATCH_COMMAND_RESV_DENY;
            }
        }
    }
    
    else if(type == DWC::MATCH_COMMAND_RESV_OK && isReg) {
        const ResvPacket* packet = reinterpret_cast<const ResvPacket*>(data);

        // Recombine the value from the userInfo array
        u32 userInfoValue = (packet->pulInfo.userInfo.info[0]) | 
                            (packet->pulInfo.userInfo.info[1] << 8) |
                            (packet->pulInfo.userInfo.info[2] << 16) |
                            (packet->pulInfo.userInfo.info[3] << 24);

        if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_VS_WW || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_BT_WW) {
            u32 expectedKey = 123456789;
        if (userInfoValue != expectedKey) {  // Host denies the player if the key doesn't match
            DenyType denyType = DENY_TYPE_BAD_PACK;
            if(roomType == RKNet::ROOMTYPE_VS_REGIONAL || roomType == RKNet::ROOMTYPE_VS_WW) mgr.deniesCount++;
            type = DWC::MATCH_COMMAND_RESV_DENY;
        }
        else if(roomType == RKNet::ROOMTYPE_VS_REGIONAL || roomType == RKNet::ROOMTYPE_VS_WW) {
            if(packet->pulInfo.statusData != mgr.ownStatusData) {
                denyType = DENY_TYPE_OTT;
                type = DWC::MATCH_COMMAND_RESV_DENY;
                }
            }
        }
    }
    mgr.denyType = denyType;
    return type;
}


static int GetSuspendType(int r3, const char* string) {
    DWC::Printf(r3, string);
    const u32 errorType = 0x120000000 + Pulsar::System::sInstance->netMgr.denyType << 28;
    return errorType;
}
kmCall(0x800dc9e8, GetSuspendType);
kmWrite32(0x800dc9f4, 0x906100d8);

static void HasBeenPulsarDenied(u32 level, const char* string) {
    register u32 error;
    asm(mr error, r0);
    DenyType type = DENY_TYPE_NORMAL;
    Mgr& mgr = Pulsar::System::sInstance->netMgr;
    if(error != 0x12) {
        type = static_cast<DenyType>(error & 0xf);
        if(RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_VS_REGIONAL && type == DENY_TYPE_BAD_PACK) mgr.deniesCount++;
    }
    Pulsar::System::sInstance->netMgr.denyType = type;
    DWC::Printf(level, string);
}
kmCall(0x800dd054, HasBeenPulsarDenied);
kmWrite32(0x800dd044, 0x60000000);

asmFunc ProcessWrapper() {
    ASM(
        nofralloc;
    mr r4, r31;
    mr r5, r25;
    mflr r31;
    bl Process;
    mtlr r31;
    rlwinm r0, r3, 0, 24, 31;
    blr;
        )
}
kmCall(0x800dc4a0, ProcessWrapper);

void Send(DWC::MatchCommand type, u32 pid, u32 ip, u16 port, void* data, u32 dataSize) {
    const RKNet::RoomType roomType = RKNet::Controller::sInstance->roomType;
    const bool isCustom = roomType == RKNet::ROOMTYPE_FROOM_NONHOST || roomType == RKNet::ROOMTYPE_FROOM_HOST
        || roomType == RKNet::ROOMTYPE_VS_REGIONAL || roomType == RKNet::ROOMTYPE_JOINING_REGIONAL
         || roomType == RKNet::ROOMTYPE_VS_WW || roomType == RKNet::ROOMTYPE_JOINING_WW;
    if(type == DWC::MATCH_COMMAND_RESERVATION && isCustom) {
        ResvPacket packet(*reinterpret_cast<const DWC::Reservation*>(data));
        System::sInstance->SetUserInfo(packet.pulInfo.userInfo);
        data = &packet;
        dataSize = sizeof(ResvPacket) / sizeof(u32);
    }
    SendMatchCommand(type, pid, ip, port, data, dataSize);
}
kmCall(0x800df078, Send);

static void ResetDenyCounter(UIControl* control, u32 soundId, u32 r5) {
    control->PlaySound(soundId, r5);
    if(RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_VS_REGIONAL) {
        Pulsar::System::sInstance->netMgr.deniesCount = 0;
    }
}
kmCall(0x80609110, ResetDenyCounter);

}
}


