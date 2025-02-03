#ifndef __PLAYER_COUNT_HPP
#define __PLAYER_COUNT_HPP

#include <PulsarSystem.hpp>
#include <Settings/UI/ExpWFCMainPage.hpp>
#include <MarioKartWii/UI/Page/Other/Globe.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <MarioKartWii/UI/Page/Other/WFCMenu.hpp>
#include <core/rvl/DWC/DWCMatch.hpp>
#include <core/GS/GSAvailable.hpp>
#include <MarioKartWii/RKSYS/RKSYSMgr.hpp>

namespace PlayerCount {

void GetNumbers(int& n150cc, int& n200c, int& nOtt, int& nRegular);

}

#endif // __PLAYER_COUNT_HPP