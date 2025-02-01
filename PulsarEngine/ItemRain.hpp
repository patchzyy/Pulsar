#pragma once

#include <PulsarSystem.hpp>
#include <MarioKartWii/System/Identifiers.hpp>

namespace Pulsar {

class ItemRain {
public:
    static ItemRain* Instance();

    // Core functionality
    void Update();
    void Enable();
    void Disable();

    // Configuration
    void SetSpawnInterval(int frames);
    void SetSpawnHeight(float height);
    void SetSpawnRadius(float radius);

private:
    ItemRain();  // Private constructor for singleton
    void SpawnRandomItem();
    ItemId GetRandomItemType();
    ItemObjId GetObjIdFromItemId(ItemId itemId);

    static ItemRain* sInstance;

    // State
    bool mIsEnabled;
    int mSpawnTimer;
    int mSpawnInterval;
    float mSpawnHeight;
    float mSpawnRadius;
};

} // namespace Pulsar 