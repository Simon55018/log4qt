#include <QCoreApplication>
#include <QDate>
#include <QDir>
#include <QThread>

#include "CLog4qt_p.h"
#include "log4qt/logger.h"
#include "log4qt/patternlayout.h"
#include "log4qt/logmanager.h"

#define OUTPUT_FORMAT   "[File:(%1) Line:(%2)] --> %3"
#define PATTERN_FORMAT  "[%p] %d --> %m %n"
#define LOG_ABSOLUTE_DIR_PATH    \
    QCoreApplication::applicationDirPath() + "/logs/" + QDate::currentDate().toString("yyyyMM") + "/"
#define LOG_FILE_NAME   QDate::currentDate().toString("yyyy-MM-dd") + ".log"

namespace Log4Qt
{
    CLog4qt* CLog4qt::m_pThis = NULL;
    CLog4qt *CLog4qt::getInstance()
    {
        if( NULL == m_pThis )
        {
            m_pThis = new CLog4qt;
        }

        return m_pThis;
    }

    void CLog4qt::deleteInstance()
    {
        if( NULL != m_pThis )
        {
            delete m_pThis;
            m_pThis = NULL;
        }
    }

    CLog4qt::~CLog4qt()
    {
    }

    void CLog4qt::setHandleQtMessage(const bool bSet)
    {
        LogManager::setHandleQtMessages(bSet);
    }

    void CLog4qt::setThreshold(const LogType emLogType)
    {
        Q_D(CLog4qt);

        Level level((Level::Value)emLogType);
        d->m_appender->setThreshold(level);
    }

    void CLog4qt::log(const LogType emLogType, const QString &sLog)
    {
        Q_D(CLog4qt);
        LogInfo stLogInfo;
        stLogInfo.emLogType = emLogType;
        stLogInfo.sLog = sLog;

        d->writeLog(stLogInfo);
    }

    void CLog4qt::debug(const QString sLog)
    {
        getInstance()->log(EM_DEBUG, sLog);
    }

    void CLog4qt::info(const QString sLog)
    {
        getInstance()->log(EM_INFO, sLog);
    }

    void CLog4qt::warn(const QString sLog)
    {
        getInstance()->log(EM_WARN, sLog);
    }

    void CLog4qt::error(const QString sLog)
    {
        getInstance()->log(EM_ERROR, sLog);
    }

    void CLog4qt::fatal(const QString sLog)
    {
        getInstance()->log(EM_FATAL, sLog);
    }

    void CLog4qt::debug(const char *pucFilename, const int lLine, const QString sLog)
    {
        QString sLogDetail = QString(OUTPUT_FORMAT)
                .arg(QString::fromLatin1(pucFilename)).arg(lLine).arg(sLog);
        debug(sLogDetail);
    }

    void CLog4qt::info(const char *pucFilename, const int lLine, const QString sLog)
    {
        QString sLogDetail = QString(OUTPUT_FORMAT)
                .arg(QString::fromLatin1(pucFilename)).arg(lLine).arg(sLog);
        info(sLogDetail);
    }

    void CLog4qt::warn(const char *pucFilename, const int lLine, const QString sLog)
    {
        QString sLogDetail = QString(OUTPUT_FORMAT)
                .arg(QString::fromLatin1(pucFilename)).arg(lLine).arg(sLog);
        warn(sLogDetail);
    }

    void CLog4qt::error(const char *pucFilename, const int lLine, const QString sLog)
    {
        QString sLogDetail = QString(OUTPUT_FORMAT)
                .arg(QString::fromLatin1(pucFilename)).arg(lLine).arg(sLog);
        error(sLogDetail);
    }

    void CLog4qt::fatal(const char *pucFilename, const int lLine, const QString sLog)
    {
        QString sLogDetail = QString(OUTPUT_FORMAT)
                .arg(QString::fromLatin1(pucFilename)).arg(lLine).arg(sLog);
       getInstance()->fatal(sLogDetail);
    }

    CLog4qt::CLog4qt(QObject *parent)
        : QObject(parent), d_ptr(new CLog4qtPrivate)
    {
        d_ptr->q_ptr = this;
    }

    CLog4qtPrivate::CLog4qtPrivate(QObject *parent)
        : QThread(parent)
    {
        q_ptr = NULL;

        m_logger = Log4Qt::Logger::rootLogger();
        PatternLayout *layout = new PatternLayout(PatternLayout::DEFAULT_CONVERSION_PATTERN);
        layout->setConversionPattern(PATTERN_FORMAT);
        layout->activateOptions();

        m_appender = new RollingFileAppender;
        m_appender->setLayout(layout);
        m_appender->setFile(LOG_ABSOLUTE_DIR_PATH + LOG_FILE_NAME);
        m_appender->setAppendFile(true);
        m_appender->setImmediateFlush(true);
        m_appender->activateOptions();

        m_logger->addAppender(m_appender);

        start();
    }

    CLog4qtPrivate::~CLog4qtPrivate()
    {
        if( isRunning() )
        {
            quit();
            wait();
        }

        if( NULL != m_appender )
        {
            delete m_appender;
            m_appender = NULL;
        }
    }

    void CLog4qtPrivate::writeLog(LogInfo stLogInfo)
    {
        QMutexLocker locker(&m_mutex);
        m_listLogInfo.append(stLogInfo);

        m_waitCondition.wakeOne();
    }

    void CLog4qtPrivate::run()
    {
        while(1)
        {
            runOnce();

            sleep(0);
        }
    }

    void CLog4qtPrivate::runOnce()
    {
        QList<LogInfo> listLogInfo;
        QMutexLocker locker(&m_mutex);
        listLogInfo.swap(m_listLogInfo);
        if( listLogInfo.isEmpty() && !m_waitCondition.wait(&m_mutex, 10000) )
        {
            return;
        }

        m_appender->setFile(LOG_ABSOLUTE_DIR_PATH + LOG_FILE_NAME);
        m_appender->activateOptions();

        QList<LogInfo>::const_iterator iter = listLogInfo.constBegin();
        for( ; iter != listLogInfo.constEnd(); ++iter)
        {
            const LogInfo stLogInfo = *iter;

            switch (stLogInfo.emLogType)
            {
            case EM_DEBUG:
                m_logger->debug(stLogInfo.sLog);
                break;

            case EM_INFO:
                m_logger->info(stLogInfo.sLog);
                break;

            case EM_WARN:
                m_logger->warn(stLogInfo.sLog);
                break;

            case EM_ERROR:
                m_logger->error(stLogInfo.sLog);
                break;

            case EM_FATAL:
                m_logger->fatal(stLogInfo.sLog);
                break;

            default:
                break;
            }
        }
    }
}

