#include <IO/Logger.hpp>
#include <Debug/Debug.hpp>
#include <IO/RiivoIO.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {

Logger& Logger::GetInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : logFileIO(nullptr), initialized(false), isWriting(false) {
    logFilePath[0] = '\0';
}

Logger::~Logger() {
    if (logFileIO) {
        logFileIO->Close();
        logFileIO = nullptr;
    }
}

size_t custom_strlen(const char* str) {
    size_t length = 0;
    while (str[length] != '\0') {
        ++length;
    }
    return length;
}

bool Logger::Init(IOType type, EGG::Heap* heap, EGG::TaskThread* taskThread, bool debugMode) {
    if (initialized) {
        return true;
    }
    this->debugMode = debugMode;

    if (!heap) {
        Debug::FatalError("Logger::Init - Initialization failed: Heap is null.");
        return false;
    }
    if (!taskThread) {
        Debug::FatalError("Logger::Init - Initialization failed: TaskThread is null.");
        return false;
    }


    // Assuming IO::sInstance is initialized and ready to use
    const char* modFolder = System::sInstance->GetModFolder();
    logFileIO = IO::sInstance;
    snprintf(this->logFilePath, sizeof(this->logFilePath), "%s/Logs/Log.txt", modFolder);

    char logFolderPath[256];
    snprintf(logFolderPath, sizeof(logFolderPath), "%s/Logs", modFolder);

    if (!logFileIO->FolderExists(logFolderPath)) {
        if (!logFileIO->CreateFolder(logFolderPath)) {
            Debug::FatalError("Logger::Init - Failed to create Logs folder.");
            return false;
        }
    }

    // Open log.txt in write mode
    logFileIO->CreateAndOpen(logFilePath, FILE_MODE_APPEND);

    const char* initMessage = "Logger initialized successfully.\n";
    logFileIO->Overwrite(custom_strlen(initMessage), initMessage);

    // Close the file
    logFileIO->Close();

    initialized = true;
    return true;
}

void Logger::LogInfo(const char* message) {
    if (!initialized)
    {
        Debug::FatalError("Logger::LogInfo - Logger is not initialized.");
    }
    if (!logFileIO)
    {
        Debug::FatalError("Logger::LogInfo - Log file IO is null.");
    }
    logFileIO->CreateAndOpen(logFilePath, FILE_MODE_APPEND);
    char finalBuffer[MAX_LOG_SIZE];
    snprintf(finalBuffer, sizeof(finalBuffer), "[INFO] %s\n", message);
    logFileIO->Write(strlen(finalBuffer), finalBuffer);
    logFileIO->Close();
}

void Logger::LogError(const char* message) {
    if (!initialized || !logFileIO) return;
    logFileIO->CreateAndOpen(logFilePath, FILE_MODE_APPEND);
    char finalBuffer[MAX_LOG_SIZE];
    snprintf(finalBuffer, sizeof(finalBuffer), "[ERROR] %s\n", message);
    logFileIO->Write(strlen(finalBuffer), finalBuffer);
    logFileIO->Close();
}

void Logger::LogDebug(const char* message) {
    if (!initialized || !logFileIO || !debugMode) return;
    logFileIO->CreateAndOpen(logFilePath, FILE_MODE_APPEND);

    char finalBuffer[MAX_LOG_SIZE];
    snprintf(finalBuffer, sizeof(finalBuffer), "[DEBUG] %s\n", message);
    logFileIO->Write(custom_strlen(finalBuffer), finalBuffer);
    logFileIO->Close();
}

void Logger::LogDebug(const char* format, int arg1) {
    if (!initialized || !logFileIO || !debugMode) return;
    logFileIO->CreateAndOpen(logFilePath, FILE_MODE_APPEND);

    char finalBuffer[MAX_LOG_SIZE];
    snprintf(finalBuffer, sizeof(finalBuffer), "[DEBUG] %s\n");
    snprintf(finalBuffer + custom_strlen(finalBuffer), sizeof(finalBuffer) - custom_strlen(finalBuffer), format, arg1);
    logFileIO->Write(custom_strlen(finalBuffer), finalBuffer);
    logFileIO->Close();
}

void Logger::LogDebug(const char* format, int arg1, int arg2) {
    if (!initialized || !logFileIO || !debugMode) return;
    logFileIO->CreateAndOpen(logFilePath, FILE_MODE_APPEND);

    char finalBuffer[MAX_LOG_SIZE];
    snprintf(finalBuffer, sizeof(finalBuffer), "[DEBUG] %s\n");
    snprintf(finalBuffer + custom_strlen(finalBuffer), sizeof(finalBuffer) - custom_strlen(finalBuffer), format, arg1, arg2);
    logFileIO->Write(custom_strlen(finalBuffer), finalBuffer);
    logFileIO->Close();
}

void Logger::LogDebug(const char* format, int arg1, int arg2, int arg3) {
    if (!initialized || !logFileIO || !debugMode) return;
    logFileIO->CreateAndOpen(logFilePath, FILE_MODE_APPEND);

    char finalBuffer[MAX_LOG_SIZE];
    snprintf(finalBuffer, sizeof(finalBuffer), "[DEBUG] %s\n");
    snprintf(finalBuffer + custom_strlen(finalBuffer), sizeof(finalBuffer) - custom_strlen(finalBuffer), format, arg1, arg2, arg3);
    logFileIO->Write(custom_strlen(finalBuffer), finalBuffer);
    logFileIO->Close();

}

void Logger::LogDebug(const char* format, int arg1, int arg2, int arg3, int arg4) {
    if (!initialized || !logFileIO || !debugMode) return;
    logFileIO->CreateAndOpen(logFilePath, FILE_MODE_APPEND);

    char finalBuffer[MAX_LOG_SIZE];
    snprintf(finalBuffer, sizeof(finalBuffer), "[DEBUG] %s\n");
    snprintf(finalBuffer + custom_strlen(finalBuffer), sizeof(finalBuffer) - custom_strlen(finalBuffer), format, arg1, arg2, arg3, arg4);
    logFileIO->Write(custom_strlen(finalBuffer), finalBuffer);
    logFileIO->Close();
}

void Logger::LogDebug(const char* format, void* ptr) {
    if (!initialized || !logFileIO || !debugMode) return;
    logFileIO->CreateAndOpen(logFilePath, FILE_MODE_APPEND);

    char finalBuffer[MAX_LOG_SIZE];
    snprintf(finalBuffer, sizeof(finalBuffer), "[DEBUG] %s\n");
    snprintf(finalBuffer + custom_strlen(finalBuffer), sizeof(finalBuffer) - custom_strlen(finalBuffer), format, ptr);
    logFileIO->Write(custom_strlen(finalBuffer), finalBuffer);
    logFileIO->Close();
}

void Logger::LogDebug(const char* format, const char* arg1, const char* arg2) {
    if (!initialized || !logFileIO || !debugMode) return;
    logFileIO->CreateAndOpen(logFilePath, FILE_MODE_APPEND);

    char finalBuffer[MAX_LOG_SIZE];
    snprintf(finalBuffer, sizeof(finalBuffer), "[DEBUG] %s\n");
    snprintf(finalBuffer + custom_strlen(finalBuffer), sizeof(finalBuffer) - custom_strlen(finalBuffer), format, arg1, arg2);
    logFileIO->Write(custom_strlen(finalBuffer), finalBuffer);
    logFileIO->Close();
}

void Logger::LogDebug(const char* format, const void* ptr) {
    if (!initialized || !logFileIO || !debugMode) return;
    logFileIO->CreateAndOpen(logFilePath, FILE_MODE_APPEND);

    char finalBuffer[MAX_LOG_SIZE];
    snprintf(finalBuffer, sizeof(finalBuffer), "[DEBUG] %s\n");
    snprintf(finalBuffer + custom_strlen(finalBuffer), sizeof(finalBuffer) - custom_strlen(finalBuffer), format, ptr);
    logFileIO->Write(custom_strlen(finalBuffer), finalBuffer);
    logFileIO->Close();
}

void Logger::LogDebug(const char* format, const char* reason) {
    if (!initialized || !logFileIO) return;
    logFileIO->CreateAndOpen(logFilePath, FILE_MODE_APPEND);

    char finalBuffer[MAX_LOG_SIZE];
    snprintf(finalBuffer, sizeof(finalBuffer), "[REASON] %s\n");
    snprintf(finalBuffer + custom_strlen(finalBuffer), sizeof(finalBuffer) - custom_strlen(finalBuffer), format, reason);
    logFileIO->Write(custom_strlen(finalBuffer), finalBuffer);
    logFileIO->Close();
}

} // namespace Pulsar
