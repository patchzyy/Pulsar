#include <kamek.hpp>
#include <PulsarSystem.hpp>
#include <Settings/Settings.hpp>
#include <MarioKartWii/GlobalFunctions.hpp>
#include <MarioKartWii/System/Identifiers.hpp>

extern u32 FPSPatchHook;
extern u32 ItemBoxHook;
extern u32 DCHook;
extern u32 OTTFixHook;
extern u32 DolphinCheat;
extern u32 MainDolCheat;
extern u32 AntiCheat1;
extern u32 AntiCheat2;
extern u32 AntiCheat3;
extern u32 AntiCheat4;
extern u32 AntiCheat5;
extern u32 AntiCheat6;
extern u32 AntiCheat7;
extern u32 GeckoLoader1;

namespace RetroRewind {
class System : public Pulsar::System {
public:
    static bool Is500cc();

    enum WeightClass{
        LIGHTWEIGHT,
        MEDIUMWEIGHT,
        HEAVYWEIGHT,
        MIIS,
        ALLWEIGHT
    };

    enum CharButtonId{
        BUTTON_BABY_MARIO,
        BUTTON_BABY_LUIGI,
        BUTTON_TOAD,
        BUTTON_TOADETTE,
        BUTTON_BABY_PEACH,
        BUTTON_BABY_DAISY,
        BUTTON_KOOPA_TROOPA,
        BUTTON_DRY_BONES,
        BUTTON_MARIO,
        BUTTON_LUIGI,
        BUTTON_YOSHI,
        BUTTON_BIRDO,
        BUTTON_PEACH,
        BUTTON_DAISY,
        BUTTON_DIDDY_KONG,
        BUTTON_BOWSER_JR,
        BUTTON_WARIO,
        BUTTON_WALUIGI,
        BUTTON_KING_BOO,
        BUTTON_ROSALINA,
        BUTTON_DONKEY_KONG,
        BUTTON_FUNKY_KONG,
        BUTTON_BOWSER,
        BUTTON_DRY_BOWSER,
        BUTTON_MII_A,
        BUTTON_MII_B
    };

    WeightClass weight;
    static Pulsar::System *Create(); //My Create function, needs to return Pulsar
    static WeightClass GetWeightClass(CharacterId);
};
} // namespace RetroRewind