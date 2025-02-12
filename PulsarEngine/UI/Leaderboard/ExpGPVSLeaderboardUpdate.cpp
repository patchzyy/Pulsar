#include <UI/Leaderboard/ExpGPVSLeaderboardUpdate.hpp>
#include <UI/Leaderboard/LeaderboardDisplay.hpp>
#include <UI/ExtendedTeamSelect/ExtendedTeamManager.hpp>
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

    if (System::sInstance->IsContext(PULSAR_MODE_OTT)) {
        setLeaderboardDisplayType(LEADERBOARD_DISPLAY_TIMES);
    }

    fillLeaderboardResults(GetRowCount(), this->results);
}

// Number of players allowed per team and per team count
// For example, 2 teams allows 6 players per team before it switches to the irregular leaderboard
// 0 teams, always irregular
// 1 team, always irregular
// 2 teams, 6 players per team
// 3 teams, 4 players per team
// 4 teams, 3 players per team
// 5 teams, 2 players per team
// 6 teams, 2 players per team
static const int ALLOWED_PLAYER_PER_TEAM_COUNT[TEAM_COUNT + 1] = {0, 0, 6, 4, 3, 2, 2};

PageId ExpGPVSLeaderboardUpdate::GetNextPage() const {
    PageId nextPageId = Pages::GPVSLeaderboardUpdate::GetNextPage();
    if (ExtendedTeamManager::IsActivated() && nextPageId == PAGE_GPVS_TOTAL_LEADERBOARDS) {
        RacedataScenario &scenario = Racedata::sInstance->racesScenario;
        int teamCount = 0;
        int numPlayerPerTeam[TEAM_COUNT] = {0};

        for (int i = 0; i < scenario.playerCount; i++) {
            ExtendedTeamID team = ExtendedTeamManager::sInstance->GetPlayerTeam(i);
            numPlayerPerTeam[team]++;
        }

        for (int i = 0; i < TEAM_COUNT; i++) {
            if (numPlayerPerTeam[i] > 0) {
                teamCount++;
            }
        }

        for (int i = 0; i < TEAM_COUNT; i++) {
            if (numPlayerPerTeam[i] > ALLOWED_PLAYER_PER_TEAM_COUNT[teamCount]) {
                return static_cast<PageId>(PULPAGE_EXTENDEDTEAMS_RESULT_TOTAL_IRREGULAR);
            }
        }

        return static_cast<PageId>(PULPAGE_EXTENDEDTEAMS_RESULT_TOTAL);
    }

    return nextPageId;
}

}
}