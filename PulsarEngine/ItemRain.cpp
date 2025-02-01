#include <kamek.hpp>
#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/System/Identifiers.hpp>
#include <MarioKartWii/Driver/DriverManager.hpp>
#include <MarioKartWii/Race/RaceInfo/RaceInfo.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <MarioKartWii/RKNet/ITEM.hpp>
#include <MarioKartWii/Item/Obj/ItemObj.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {
namespace ItemRain {

    static u32 rngSeed = 0x12345678;
    static s32 sRaceInfoFrameCounter = 0;

    // Item pools for different rarity levels
    static const ItemId commonItems[] = {GREEN_SHELL, RED_SHELL, BANANA};
    static const ItemId uncommonItems[] = {MUSHROOM, FAKE_ITEM_BOX, MEGA_MUSHROOM, BOBOMB};
    static const ItemId rareItems[] = {STAR, BLUE_SHELL, LIGHTNING, BLOOPER};
    static const ItemId veryRareItems[] = {POW_BLOCK, GOLDEN_MUSHROOM, BULLET_BILL};

    // Constants
    static const int SPAWN_INTERVAL = 10;  // Frames between spawns
    static const int ITEMS_PER_SPAWN = 5;  // Number of items to spawn
    static const int SPAWN_HEIGHT = 3000;  // Height above player to spawn items
    static const int SPAWN_RADIUS = 8000;  // Max distance from player to spawn items

    static u32 GetRandom() {
        rngSeed = rngSeed * 1103515245 + 12345;
        return rngSeed;
    }

    static int GetRandomRange(int min, int max) {
        return min + (GetRandom() % (max - min + 1));
    }

    static ItemId GetRandomItem() {
        u32 randNum = GetRandom() % 100;
        if (randNum < 50) { // 50% common
            return commonItems[GetRandom() % (sizeof(commonItems) / sizeof(commonItems[0]))];
        } else if (randNum < 80) { // 30% uncommon
            return uncommonItems[GetRandom() % (sizeof(uncommonItems) / sizeof(uncommonItems[0]))];
        } else if (randNum < 95) { // 15% rare
            return rareItems[GetRandom() % (sizeof(rareItems) / sizeof(rareItems[0]))];
        } else { // 5% very rare
            return veryRareItems[GetRandom() % (sizeof(veryRareItems) / sizeof(veryRareItems[0]))];
        }
    }

    void SpawnItemRain() {
        if (!Racedata::sInstance || !Raceinfo::sInstance || !Item::Manager::sInstance) return;
        if (!Raceinfo::sInstance->IsAtLeastStage(RACESTAGE_RACE)) return;

        sRaceInfoFrameCounter++;
        if ((sRaceInfoFrameCounter % SPAWN_INTERVAL) != 0) return;

        bool isOnline = (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST ||
                        RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST);

        u8 playerCount = Pulsar::System::sInstance->nonTTGhostPlayersCount;
        if (playerCount == 0) return;

        Vec3 dummyDirection;
        dummyDirection.x = 0.0f;
        dummyDirection.y = 0.0f;
        dummyDirection.z = 0.0f;

        for (int i = 0; i < ITEMS_PER_SPAWN; i++) {
            u32 rndPlayerIdx = GetRandom() % playerCount;
            Item::Player& player = Item::Manager::sInstance->players[rndPlayerIdx];
            Vec3 playerPos = player.GetPosition();

            Vec3 spawnPos;
            spawnPos.x = playerPos.x + GetRandomRange(-SPAWN_RADIUS, SPAWN_RADIUS);
            spawnPos.y = playerPos.y + SPAWN_HEIGHT;
            spawnPos.z = playerPos.z + GetRandomRange(-SPAWN_RADIUS, SPAWN_RADIUS);

            ItemId itemID = GetRandomItem();
            Item::Manager::sInstance->CreateItemDirect(itemID, &spawnPos, &dummyDirection, rndPlayerIdx);
        }
    }
    RaceFrameHook ItemRainHook(SpawnItemRain);

} // namespace ItemRain
} // namespace Pulsar