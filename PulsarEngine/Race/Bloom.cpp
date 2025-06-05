#include <RetroRewind.hpp>

namespace Pulsar {

void BloomPatch(){
    BloomHook = 0x00;
    if(Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_BLOOM) == RACESETTING_BLOOM_ENABLED){
        BloomHook = 0x03000000;
    }
}
static SectionLoadHook PatchBloom(BloomPatch);

}