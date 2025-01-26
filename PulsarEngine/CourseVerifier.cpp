#include <kamek.hpp>
#include <PulsarSystem.hpp>
#include <AntiCheat.hpp>
#include <core/rvl/NET/NETDigest.hpp>
#include <MarioKartWii/Archive/ArchiveMgr.hpp>
#include <core/rvl/dvd/dvd.hpp>
#include <core/egg/mem/Heap.hpp>
#include <SLotExpansion/CupsConfig.hpp>
#include <SZSHashes.hpp>
#include <SlotExpansion/CupsConfig.hpp>
#include <core/rvl/os/OS.hpp>

template<typename T>
T Min(T a, T b) {
    return (a < b) ? a : b;
}

namespace AntiCheat {

void CalculateSHA1(const void* data, u32 length, u8* hashOut) {
    NET::SHA1Context ctx;
    NET::SHA1Init(&ctx);
    NET::SHA1Update(&ctx, data, length);
    NET::SHA1GetDigest(&ctx, hashOut);
}

void HashToString(const u8* hash, char* strOut) {
    static const char* hexChars = "0123456789abcdef";
    for (int i = 0; i < SHA1_DIGEST_SIZE; i++) {
        strOut[i * 2] = hexChars[hash[i] >> 4];
        strOut[i * 2 + 1] = hexChars[hash[i] & 0xf];
    }
    strOut[SHA1_DIGEST_SIZE * 2] = '\0';
}

const char* GetFilenameFromPath(const char* filepath) {
    const char* filename = filepath;
    for (int i = 0; filepath[i] != '\0'; i++) {
        if (filepath[i] == '/') filename = &filepath[i + 1];
    }
    return filename;
}

static void BytesToHexString(const u8* bytes, int length, char* output) {
    static const char hexChars[] = "0123456789abcdef";
    for (int i = 0; i < length; i++) {
        output[i * 2] = hexChars[bytes[i] >> 4];
        output[i * 2 + 1] = hexChars[bytes[i] & 0xF];
    }
    output[length * 2] = '\0';
}

static bool VerifyTrackHash(const char* filename) {
    OS::Report("Checking hash for file: %s\n", filename);
    
    DVD::FileInfo fileInfo;
    if (!DVD::Open(filename, &fileInfo)) {
        OS::Report("Failed to open file: %s\n", filename);
        return false;
    }

    // Get file size first
    s32 fileSize = fileInfo.length;
    if (fileSize <= 0) {
        OS::Report("Invalid file size for: %s\n", filename);
        DVD::Close(&fileInfo);
        return false;
    }

    NET::SHA1Context sha1;
    NET::SHA1Init(&sha1);

    // Use a 32KB buffer instead of loading the whole file
    alignas(32) u8 buffer[32 * 1024];
    s32 bytesRead;
    s32 offset = 0;
    
    while (offset < fileSize && 
           (bytesRead = DVD::ReadPrio(&fileInfo, buffer, 
                                    (u32)Min((s32)sizeof(buffer), fileSize - offset), 
                                    offset, 2)) > 0) {
        NET::SHA1Update(&sha1, buffer, bytesRead);
        offset += bytesRead;
    }
    
    DVD::Close(&fileInfo);

    // Check if we read the entire file
    if (offset != fileSize) {
        OS::Report("Failed to read entire file: %s (read %d/%d bytes)\n", 
                  filename, offset, fileSize);
        return false;
    }

    u8 hash[20];
    NET::SHA1GetDigest(&sha1, hash);

    char hashStr[41];
    BytesToHexString(hash, 20, hashStr);
    OS::Report("Calculated hash: %s\n", hashStr);

    const char* baseFilename = GetFilenameFromPath(filename);
    for (u32 i = 0; i < sizeof(SZS_HASHES) / sizeof(SZS_HASHES[0]); i++) {
        if (strcmp(SZS_HASHES[i].filename, baseFilename) == 0) {
            OS::Report("Expected hash: %s\n", SZS_HASHES[i].expectedHash);
            return strcmp(hashStr, SZS_HASHES[i].expectedHash) == 0;
        }
    }
    OS::Report("No matching hash found in SZS_HASHES for: %s\n", filename);
    return false;
}

static void OnTrackLoad() {
    char filepath[256];
    const Pulsar::CupsConfig* cupsConfig = Pulsar::CupsConfig::sInstance;
    Pulsar::PulsarId pulsarId = cupsConfig->GetWinning();
    CourseId realId = Pulsar::CupsConfig::ConvertTrack_PulsarIdToRealId(pulsarId);
    
    snprintf(filepath, sizeof(filepath), "Race/Course/%d.szs", realId);
    OS::Report("Verifying track: %s\n", filepath);
    
    if (!VerifyTrackHash(filepath)) {
        OS::Report("Track verification failed for %s!\n", filepath);
        Pulsar::Debug::FatalError("Track file failed integrity check.");
    }
}

static RaceLoadHook TrackVerifier(OnTrackLoad);

} // namespace AntiCheat