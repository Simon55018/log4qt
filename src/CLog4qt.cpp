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

        d->writeLog(emLogType, sLog);
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

        // 获取rootLogger指针
        m_logger = Log4Qt::Logger::rootLogger();
        // 设置输出日志格式
        PatternLayout *layout = new PatternLayout(PatternLayout::DEFAULT_CONVERSION_PATTERN);
        layout->setConversionPattern(PATTERN_FORMAT);
        // 函数并没有任何调用,这句是为了完善逻辑
        layout->activateOptions();

        // 检查日志的目录是否存在,若不存在则创建日志目录
        QDir dir(LOG_ABSOLUTE_DIR_PATH);
        if( !dir.exists() )
        {
            dir.mkpath(LOG_ABSOLUTE_DIR_PATH);
        }

        // 滚动文件输出器
        m_appender = new RollingFileAppender;
        // 设置日志格式
        m_appender->setLayout(layout);
        // 设置日志名称
        m_appender->setFile(LOG_ABSOLUTE_DIR_PATH + LOG_FILE_NAME);
        // 设置在日志后面增加新的信息
        m_appender->setAppendFile(true);
        // 设置马上输出日志信息
        m_appender->setImmediateFlush(true);
        // 设置输出器
        m_appender->activateOptions();

        // 增加文本输出器
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

    void CLog4qtPrivate::writeLog(const LogInfo stLogInfo)
    {
        // 上锁
        QMutexLocker locker(&m_mutex);
        // 增加日志信息
        m_listLogInfo.append(stLogInfo);

        // 开启一次线程
        m_waitCondition.wakeOne();
    }

    void CLog4qtPrivate::writeLog(const LogType emLogType, const QString sLog)
    {
        LogInfo stLogInfo;
        stLogInfo.emLogType = emLogType;
        stLogInfo.sLog = sLog;

        writeLog(stLogInfo);
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
        listLogInfo.swap(m_listLogInfo);    // 获取日志信息
        // 判断日志信息是否为空, 等待10s
        if( listLogInfo.isEmpty() && !m_waitCondition.wait(&m_mutex, 10000) )
        {
            return;
        }

        // 设置输出文件名(为了程序一直运行时, 第二天时会重新创建一些的文件)
        m_appender->setFile(LOG_ABSOLUTE_DIR_PATH + LOG_FILE_NAME);
        m_appender->activateOptions();

        // 获取日志链表
        QList<LogInfo>::const_iterator iter = listLogInfo.constBegin();
        for( ; iter != listLogInfo.constEnd(); ++iter)
        {
            const LogInfo stLogInfo = *iter;

            // 根据日志类型输出日志
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

    CLogBase::CLogBase(LogType emLogType, const char *pucFileName, int lLine)
        : m_emLogType(emLogType), m_pucFileName(pucFileName), m_lLine(lLine)
    {

    }

    CLogBase::~CLogBase()
    {
        // 获知日志信息
        QList<QVariant>::const_iterator iter = m_listVarData.constBegin();

        QString sLog;
        // 生成日志信息
        for( ; m_listVarData.constEnd() != iter ; ++iter)
        {
            QVariant var = *iter;
            sLog.append(var.toString());
        }

        if( NULL != m_pucFileName )
        {
            sLog = QString(OUTPUT_FORMAT).arg(QString::fromLatin1(m_pucFileName))
                                         .arg(m_lLine).arg(sLog);
        }

        // 根据日志类型输出日志
        switch (m_emLogType)
        {
        case EM_DEBUG:
            CLog4qt::debug(sLog);
            break;

        case EM_INFO:
            CLog4qt::info(sLog);
            break;

        case EM_WARN:
            CLog4qt::warn(sLog);
            break;

        case EM_ERROR:
            CLog4qt::error(sLog);
            break;

        case EM_FATAL:
            CLog4qt::fatal(sLog);
            break;

        default:
            break;
        }
    }

    CLogBase &CLogBase::operator <<(const QVariant var)
    {
        // 每次操作<<运算符就会增加一个日志信息
        m_listVarData.push_back(var);

        return *this;
    }
}

