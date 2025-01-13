#include <UI/Leaderboard/ExpGPVSLeaderboardUpdate.hpp>
#include <UI/Leaderboard/LeaderboardDisplay.hpp>
#include <Settings/Settings.hpp>

namespace Pulsar {
namespace UI {

void ExpGPVSLeaderboardUpdate::OnUpdate() {
    if (checkLeaderboardDisplaySwapInputs()) {
        nextLeaderboardDisplayType();
        fillLeaderboardResults(GetRowCount(), this->results);
    }
}

// Apply old toggle logic, set default display type
void ExpGPVSLeaderboardUpdate::BeforeEntranceAnimations() {
    if(static_cast<Pulsar::Times>(Pulsar::Settings::Mgr::Get().GetUserSettingValue(static_cast<Pulsar::Settings::UserType>(Pulsar::Settings::SETTINGSTYPE_RR2), Pulsar::SETTINGRR2_RADIO_TIMES)) == Pulsar::TIMES_DISABLED) {
        setLeaderboardDisplayType(LEADERBOARD_DISPLAY_NAMES);
    } else if(static_cast<Pulsar::Times>(Pulsar::Settings::Mgr::Get().GetUserSettingValue(static_cast<Pulsar::Settings::UserType>(Pulsar::Settings::SETTINGSTYPE_RR2), Pulsar::SETTINGRR2_RADIO_TIMES)) == Pulsar::TIMES_ENABLED) {
        setLeaderboardDisplayType(LEADERBOARD_DISPLAY_TIMES);
    }
}

}
}