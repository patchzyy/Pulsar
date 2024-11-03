#ifndef _PULINFO_
#define _PULINFO_
#include <kamek.hpp>

namespace Pulsar {
#pragma pack(push, 1)
class Info {
public:
    friend class System;
    //User Options Getters

    inline u32  GetKey() const { return this->roomKey; }
    inline bool HasTrophies() const { return this->hasTTTrophies; }
    inline bool Has200cc() const { return this->has200cc; }
    inline bool HasUMTs() const { return this->hasUMTs; }
    inline bool HasFeather() const { return this->hasFeather; }
    inline bool HasMegaTC() const { return this->hasMegaTC; }
    inline u32 GetWiimmfiRegion() const { return this->wiimmfiRegion; }
    inline u32 GetProb100() const { return this->prob100cc; }
    inline u32 GetProb150() const { return this->prob150cc; }
    inline u32 GetTrackBlocking() const { return  this->trackBlocking; }
    inline u16 GetCupIconCount() const {
        u16 count = this->cupIconCount;
        if (count == 0) count = 100;
        return count;
    }
    inline u16 GetChooseNextTrackTimer() const { return this->chooseNextTrackTimer; }

/*
Wiimmfi being wiimmfi means I can't use a kmCall or a even a kmBranch because a wiimmfi function that USES THE LR gets injected,
meaning the LR needs to be preserved. The u64 return is just to prevent the register variable from using r3/r4 as they are not safe
This is insanely hacky but it works and there aren't a million solutions
*/
// #define PatchRegion(addr)\
//     static inline u64 GetWiimmfiRegionStatic##addr(u64 src) {\
//         register const Info *sInstance = Info::sInstance;\
//         asmVolatile(lwz r7, Info.wiimmfiRegion(sInstance););\
//         return src;\
//     };\
//     kmBranch(addr, GetWiimmfiRegionStatic##addr);\
//     kmPatchExitPoint(GetWiimmfiRegionStatic##addr, ##addr + 4);

private:
    Info() {}
    void Init(const Info& rawInfo) { memcpy(this, &rawInfo, sizeof(Info)); }
    u32 roomKey; //0x0 transmitted to other players
    u32 prob100cc; //0x4
    u32 prob150cc; //0x8
    u32 wiimmfiRegion; //0xc
    u32 trackBlocking; //0x10
    bool hasTTTrophies; //0x14
    bool has200cc; //0x15
    bool hasUMTs; //0x16
    bool hasFeather; //0x17
    bool hasMegaTC; //0x18
    u16 cupIconCount; //0x19
    u8 chooseNextTrackTimer; //0x1b
    u8 reservedSpace[40];
};
size_assert(Info, 0x44);
#pragma pack(pop)

}//namespace Pulsar

#endif