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

    // Overloaded LogDebug functions to support formatted logging with integer arguments
    void LogDebug(const char* format, int arg1);
    void LogDebug(const char* format, int arg1, int arg2);
    void LogDebug(const char* format, int arg1, int arg2, int arg3);
    void LogDebug(const char* format, int arg1, int arg2, int arg3, int arg4);
    void LogDebug(const char *format, void *ptr);
    void LogDebug(const char *format, const char *arg1, const char *arg2);
    void LogDebug(const char* format, const void* ptr);

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
