#ifndef _PULUI_
#define _PULUI_
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <MarioKartWii/UI/Ctrl/UIControl.hpp>

namespace Pulsar {
namespace UI {

void ChangeImage(LayoutUIControl& control, const char* paneName, const char* tplName);
const wchar_t* GetCustomMsg(s32 bmgId);
void UnbindRLMC(lyt::Material* mat);
void ResetMatColor(lyt::Pane* pane, u32 color);
/*Expanded pages: (do not necessitate a PulPageId)
-ExpGhostSelect
-ExpFroom
-ExpFroomMessages (only static variables)
-ExpOptions
-ExpSinglePlayer
-ExpWFCMain
-ExpVR
-ExpCharacterSelect
-ExpKartSelect
-ExpBattleKartSelect
-ExpMultiKartSelect

*/
enum PulPageId {

    PULPAGE_INITIAL = 0x100,
    PULPAGE_CHOOSENEXT = PULPAGE_INITIAL,
    PULPAGE_TEAMSELECT,
    PULPAGE_ROOMKICK,
    PULPAGE_KORACEEND,
    PULPAGE_KOWINNER,
    PULPAGE_SETTINGS,
    PULPAGE_EXTENDEDTEAMSELECT,
    PULPAGE_EXTENDEDTEAMS_RESULT_TOTAL,
    PULPAGE_EXTENDEDTEAMS_RESULT_TOTAL_IRREGULAR,
    PULPAGE_CODEDRIVEN,

    PULPAGE_MAX = PULPAGE_CODEDRIVEN - PULPAGE_INITIAL + 1
};

class ExpSection : public Section {  // u32 id -> either a standard pageId but can also be a PulPageId
   public:
    static ExpSection* GetSection() { return reinterpret_cast<ExpSection*>(SectionMgr::sInstance->curSection); }

    static void CreatePages(ExpSection& self, SectionId id);
    void CreatePulPages();
    static void CreateAndInitPage(ExpSection& self, u32 id);
    static void SetNextPage(u32 id, u32 animDirection);
    static void DisposePulPages(SectionPad& pad, bool enablePointer);
    static void AddPageLayer(ExpSection& self, u32 id);
    static Page* AddPageLayerAnimatedReturnTopLayer(ExpSection& self, u32 id, u32 animDirection);

    template <class T>
    inline T* GetPulPage() const {
        static_assert(is_base_of<Page, T>::value, "Not a Page");
        return static_cast<T*>(this->pulPages[T::id - PULPAGE_INITIAL]);
    }

    template <class T>
    inline T* GetPulPage(PulPageId id) const {
        static_assert(is_base_of<Page, T>::value, "Not a Page");
        return static_cast<T*>(this->pulPages[id - PULPAGE_INITIAL]);
    }
    inline void SetPulPage(Page* t, PulPageId id) { this->pulPages[id - PULPAGE_INITIAL] = t; }

    Page* pulPages[PULPAGE_MAX];
    bool hasAutoVote;
};

// Simple enum of BMGs, making any change much easier as everything is centralized here
enum BMG {
    // vanilla
    BMG_PLEASE_WAIT_A_MOMENT = 0x401,
    BMG_SAVED_GHOST = 0x45b,
    BMG_FINISH = 0x4b5,
    BMG_SCORE_PTS = 0x521,
    BMG_SCORE_POINTS = 0x523,
    BMG_DISPLAY_TIME = 0x578,
    BMG_YES = 0x7d2,
    BMG_NO = 0x7d3,
    BMG_TIME_TRIALS = 0xbb9,

    BMG_GP_BOTTOM_TEXT = 0xd20,
    BMG_GP_GOLD_TROPHY = 0xd2a,
    BMG_GP_SILVER_TROPHY = 0xd2b,
    BMG_GP_BRONZE_TROPHY = 0xd2c,
    BMG_GP_RANK_3STARS = 0xd2d,
    BMG_GP_RANK_2STARS = 0xd2e,
    BMG_GP_RANK_1STAR = 0xd2f,
    BMG_GP_RANK_A = 0xd30,
    BMG_GP_RANK_B = 0xd31,
    BMG_GP_RANK_C = 0xd32,
    BMG_GP_RANK_D = 0xd33,
    BMG_GP_RANK_E = 0xd34,
    BMG_GP_RANK_F = 0xd35,  // unused
    BMG_GP_BLANK = 0xd36,

    BMG_CHOOSE_GHOST_DATA = 0xd4f,
    BMG_DISCONNECTED_FROM_OTHER_PLAYERS = 0xfb2,
    BMG_PLAY_GP = 0x100e,
    BMG_PLAY_TEAM_GP = 0x100f,
    BMG_RATING = 0x106a,  // vr/br value + "rating" under
    BMG_RACE_WITH11P = 0x10da,
    BMG_BATTLE_WITH6P = 0x10db,
    BMG_READY_TO_RACE = 0x1105,
    BMG_CHALLENGE_GHOST = 0x177b,
    BMG_WATCH_REPLAY = 0x177c,
    BMG_NUMBER = 0x2044,
    BMG_PERCENT_MENU = 0x2048,
    BMG_KICK_BOTTOM = 0x2179,
    BMG_KICK_CONFIRM = 0x217A,
    BMG_PLAYER_COUNT = 0x217B,
    BMG_REGCUPS = 0x23f0,
    BMG_REGS = 0x2454,
    BMG_BATTLE = 0x2498,
    BMG_MII_NAME = 0x251d,
    BMG_TIMER_DIFF_MINUS = 0x25b3,
    BMG_TIMER_DIFF_PLUS = 0x25b4,

    // race
    BMG_INFO_DISPLAY = 0x2700,
    BMG_TROPHY_EARNED = 0x2701,
    BMG_GP_RACE = 0x2702,
    BMG_CHOOSE_NEXT = 0x2703,
    BMG_RANDOM_TRACK = 0x2704,
    BMG_PERCENT_RACE = 0x2705,
    BMG_NUMBER_RACE = 0x2706,

    // Menu
    BMG_TT_MODE_BUTTONS = 0x2800,
    BMG_TT_MODE_BOTTOM_CUP = 0x2810,
    BMG_TT_MODE_BOTTOM_SINGLE = 0x2820,
    BMG_TT_BOTTOM_CUP_NOTROPHY = 0x282f,
    BMG_TT_BOTTOM_CUP = 0x2830,

    BMG_TT_BOTTOM_COURSE_NOTROPHY = 0x2831,
    BMG_TT_BOTTOM_COURSE = 0x2832,

    BMG_NO_TROPHY = 0x2833,
    BMG_TROPHY = 0x2834,

    BMG_FAV_GHOST = 0x2835,

    BMG_SELECT_GHOST = 0x2840,
    BMG_GHOST_SELECTED = 0x2841,
    BMG_GHOST_SELECTED_COUNTER = 0x2842,
    BMG_RANDOM_TROPHIES_MISSING = 0x2843,
    BMG_RANDOM_CUP = 0x2844,
    BMG_RANDOM_TROPHIES_MISSING_GCN = 0x2845,
    BMG_RANDOM_ALL_TROPHIES_GCN = 0x2846,
    BMG_DATE = 0x2847,
    BMG_NINTENDO = 0x2848,
    BMG_TEXT = 0x2849,
    BMG_TEAM_SELECT = 0x284a,
    BMG_ROOM_DENY = 0x284b,
    BMG_TOO_MANY_DENIES = 0x284c,

    BMG_VS_BUTTON = 0x6900,

    // Worldwides
    BMG_MAIN_MODES = 0x6901,
    BMG_OTHER_MODES = 0x6902,
    BMG_BATTLE_MODES = 0x6914,

    // Custom Track
    BMG_CT_BUTTON = 0x690c,

    // Regular Track
    BMG_REGULAR_BUTTON = 0x6907,

    // OTT
    BMG_OTT_WW_BOTTOM = 0x6903,
    BMG_OTT_BUTTON = 0x6904,
    BMG_OTT_PLAYING = 0x6905,
    BMG_OTT_TITLE_TEXT = 0x6906,
    BMG_OTT_TIME_DIFF = 0x2854,

    // 200cc
    BMG_200_WW_BOTTOM = 0x6908,
    BMG_200_BUTTON = 0x6909,
    BMG_200_PLAYING = 0x690a,
    BMG_200_TITLE_TEXT = 0x690b,

    // ItemRain
    BMG_ITEM_RAIN_WW_BOTTOM = 0x690d,
    BMG_ITEM_RAIN_BUTTON = 0x690e,
    BMG_ITEM_RAIN_PLAYING = 0x690f,
    BMG_ITEM_RAIN_TITLE_TEXT = 0x6910,

    // Battle
    BMG_BATTLE_WW_BOTTOM = 0x6915,
    BMG_BATTLE_BUTTON = 0x6916,
    BMG_BATTLE_TITLE_TEXT = 0x6917,

    // Elimination
    BMG_BATTLE_BUTTON_ELIM = 0x6919,
    BMG_BATTLE_WW_BOTTOM_ELIM = 0x691a,
    
    // RT
    BMG_TITLE_TEXT_RT = 0x6912,

    // CT
    BMG_TITLE_TEXT_CT = 0x6913,

    // Regs
    BMG_TITLE_TEXT_REGS = 0x691b,

    // BT
    BMG_TITLE_TEXT_BT = 0x6918,

    // VR Rating
    BMG_VR_RATING = 0x285e,
    BMG_BR_RATING = 0x6969,

    // Language
    BMG_LANGUAGE_RESET_REQUIRED = 0x295f,

    // KO
    BMG_KO_OUT = 0x2860,
    BMG_KO_WINNER = 0x2861,
    // these 3 need to follow each other
    BMG_KO_AVERAGE_PERCENT_TITLE = 0x2862,
    BMG_KO_TIME_DANGER_TITLE = 0x2863,
    BMG_KO_ALMOST_OUT_TITLE = 0x2864,

    BMG_KO_ELIM_START_NONE = 0x2865,
    BMG_KO_ELIM_START_SINGLE = 0x2866,
    BMG_KO_ELIM_START_DOUBLE = 0x2867,
    BMG_KO_ELIM_START_TRIPLE = 0x2868,
    BMG_KO_ELIM_START_QUADRUPLE = 0x2869,
    BMG_KO_RACEEND_SPECTATE = 0x286a,
    BMG_KO_RACEEND_QUIT = 0x286b,
    BMG_KO_TIE = 0x286c,

    BMG_SETTINGSBUTTON_BOTTOM = 0x2901,
    BMG_TEAMS_BOTTOM = 0x2910,
    BMG_TEAMS_ENABLED = 0x2912,
    BMG_TEAMS_DISABLED = 0x2913,

    // froom
    BMG_PLAY_OTT = 0x2914,
    BMG_PLAY_KO = 0x2915,
    BMG_PLAY_OTTKO = 0x2916,
    BMG_PLAY_TEAM_OTT = 0x2917,
    BMG_PLAY_TEAM_KO = 0x2918,
    BMG_PLAY_TEAM_OTTKO = 0x2919,

    BMG_CUPS = 0x10000,
    BMG_TRACKS = 0x20000,
    BMG_AUTHORS = 0x30000,

    // ADD 0x50000 to all of these for YOUR settings, 0x52f01 = the button text etc...
    BMG_SETTINGS_PAGE = 0x2f01,
    BMG_SETTINGS_BOTTOM = 0x2f10,
    BMG_SETTINGS_TITLE = 0x2f20,
    BMG_RADIO_SETTINGS = 0x3000,
    BMG_SCROLLER_SETTINGS = 0x3A00,
    BMG_USERSETTINGSOFFSET = 0x60000,  // user settings therefore start at 0x53000 for radio settings, 0x53A00 for scrollers

    // Custom texts (extended teams, explanations, etc..)
    BMG_EXTENDEDTEAMS_EXPLANATION = 0x83337,
    BMG_EXTENDEDTEAMS_NONHOST_TITLE = 0x83338,
    BMG_EXTENDEDTEAMS_IRREGULAR_WARNING = 0x8333A,
    BMG_EXTENDEDTEAMS_TEAM_NAME = 0x83340,
    BMG_EXTENDEDTEAMS_WINNER = 0x83350,
    BMG_EXTENDEDTEAMS_PLAY = 0x83352,
};

const char controlFolder[] = "control";
const char buttonFolder[] = "button";
const char raceFolder[] = "game_image";
const char bgFolder[] = "bg";

}  // namespace UI
}  // namespace Pulsar

#endif