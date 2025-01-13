#include <UI/Leaderboard/ExpGPVSLeaderboardTotal.hpp>
#include <UI/Leaderboard/LeaderboardDisplay.hpp>
#include <Settings/Settings.hpp>

namespace Pulsar {
namespace UI {

void ExpGPVSLeaderboardTotal::OnUpdate() {
    const Input::RealControllerHolder* controllerHolder = SectionMgr::sInstance->pad.padInfos[0].controllerHolder;
    const ControllerType controllerType = controllerHolder->curController->GetType();
    const u16 inputs = controllerHolder->inputStates[0].buttonRaw;
    const u16 newInputs = (inputs & ~controllerHolder->inputStates[1].buttonRaw);

    bool swapDisplayType = false;
    switch (controllerType) {
    case NUNCHUCK:
    case WHEEL:
        swapDisplayType = (newInputs & WPAD::WPAD_BUTTON_LEFT | WPAD::WPAD_BUTTON_RIGHT) != 0;
        break;
    case CLASSIC:
        swapDisplayType = (newInputs & (WPAD::WPAD_CL_TRIGGER_L | WPAD::WPAD_CL_TRIGGER_R)) != 0;
        break;
    default:
        swapDisplayType = (newInputs & (PAD::PAD_BUTTON_L | PAD::PAD_BUTTON_R)) != 0;
        break;
    }

    if (swapDisplayType) {
        nextLeaderboardDisplayType();
        fillLeaderboardResults(GetRowCount(), this->results);
    }
}

} // namespace UI
} // namespace Pulsar