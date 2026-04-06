class Logger {
    int logCount;

    Logger() : logCount(0) {}             // private constructor
    ~Logger() = default;

public:
    // deleted to enforce single instance
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& getInstance() {
        static Logger instance; // created once, on first call (C++11 guarantees thread safety)
        return instance;
    }

    void log(const char* message) {
        ++logCount;
        // write message somewhere...
    }

    int getLogCount() const { return logCount; }
};