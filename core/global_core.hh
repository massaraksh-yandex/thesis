#ifndef GLOBAL_CORE_HH
#define GLOBAL_CORE_HH

#include <QSharedPointer>
#include "../CImg.h"
#include "../global_defines.hh"
#include "logging.hh"
#include <stdexcept>

#define ADD_LOGGING \
    signals: \
        void log(Log::LogType type, int indent, QString message); \
        void progress(int value, int maximum);\

#define ADD_PROGRESS \
    signals: \
        void progress(int value, int maximum);\


#define ADD_INTERRUPTABLE(WATCHDOG) \
    public: \
        void interrupt() { _interrupt = true; WATCHDOG.cancel(); WATCHDOG.waitForFinished(); } \
    private: \
        bool _interrupt; \



#endif // GLOBAL_CORE_HH
