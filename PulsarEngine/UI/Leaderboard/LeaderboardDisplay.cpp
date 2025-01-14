#include <UI/Leaderboard/LeaderboardDisplay.hpp>
#include <MarioKartWii/Race/RaceInfo/RaceInfo.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>

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

bool checkLeaderboardDisplaySwapInputs() {
    const Input::RealControllerHolder* controllerHolder = SectionMgr::sInstance->pad.padInfos[0].controllerHolder;
    const ControllerType controllerType = controllerHolder->curController->GetType();
    const u16 inputs = controllerHolder->inputStates[0].buttonRaw;
    const u16 newInputs = (inputs & ~controllerHolder->inputStates[1].buttonRaw);

    bool swapDisplayType = false;
    switch (controllerType) {
    case NUNCHUCK:
    case WHEEL:
        swapDisplayType = (newInputs & (WPAD::WPAD_BUTTON_LEFT | WPAD::WPAD_BUTTON_RIGHT)) != 0;
        break;
    case CLASSIC:
        swapDisplayType = (newInputs & (WPAD::WPAD_CL_TRIGGER_L | WPAD::WPAD_CL_TRIGGER_R)) != 0;
        break;
    default:
        swapDisplayType = (newInputs & (PAD::PAD_BUTTON_L | PAD::PAD_BUTTON_R)) != 0;
        break;
    }

    return swapDisplayType;
}

}//namespace UI
}//namespace Pulsar