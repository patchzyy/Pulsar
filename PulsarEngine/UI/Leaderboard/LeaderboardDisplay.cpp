#include <UI/Leaderboard/LeaderboardDisplay.hpp>
#include <MarioKartWii/Race/RaceInfo/RaceInfo.hpp>

namespace Pulsar {
namespace UI {

static LeaderboardDisplayType displayLeaderboardType = LEADERBOARD_DISPLAY_NAMES;

void setLeaderboardDisplayType(LeaderboardDisplayType type) {
    displayLeaderboardType = type;
}

LeaderboardDisplayType getLeaderboardDisplayType() {
    return displayLeaderboardType;
}

// In case we want to add more display types (FC, flags, whatever..)
void nextLeaderboardDisplayType() {
    if (displayLeaderboardType == LEADERBOARD_DISPLAY_NAMES) {
        displayLeaderboardType = LEADERBOARD_DISPLAY_TIMES;
    } else if (displayLeaderboardType == LEADERBOARD_DISPLAY_TIMES) {
        displayLeaderboardType = LEADERBOARD_DISPLAY_NAMES;
    }
}

void fillLeaderboardResults(int count, CtrlRaceResult** results) {
    // Copy the way the game loops here
    for(int i = 0; i < (count & 0xff); ++i) {
        const int position = (i + 1) & 0xff;
        const u8 playerId = Raceinfo::sInstance->playerIdInEachPosition[position - 1];
        if(displayLeaderboardType == LEADERBOARD_DISPLAY_TIMES) {
            results[i]->FillFinishTime(playerId);
        } else if(displayLeaderboardType == LEADERBOARD_DISPLAY_NAMES) {
            results[i]->FillName(playerId);
        }
    }
}

}//namespace UI
}//namespace Pulsar