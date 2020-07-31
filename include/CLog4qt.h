#ifndef CLOG4QT_H
#define CLOG4QT_H

#include <QtCore/qglobal.h>
#include <QObject>

#if defined(LOG4QT_LIBRARY)
#  define LOG4QTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LOG4QTSHARED_EXPORT Q_DECL_IMPORT
#endif

#define LOG_DEBUG(log)          Log4Qt::CLog4qt::debug(log);
#define LOG_INFO(log)           Log4Qt::CLog4qt::info(log);
#define LOG_WARN(log)           Log4Qt::CLog4qt::warn(log);
#define LOG_ERROR(log)          Log4Qt::CLog4qt::error(log);
#define LOG_FATAL(log)          Log4Qt::CLog4qt::fatal(log);

#define LOG_DEBUG_LOC(log)      Log4Qt::CLog4qt::debug(__FILE__, __LINE__, log);
#define LOG_INFO_LOC(log)       Log4Qt::CLog4qt::info(__FILE__, __LINE__, log);
#define LOG_WARN_LOC(log)       Log4Qt::CLog4qt::warn(__FILE__, __LINE__, log);
#define LOG_ERROR_LOC(log)      Log4Qt::CLog4qt::error(__FILE__, __LINE__, log);
#define LOG_FATAL_LOC(log)      Log4Qt::CLog4qt::fatal(__FILE__, __LINE__, log);

namespace Log4Qt
{
    enum LogType
    {
        EM_DEBUG = 96,
        EM_INFO = 128,
        EM_WARN = 150,
        EM_ERROR = 182,
        EM_FATAL = 214,
    };

    class CLog4qtPrivate;
    class LOG4QTSHARED_EXPORT CLog4qt : public QObject
    {
        Q_OBJECT
        Q_DECLARE_PRIVATE(CLog4qt)
        Q_DISABLE_COPY(CLog4qt)

    public:
        static CLog4qt *getInstance();
        static void deleteInstance();

        virtual ~CLog4qt();

        void setHandleQtMessage(const bool bSet);

        void setThreshold(const LogType emLogType);

        void log(const LogType emLogType, const QString &sLog);

        static void debug(const QString sLog);
        static void debug(const char* pucFilename, const int lLine, const QString sLog);

        static void info(const QString sLog);
        static void info(const char* pucFilename, const int lLine, const QString sLog);

        static void warn(const QString sLog);
        static void warn(const char* pucFilename, const int lLine, const QString sLog);

        static void error(const QString sLog);
        static void error(const char* pucFilename, const int lLine, const QString sLog);

        static void fatal(const QString sLog);
        static void fatal(const char* pucFilename, const int lLine, const QString sLog);

    private:
        explicit CLog4qt(QObject *parent = 0);

        static CLog4qt      *m_pThis;

        QScopedPointer<CLog4qtPrivate>  d_ptr;
    };

#define g_Log       (Log4Qt::CLog4qt::getInstance())
#define g_Log_Del   (Log4Qt::CLog4qt::deleteInstance())
}

#endif // CLOG4QT_H
