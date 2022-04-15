#pragma once

//? Carry the state of the calling thread
//TODO: Full filename vs small
//TODO: File output
//TODO: Abstract platform-specific code further
//TODO: date printing
//?Use less includes
//TODO: Release build
//TODO: Every call to logger functions must be through macros so they can be stripped out on release builds
//? fix out of order lock acquisition when two or more threads are waiting
//? Different thread for output object

#include <cstdio>
#include <ctime>
#include <string.h>

#include <sstream>
#include <mutex>
#include <chrono>
#include <iomanip>

#define STRINGIFY_EXPANDED(x) STRINGIFY_DEFINE(x)
#define STRINGIFY_DEFINE(x) #x

#ifdef LOGGING
#ifdef LOGCOLOR

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define WIN_COLOR
#define color_error_orange    6 //error
#define color_trace_light_gray 7 //trace
#define color_info_blue       9 //info
#define color_debug_green     10 //debug
#define color_critical_red       12 //critical
#define color_warn_yellow    14 //warn
#define color_default_white 15
#include <windows.h>
#endif


#ifdef __linux__
#define LINUX_COLOR
#define color_reset 0 //reset
#define color_error_orange    33 //error
#define color_trace_light_gray 37 //trace
#define color_info_blue       94 //info
#define color_debug_green     92 //debug
#define color_critical_red       91 //critical
#define color_warn_yellow    93 //warn
#define color_default_white 97
#endif

#endif


namespace Logger {
constexpr int MESSAGE_LENGTH = 500;

enum class Priority : unsigned char {
    TRACE = 0,
    DEBUG,
    INFO,
    WARN,
    ERR,
    CRITICAL,
};

enum Items : unsigned short {
    TIME = 1 << 0,
    LEVEL = 1 << 1,
    STRING = 1 << 2,
    LINE = 1 << 3,
    FILE = 1 << 4,
};

union Format {
    struct {
        unsigned int time : 1;
        unsigned int level : 1;
        unsigned int string : 1;
        unsigned int line : 1;
        unsigned int file : 1;
        unsigned int unused : 3;
    };
    unsigned int print_state = 0;

    Format() : print_state(0) {}
    Format(int ps) : print_state(ps) {}
};

struct State {
private:
    Priority min_priority = Priority::INFO;
    Format curr_format = { Items::LEVEL | Items::STRING };

    friend struct Output;

public:
    static void set_state(Priority new_priority) {
        get_instance().min_priority = new_priority;
    }

    static void set_state(Format new_format) {
        get_instance().curr_format = new_format;
    }

    static void set_state(Priority new_priority, Format new_format) {
        get_instance().curr_format = new_format;
        get_instance().min_priority = new_priority;
    }

private:
    State() {    }

    State(State const&) = delete;
    State& operator=(State const&) = delete;

    ~State() {
    }

    static State& get_instance() {
        thread_local State state;
        return state;
    }

};


struct Output {
private:
    std::mutex output_mutex;
#ifdef WIN_COLOR
    WORD default_colors = 0;
#endif

public:
    template<typename... Args>
    static void trace(int line, const char* source_file, const char* message, Args... args)
    {
        get_instance().print(line, source_file, "[Trc]\t", Logger::Priority::TRACE, message, args...);
    }
    
    template<typename... Args>
    static void debug(int line, const char* source_file, const char* message, Args... args)
    {
        get_instance().print(line, source_file, "[Dbg]\t", Logger::Priority::DEBUG, message, args...);
    }

    template<typename... Args>
    static void info(int line, const char* source_file, const char* message, Args... args)
    {
        get_instance().print(line, source_file, "[Info]\t", Logger::Priority::INFO, message, args...);
    }

    template<typename... Args>
    static void warn(int line, const char* source_file, const char* message, Args... args)
    {
        get_instance().print(line, source_file, "[Warn]\t", Logger::Priority::WARN, message, args...);
    }

    template<typename... Args>
    static void error(int line, const char* source_file, const char* message, Args... args)
    {
        get_instance().print(line, source_file, "[Err]\t", Logger::Priority::ERR, message, args...);
    }

    template<typename... Args>
    static void critical(int line, const char* source_file, const char* message, Args... args)
    {
        get_instance().print(line, source_file, "[Crit]\t", Logger::Priority::CRITICAL, message, args...);
    }

    template <typename... Args>
    static void ctrace(int line, const char *source_file, Args... args)
    {
        get_instance().cprint(line, source_file, "[Trc]\t", Logger::Priority::TRACE, args...);
    }

    template <typename... Args>
    static void cdebug(int line, const char *source_file, Args... args)
    {
        get_instance().cprint(line, source_file, "[Dbg]\t", Logger::Priority::DEBUG, args...);
    }

    template <typename... Args>
    static void cinfo(int line, const char *source_file, Args... args)
    {
        get_instance().cprint(line, source_file, "[Info]\t", Logger::Priority::INFO, args...);
    }

    template <typename... Args>
    static void cwarn(int line, const char *source_file, Args... args)
    {
        get_instance().cprint(line, source_file, "[Warn]\t", Logger::Priority::WARN, args...);
    }

    template <typename... Args>
    static void cerror(int line, const char *source_file, Args... args)
    {
        get_instance().cprint(line, source_file, "[Err]\t", Logger::Priority::ERR, args...);
    }

    template <typename... Args>
    static void ccritical(int line, const char *source_file, Args... args)
    {
        get_instance().cprint(line, source_file, "[Crit]\t", Logger::Priority::CRITICAL, args...);
    }

private:
    Output() {
#ifdef WIN_COLOR
        get_console_defaults();
#endif
        printf("Output Instance Created\n");
    }

#ifdef WIN_COLOR
    void get_console_defaults()
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        {
            default_colors = csbi.wAttributes;
        }
    }
#endif

    Output(Output const&) = delete;
    Output& operator=(Output const&) = delete;

    ~Output() {
        printf("Output Instance destroyed");
    }

    static Output& get_instance() {
        static Output output;
        return output;
    }

    std::string time_in_HH_MM_SS_MMM_MMMM()
    {
        using namespace std::chrono;

        // get current time
        auto now = system_clock::now();

        // get number of milliseconds for the current second
        // (remainder after division into seconds)
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        auto mksec = duration_cast<microseconds>(now.time_since_epoch()) % 1000;

        // convert to std::time_t in order to convert to std::tm (broken time)
        auto timer = system_clock::to_time_t(now);

        // convert to broken time
        std::tm bt = *std::localtime(&timer);

        std::ostringstream oss;

        oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count() << "." << mksec.count();

        return oss.str();
    }

    template<typename... Args>
    void print(int line_number, const char* source_file, const char* message_priority_str, Logger::Priority message_priority, const char* message, Args... args)
    {
        if (State::get_instance().min_priority > message_priority) {
            return;
        }

#ifdef WIN_COLOR
        int text_color = color_default_white;

        switch(message_priority) {
            case Priority::TRACE : {
                text_color = color_trace_light_gray;
                break;
            }
            case Priority::DEBUG : {
                text_color = color_debug_green;
                break;
            }
            case Priority::INFO : {
                text_color = color_info_blue;
                break;
            }
            case Priority::WARN : {
                text_color = color_warn_yellow;
                break;
            }
            case Priority::ERR : {
                text_color = color_error_orange;
                break;
            }
            case Priority::CRITICAL : {
                text_color = color_critical_red;
                break;
            }
        }

#endif

        std::ostringstream ss;

#ifdef LINUX_COLOR
        ss << "\033[";
        switch (message_priority) {
            case Priority::TRACE: {
                ss << STRINGIFY_EXPANDED(color_trace_light_gray);
                break;
            }
            case Priority::DEBUG: {
                ss << STRINGIFY_EXPANDED(color_debug_green);
                break;
            }
            case Priority::INFO: {
                ss << STRINGIFY_EXPANDED(color_info_blue);
                break;
            }
            case Priority::WARN: {
                ss << STRINGIFY_EXPANDED(color_warn_yellow);
                break;
            }
            case Priority::ERR: {
                ss << STRINGIFY_EXPANDED(color_error_orange);
                break;
            }
            case Priority::CRITICAL: {
                ss << STRINGIFY_EXPANDED(color_critical_red);
                break;
            }
        }
        ss << "m";
#endif

        if (State::get_instance().curr_format.time) {            
            ss << time_in_HH_MM_SS_MMM_MMMM() << "  \t";
        }

        if (State::get_instance().curr_format.level) {
            ss << message_priority_str << " ";
        }

        if (State::get_instance().curr_format.string) {
            char buffer_m[MESSAGE_LENGTH];
            //!MESSAGE WILL BE CUT-OFF IF STRING AFTER FORMATTING IS GREATER THAN MESSAGE_LENGTH
            int size = snprintf(buffer_m, MESSAGE_LENGTH, message, args...);
            if (size + 1 > MESSAGE_LENGTH) {
                char* big_buffer = (char*)malloc((size + 1) * sizeof(char));
                snprintf(big_buffer, size + 1, message, args...);
                ss << big_buffer << " \t";
                free(big_buffer);
            }
            else {
                ss << buffer_m << " \t";
            }
        }

        if (State::get_instance().curr_format.line) {
            ss << "on line number " << line_number << " in source file " << source_file;
        }

#ifdef __linux__
        ss << "\033[0m";
#endif
        ss << std::endl;

        std::string output_string = ss.str();

        
        output_mutex.lock();
#ifdef WIN_COLOR
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)(text_color));
#endif

        printf("%s", output_string.c_str());

#ifdef WIN_COLOR
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), default_colors);
#endif

        output_mutex.unlock();

        // if (file)
        // {
        //     fprintf(file, "%s\t", buffer_fd);
        //     fprintf(file, message_priority_str);
        //     fprintf(file, message, args...);
        //     fprintf(file, " on line %d in %s", line_number, source_file);
        //     fprintf(file, "\n");
        // }
    }

    template <typename T>
    void push_into_sstream(std::ostream& ss, T item) {
        ss << item << " ";
    }

    template <typename T, typename... Args>
    void push_into_sstream(std::ostream& ss, T item, Args... args) {
        push_into_sstream(ss, item);
        push_into_sstream(ss, args...);
    }

    template <typename... Args>
    void cprint(int line_number, const char *source_file, const char *message_priority_str, Logger::Priority message_priority, Args... args)
    {
        if (State::get_instance().min_priority > message_priority)
        {
            return;
        }

        // printf("Using state instance: %llu\n", (unsigned long long)(&State::get_instance()));
        // printf("Format: %d", State::get_instance().curr_format.print_state);

        // char buffer_fd[80];
        // std::strftime(buffer_fd, 80, "%F %T", timestamp);

#ifdef WIN_COLOR
        int text_color = color_default_white;

        switch (message_priority)
        {
        case Priority::TRACE:
        {
            text_color = color_trace_light_gray;
            break;
        }
        case Priority::DEBUG:
        {
            text_color = color_debug_green;
            break;
        }
        case Priority::INFO:
        {
            text_color = color_info_blue;
            break;
        }
        case Priority::WARN:
        {
            text_color = color_warn_yellow;
            break;
        }
        case Priority::ERR:
        {
            text_color = color_error_orange;
            break;
        }
        case Priority::CRITICAL:
        {
            text_color = color_critical_red;
            break;
        }
        }

#endif

        std::ostringstream ss;

#ifdef LINUX_COLOR
        ss << "\033[";
        switch (message_priority)
        {
        case Priority::TRACE:
        {
            ss << STRINGIFY_EXPANDED(color_trace_light_gray);
            break;
        }
        case Priority::DEBUG:
        {
            ss << STRINGIFY_EXPANDED(color_debug_green);
            break;
        }
        case Priority::INFO:
        {
            ss << STRINGIFY_EXPANDED(color_info_blue);
            break;
        }
        case Priority::WARN:
        {
            ss << STRINGIFY_EXPANDED(color_warn_yellow);
            break;
        }
        case Priority::ERR:
        {
            ss << STRINGIFY_EXPANDED(color_error_orange);
            break;
        }
        case Priority::CRITICAL:
        {
            ss << STRINGIFY_EXPANDED(color_critical_red);
            break;
        }
        }
        ss << "m";
#endif

        if (State::get_instance().curr_format.time)
        {
            ss << time_in_HH_MM_SS_MMM_MMMM() << "  \t";
        }

        if (State::get_instance().curr_format.level)
        {
            ss << message_priority_str << " ";
        }

        if (State::get_instance().curr_format.string)
        {
            push_into_sstream(ss, args...);
            ss << " \t";
        }

        if (State::get_instance().curr_format.line)
        {
            ss << "on line number " << line_number << " in source file " << source_file;
        }

#ifdef __linux__
        ss << "\033[0m";
#endif
        ss << std::endl;

        std::string output_string = ss.str();

        output_mutex.lock();
#ifdef WIN_COLOR
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)(text_color));
#endif

        printf("%s", output_string.c_str());

#ifdef WIN_COLOR
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), default_colors);
#endif

        output_mutex.unlock();

        // if (file)
        // {
        //     fprintf(file, "%s\t", buffer_fd);
        //     fprintf(file, message_priority_str);
        //     fprintf(file, message, args...);
        //     fprintf(file, " on line %d in %s", line_number, source_file);
        //     fprintf(file, "\n");
        // }
    }
};

}

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG_TRACE(Message, ...) (Logger::Output::trace(__LINE__, __FILENAME__, Message, ##__VA_ARGS__))
#define LOG_DEBUG(Message, ...) (Logger::Output::debug(__LINE__, __FILENAME__, Message, ##__VA_ARGS__))
#define LOG_INFO(Message, ...) (Logger::Output::info(__LINE__, __FILENAME__, Message, ##__VA_ARGS__))
#define LOG_WARN(Message, ...) (Logger::Output::warn(__LINE__, __FILENAME__, Message, ##__VA_ARGS__))
#define LOG_ERROR(Message, ...) (Logger::Output::error(__LINE__, __FILENAME__, Message, ##__VA_ARGS__))
#define LOG_CRITICAL(Message, ...) (Logger::Output::critical(__LINE__, __FILENAME__, Message, ##__VA_ARGS__))

#define CLOG_TRACE(...) (Logger::Output::ctrace(__LINE__, __FILENAME__, __VA_ARGS__))
#define CLOG_DEBUG(...) (Logger::Output::cdebug(__LINE__, __FILENAME__, __VA_ARGS__))
#define CLOG_INFO(...) (Logger::Output::cinfo(__LINE__, __FILENAME__, __VA_ARGS__))
#define CLOG_WARN(...) (Logger::Output::cwarn(__LINE__, __FILENAME__, __VA_ARGS__))
#define CLOG_ERROR(...) (Logger::Output::cerror(__LINE__, __FILENAME__, __VA_ARGS__))
#define CLOG_CRITICAL(...) (Logger::Output::ccritical(__LINE__, __FILENAME__, __VA_ARGS__))

#define LOGGER_PRIORITY(p) (Logger::State::set_state(p))
#define LOGGER_FORMAT(f) (Logger::State::set_state(f))
#define LOGGER_STATE(p, f) (Logger::State::set_state(p, f))

#else
#define LOG_TRACE(Message, ...)
#define LOG_DEBUG(Message, ...)
#define LOG_INFO(Message, ...)
#define LOG_WARN(Message, ...)
#define LOG_ERROR(Message, ...)
#define LOG_CRITICAL(Message, ...)

#define CLOG_TRACE(Message, ...)
#define CLOG_DEBUG(Message, ...)
#define CLOG_INFO(Message, ...)
#define CLOG_WARN(Message, ...)
#define CLOG_ERROR(Message, ...)
#define CLOG_CRITICAL(Message, ...)

#define LOGGER_PRIORITY(p)
#define LOGGER_FORMAT(f)
#define LOGGER_STATE(p, f)

#endif