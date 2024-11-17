#include <kamek.hpp>
#include <MarioKartWii/Item/ItemBehaviour.hpp>
#include <MarioKartWii/Item/Obj/ObjProperties.hpp>
#include <RetroRewind.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>

namespace RetroRewind {
namespace Race {
    static void ChangeBlueOBJProperties(Item::ObjProperties* dest, const Item::ObjProperties& rel) {
    bool itemModeRandom = Pulsar::GAMEMODE_DEFAULT;
    bool itemModeBlast = Pulsar::GAMEMODE_DEFAULT;
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST) {
        itemModeRandom = System::sInstance->IsContext(Pulsar::PULSAR_ITEMMODERANDOM) ? Pulsar::GAMEMODE_RANDOM : Pulsar::GAMEMODE_DEFAULT;
        itemModeBlast = System::sInstance->IsContext(Pulsar::PULSAR_ITEMMODEBLAST) ? Pulsar::GAMEMODE_BLAST : Pulsar::GAMEMODE_DEFAULT;
    }
        new (dest) Item::ObjProperties(rel);
        if(itemModeBlast == Pulsar::GAMEMODE_BLAST) {
            dest->limit = 25;
        }
    if (itemModeRandom == Pulsar::GAMEMODE_RANDOM){
        dest->limit = 5;
    }
    }

    kmCall(0x80790b74, ChangeBlueOBJProperties);

    static void ChangeBillOBJProperties(Item::ObjProperties* dest, const Item::ObjProperties& rel){
    bool itemModeRandom = Pulsar::GAMEMODE_DEFAULT;
    bool itemModeBlast = Pulsar::GAMEMODE_DEFAULT;
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST) {
        itemModeRandom = System::sInstance->IsContext(Pulsar::PULSAR_ITEMMODERANDOM) ? Pulsar::GAMEMODE_RANDOM : Pulsar::GAMEMODE_DEFAULT;
        itemModeBlast = System::sInstance->IsContext(Pulsar::PULSAR_ITEMMODEBLAST) ? Pulsar::GAMEMODE_BLAST : Pulsar::GAMEMODE_DEFAULT;
    }
    new (dest) Item::ObjProperties(rel);
    if (itemModeRandom == Pulsar::GAMEMODE_RANDOM){
        dest->limit = 25;
    }
    if (itemModeBlast == Pulsar::GAMEMODE_BLAST){
        dest->limit = 5;
    }

}

    kmCall(0x80790bf4, ChangeBillOBJProperties);

    static void ChangeBombOBJProperties(Item::ObjProperties* dest, const Item::ObjProperties& rel){
    bool itemModeRandom = Pulsar::GAMEMODE_DEFAULT;
    bool itemModeBlast = Pulsar::GAMEMODE_DEFAULT;
    if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST || RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST) {
        itemModeRandom = System::sInstance->IsContext(Pulsar::PULSAR_ITEMMODERANDOM) ? Pulsar::GAMEMODE_RANDOM : Pulsar::GAMEMODE_DEFAULT;
        itemModeBlast = System::sInstance->IsContext(Pulsar::PULSAR_ITEMMODEBLAST) ? Pulsar::GAMEMODE_BLAST : Pulsar::GAMEMODE_DEFAULT;
    }
    new (dest) Item::ObjProperties(rel);
    if(itemModeRandom == Pulsar::GAMEMODE_RANDOM){
        dest->limit = 20;
    }
    if (itemModeBlast == Pulsar::GAMEMODE_BLAST){
        dest->limit = 25;
    }
}

    kmCall(0x80790bb4, ChangeBombOBJProperties);
} // namespace Race
} // namespace RetroRewind