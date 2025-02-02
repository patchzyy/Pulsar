#include <UI/PlayerCount.hpp>

// Callbacks that can occur during server browsing operations
typedef enum {
  sbc_serveradded, // a server was added to the list, may just have an IP & port
                   // at this point
  sbc_serverupdated, // server information has been updated - either basic or
                     // full information is now available about this server
  sbc_serverupdatefailed, // an attempt to retrieve information about this
                          // server, either directly or from the master, failed
  sbc_serverdeleted,      // a server was removed from the list
  sbc_updatecomplete,     // the server query engine is now idle
  sbc_queryerror, // the master returned an error string for the provided query
  sbc_serverchallengereceived // received ip verification challenge from server
} SBCallbackReason;

typedef void (*SBServerKeyEnumFn)(char* key, char* value,
                                  void* instance);



typedef void* SBServer;
typedef void* ServerBrowser;
typedef void (*ServerBrowserCallback)(ServerBrowser sb, SBCallbackReason reason,
                                      SBServer server, void* instance);
typedef void (*TableMapFn)(void* elem, void* clientData);

extern "C" ServerBrowser ServerBrowserNewA(const char* queryForGamename,
                               const char* queryFromGamename,
                               const char* queryFromKey,
                               int queryFromVersion, int maxConcUpdates,
                               int queryVersion, bool lanBrowse,
                               ServerBrowserCallback callback, void* instance);
extern "C" int ServerBrowserLimitUpdateA(ServerBrowser sb, bool async,
                                 bool disconnectOnComplete,
                                 const unsigned char* basicFields,
                                 int numBasicFields,
                                 const char* serverFilter, int maxServers);
extern "C" int ServerBrowserCount(ServerBrowser sb);
extern "C" void ServerBrowserThink(ServerBrowser sb);
extern "C" void ServerBrowserClear(ServerBrowser sb);
extern "C" void ServerBrowserFree(ServerBrowser sb);
extern "C" SBServer ServerBrowserGetServer(ServerBrowser sb, int index);
extern "C" void TableMapSafe(void* table, TableMapFn fn, void* clientData);
extern "C" const char* SBServerGetStringValueA(SBServer server, const char* keyname,
                                    const char* def);

extern "C" int SBServerGetIntValueA(SBServer server, const char* key, int idefault);

extern "C" void qr2_register_keyA(int keyid, const char* key);

extern "C" void msleep(unsigned int msecs);
extern "C" void* gsimalloc(u32 size);
extern "C" int DWCi_QR2Startup(u32 profileID);
extern "C" void DWC_SetReportLevel(u32 level);
extern "C" void DWC_ProcessFriendsMatch();
extern "C" void DWCi_SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void* instance);

typedef struct {
  SBServerKeyEnumFn EnumFn;
  void* instance;
} SBServerEnumData;

typedef struct _SBKeyValuePair {
  const char* key;
  const char* value;
} SBKeyValuePair;

static void KeyMapF(void* elem, void* clientData) {
  SBKeyValuePair* kv = (SBKeyValuePair*)elem;
  SBServerEnumData* ped = (SBServerEnumData*)clientData;
  ped->EnumFn((char*)kv->key, (char*)kv->value, ped->instance);
}

void SBServerEnumKeys(SBServer server, SBServerKeyEnumFn KeyFn,
                      void* instance) {
  SBServerEnumData ed;

  ed.EnumFn = KeyFn;
  ed.instance = instance;
  TableMapSafe(*(void**)((u32)server + 0x18), KeyMapF, &ed);
}

void getRegionParamsFromString(const char* region, char* outputRegion, u32& outputRegionID) {

    char value[32];
    strncpy(value, region, 32);

    char* delimiter = strchr(value, '_');
    if (delimiter) {
        *delimiter = '\0';
        strncpy(outputRegion, value, 16);

        char* end = nullptr;
        int regionInt = strtol(delimiter + 1, &end, 10);

        if (end) {
            outputRegionID = regionInt;
        }
    }
}

static ServerBrowser playerCntSB = nullptr;
static bool isHookedRequest = false;
static float hookLocalTimer = 0.0f;
static bool hasRKNetRequestFinished = true;

static int numPlayersRegular = 0;
static int numPlayers200cc = 0;
static int numPlayers150cc = 0;
static int numPlayersOTT = 0;

void PlayerCount::GetNumbers(int& n150cc, int& n200c, int& nOtt, int& nRegular) {
    n150cc = numPlayers150cc;
    n200c = numPlayers200cc;
    nOtt = numPlayersOTT;
    nRegular = numPlayersRegular;
}

void hookedCallback(ServerBrowser sb, SBCallbackReason reason,
                                      SBServer server, void* instance) {
    
    DWCi_SBCallback(sb, reason, server, instance);
    if (reason == sbc_updatecomplete) {
        hasRKNetRequestFinished = true;
    }
}

void sbCallback(ServerBrowser sb, SBCallbackReason reason,
                                      SBServer server, void* instance) {
    if (reason == sbc_updatecomplete) {
        int local150cc = 0;
        int local200cc = 0;
        int localOTT = 0;
        int localRegular = 0;
        for (int i = 0; i < ServerBrowserCount(sb); i++) {
            SBServer server = ServerBrowserGetServer(sb, i);
            
            char region[16];
            u32 regionID = 0xffffffff;

            const char* rk = SBServerGetStringValueA(server, "rk", "");
            getRegionParamsFromString(rk, region, regionID);

            int numplayers = SBServerGetIntValueA(server, "numplayers", -1) + 1;

            int numRegs = 0, num150 = 0, num200 = 0, numOTT = 0;
            if (strstr(region, "vs") && regionID == 0x0A) {
                num150 += numplayers;
            } else if (strstr(region, "vs") && regionID == 0x0C) {
                num200 += numplayers;
            } else if (strstr(region, "vs") && regionID == 0x0B) {
                numOTT += numplayers;
            } else if (strstr(region, "vs") && regionID == 0xffffffff) {
                numRegs += numplayers;
            }

            local150cc += num150;
            local200cc += num200;
            localOTT += numOTT;
            localRegular += numRegs;
        } 

        numPlayers150cc = local150cc;
        numPlayers200cc = local200cc;
        numPlayersOTT = localOTT;
        numPlayersRegular = localRegular;

        OS::Report("150cc: %d, 200cc: %d, OTT: %d, Regular: %d\n", local150cc, local200cc, localOTT, localRegular);

        isHookedRequest = false;

    }
}

// 0x08 == numplayers
// 0x0a == maxplayers
// 0x32 == dwc_pid
// 0x33 == dwc_mtype
// 0x34 == dwc_mver
// 0x35 == dwc_eval
// 0x36 == dwc_groupid
// 0x37 == dwc_hoststate
// 0x38 == dwc_suspend
// 0x64 == rk
// 0x65 == ev
// 0x66 == eb
// 0x67 == p
static const u8 basicFields[] = {0x08, 0x0a, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x64, 0x65, 0x66, 0x67};

bool hasQR2Initialized = false;
int hook_QR2Startup(u32 id) {
    int res = DWCi_QR2Startup(id);
    
    OS::Report("============================= QRSTARTUP2 %12d %d =============================\n", id, res);

    qr2_register_keyA(0x32, "dwc_pid");
    qr2_register_keyA(0x33, "dwc_mtype");
    qr2_register_keyA(0x34, "dwc_mver");
    qr2_register_keyA(0x35, "dwc_eval");
    qr2_register_keyA(0x36, "dwc_groupid");
    qr2_register_keyA(0x37, "dwc_hoststate");
    qr2_register_keyA(0x38, "dwc_suspend");
    qr2_register_keyA(0x64, "rk");
    qr2_register_keyA(0x65, "ev");
    qr2_register_keyA(0x66, "eb");
    qr2_register_keyA(0x67, "p");

    hasQR2Initialized = true;

    return res;
}

void StartRequestTask(void* arg) {
    if (isHookedRequest) {
        
        playerCntSB = ServerBrowserNewA(
            DWC::MatchControl::sInstance->gameName,
            DWC::MatchControl::sInstance->gameName,
            DWC::MatchControl::sInstance->secretKey,
            0,
            20,
            1,
            false,
            sbCallback,
            nullptr
        );
        ServerBrowserLimitUpdateA(playerCntSB, false, false, basicFields, sizeof(basicFields), "gamename = \"mariokartwii\"", 256);
        ServerBrowserFree(playerCntSB);
    }
}

ServerBrowser hook_ServerBrowserNewA(const char* queryForGamename,
                               const char* queryFromGamename,
                               const char* queryFromKey,
                               int queryFromVersion, int maxConcUpdates,
                               int queryVersion, bool lanBrowse,
                               ServerBrowserCallback callback, void* instance) {
    return ServerBrowserNewA(queryForGamename, queryFromGamename, queryFromKey, queryFromVersion, maxConcUpdates, queryVersion, lanBrowse, hookedCallback, instance);
}

int hook_ServerBrowserLimitUpdateA(ServerBrowser sb, bool async,
                                 bool disconnectOnComplete,
                                 const unsigned char* basicFields,
                                 int numBasicFields,
                                 const char* serverFilter, int maxServers) {
    

    while (isHookedRequest)
        msleep(10);

    hasRKNetRequestFinished = false;
    int res = ServerBrowserLimitUpdateA(
        sb,
        async,
        disconnectOnComplete,
        basicFields,
        numBasicFields,
        serverFilter,
        maxServers
    );

    return res;
}

void hook_DWC_SetReportLevel(u32 level) {
    DWC_SetReportLevel(level);
}

void hook_Section_calc(Section* _this) {
    hookLocalTimer += 1.0f / 60.0f;
    _this->UpdateLayers();

    if (hasQR2Initialized && !isHookedRequest && hasRKNetRequestFinished && hookLocalTimer >= 5.0f && SectionMgr::sInstance->curSection->pages[Pages::Globe::id]) {
        isHookedRequest = true;
        hookLocalTimer = 0.0f;
        Pulsar::System::sInstance->taskThread->Request(StartRequestTask, nullptr, 0);
    }
}

void hook_ServerBrowserThink(ServerBrowser sb) {
    if (!isHookedRequest) {
        ServerBrowserThink(sb);
    }
}

kmCall(0x800d0584, hook_QR2Startup);
kmCall(0x800d413c, hook_QR2Startup);
kmCall(0x800d5484, hook_QR2Startup);
kmCall(0x800d56bc, hook_QR2Startup);
kmCall(0x800d605c, hook_QR2Startup);
kmCall(0x800d62c0, hook_QR2Startup);

kmCall(0x800db908, hook_ServerBrowserLimitUpdateA);
kmCall(0x80658be8, hook_DWC_SetReportLevel);
kmCall(0x800d5390, hook_ServerBrowserNewA);
kmCall(0x800d573c, hook_ServerBrowserNewA);
kmCall(0x800d5f74, hook_ServerBrowserNewA);
kmCall(0x800d6208, hook_ServerBrowserNewA);
kmCall(0x800e435c, hook_ServerBrowserNewA);
kmCall(0x800d8384, hook_ServerBrowserThink);
kmCall(0x80622514, hook_Section_calc);