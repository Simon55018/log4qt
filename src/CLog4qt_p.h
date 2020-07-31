#ifndef CLOG4QT_P_H
#define CLOG4QT_P_H

#include "CLog4qt.h"
#include "log4qt/logger.h"
#include "log4qt/rollingfileappender.h"

#include <QThread>
#include <QWaitCondition>

namespace Log4Qt
{
    struct LogInfo
    {
        LogType     emLogType;
        QString     sLog;
    };

    class CLog4qtPrivate : public QThread
    {
        Q_OBJECT
        Q_DECLARE_PUBLIC(CLog4qt)

    public:
        explicit CLog4qtPrivate(QObject *parent = 0);

        virtual ~CLog4qtPrivate();

        void writeLog(const LogInfo stLogInfo);

        void writeLog(const LogType emLogType, const QString sLog);

    protected:
        virtual void run();

    private:
        void runOnce();

    private:
        CLog4qt*    q_ptr;
        Logger      *m_logger;
        RollingFileAppender *m_appender;
        QList<LogInfo>      m_listLogInfo;
        QWaitCondition      m_waitCondition;
        QMutex              m_mutex;
    };
}
#endif
