#ifndef _LOGGER_
#define _LOGGER_
#include <include/c_stdio.h>
#include <include/c_string.h>
#include <IO/IO.hpp>

namespace Pulsar {

class Logger {
public:
    static Logger& GetInstance();

    bool Init(IOType type, EGG::Heap* heap, EGG::TaskThread* taskThread, bool debugMode = true);

    void LogInfo(const char* message);
    void LogError(const char* message);
    void LogDebug(const char* message);

private:
    Logger();
    ~Logger();

    IO* logFileIO;
    bool initialized;
    char logFilePath[256];
    bool isWriting;
    bool debugMode;

    static const size_t MAX_LOG_SIZE = 1024;
};

} // namespace Pulsar

#endif // _LOGGER_
