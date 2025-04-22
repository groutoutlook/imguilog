#include "ILog.hpp"
#include <ctime>
#include "ILogImGui.hpp"

#if _MSC_VER && !__INTEL_COMPILER
    #define _CRT_SECURE_NO_WARNINGS
#endif

#define TIME_COUNT(x) std::chrono::time_point_cast<std::chrono::microseconds>(x).time_since_epoch().count()

void ILog::Timer::start() noexcept
{
    const auto tmp = std::chrono::high_resolution_clock::now();
    // scary pointer bullshit because of the C API
    timer.startPos = *static_cast<const double* const>(static_cast<const void* const>(&tmp));
}

void ILog::Timer::stop() noexcept
{
    const auto endTime = std::chrono::high_resolution_clock::now();
    timer.duration = static_cast<double>(TIME_COUNT(endTime) - TIME_COUNT(
        // scary pointer bullshit because of the C API
        *static_cast<const std::chrono::time_point<std::chrono::high_resolution_clock>* const>(
            static_cast<const void* const>(&timer.startPos)
        )
    )) * 0.001;
}

ILog::Timer::~Timer() noexcept
{
    stop();
}

double ILog::Timer::get() const noexcept
{
    return timer.duration;
}

void ILog::Logger::setCrashOnError(const bool bError) noexcept
{
    LoggerInternal::get().bUsingErrors = bError;
}

void ILog::Logger::setEnableLogging(const bool bEnable) noexcept
{
    LoggerInternal::get().bLoggingEnabled = bEnable;
}

void ILog::Logger::setCurrentLogFile(const char* file) noexcept
{
    LoggerInternal::get().shutdownFileStream();
    LoggerInternal::get().fileout = std::ofstream(file);
}

void ILog::Logger::setLogOperation(const LogOperations op) noexcept
{
    LoggerInternal::get().operationType = op;
}

void ILog::Logger::log(const char* message, const LogType type) noexcept
{
    log(message, type, "");
}

std::string ILog::LoggerInternal::getCurrentTime() noexcept
{
    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::string realTime = std::ctime(&now);
    realTime.erase(24);

    return realTime;
}

void ILog::LoggerInternal::shutdownFileStream() noexcept
{
    fileout.close();
}

ILog::LoggerInternal::LoggerInternal() noexcept
{
#ifdef ILOG_IMGUI
    const CommandType clear =
    {
        .cmd = "clear",
        .cmdHint = "Clears the scroll buffer",
        .func = [&](const std::string&){ messageLog.clear(); },
    };

    const CommandType help
    {
        .cmd = "help",
        .cmdHint = "Sends a help message",
        .func = ILog::ImGuiConsole::showHelpMessage
    };

    commands.emplace_back(clear);
    commands.emplace_back(help);
#endif
}

ILog::LoggerInternal::~LoggerInternal() noexcept
{
    shutdownFileStream();
}

ILog::LoggerInternal* ILog::LoggerInternal::getWithCreate() noexcept
{
    static LoggerInternal logger{};
    return &logger;
}

ILog::LoggerInternal& ILog::LoggerInternal::get(LoggerInternal* lg) noexcept
{
    static LoggerInternal* logger = lg == nullptr ? getWithCreate() : lg;
    return *logger;
}
