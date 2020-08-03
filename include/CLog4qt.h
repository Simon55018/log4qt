#ifndef CLOG4QT_H
#define CLOG4QT_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QVariant>

#if defined(LOG4QT_LIBRARY)
#  define LOG4QTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LOG4QTSHARED_EXPORT Q_DECL_IMPORT
#endif

// 以下宏用于输出不带文件名和行数信息的日志
#define LOG_DEBUG(log)          Log4Qt::CLog4qt::debug(log);
#define LOG_INFO(log)           Log4Qt::CLog4qt::info(log);
#define LOG_WARN(log)           Log4Qt::CLog4qt::warn(log);
#define LOG_ERROR(log)          Log4Qt::CLog4qt::error(log);
#define LOG_FATAL(log)          Log4Qt::CLog4qt::fatal(log);

// 以下宏用于输出含有文件名和行数信息的日志
#define LOG_DEBUG_LOC(log)      Log4Qt::CLog4qt::debug(__FILE__, __LINE__, log);
#define LOG_INFO_LOC(log)       Log4Qt::CLog4qt::info(__FILE__, __LINE__, log);
#define LOG_WARN_LOC(log)       Log4Qt::CLog4qt::warn(__FILE__, __LINE__, log);
#define LOG_ERROR_LOC(log)      Log4Qt::CLog4qt::error(__FILE__, __LINE__, log);
#define LOG_FATAL_LOC(log)      Log4Qt::CLog4qt::fatal(__FILE__, __LINE__, log);

// 以下宏用于多组信息组成的日志信息
#define logDebug                Log4Qt::CLog4qt::lDebug
#define logInfo                 Log4Qt::CLog4qt::lInfo
#define logWarn                 Log4Qt::CLog4qt::lWarn
#define logError                Log4Qt::CLog4qt::lError
#define logFatal                Log4Qt::CLog4qt::lFatal

// 以下宏用于多组信息组成的日志信息,包含代码的文件名及行号
#define logDebugWithLoc()       Log4Qt::CLog4qt::lDebug(__FILE__, __LINE__)
#define logInfoWithLoc()        Log4Qt::CLog4qt::lInfo(__FILE__, __LINE__)
#define logWarnWithLoc()        Log4Qt::CLog4qt::lWarn(__FILE__, __LINE__)
#define logErrorWithLoc()       Log4Qt::CLog4qt::lError(__FILE__, __LINE__)
#define logFatalWithLoc()       Log4Qt::CLog4qt::lFatal(__FILE__, __LINE__)

/**
 *  日志输出格式为[日志等级] 时间 --> 日志信息 /r/n
 *                 [%p]    %d -->   %m    %n
 *  暂时不支持修改输出格式.
 *  每日输出一份日志,包含设置等级以上的所有信息.
 */
namespace Log4Qt
{
    // 日志类型(等级)
    enum LogType
    {
        EM_NULL = 0,
        EM_DEBUG = 96,          // 调试输出
        EM_INFO = 128,          // 信息输出
        EM_WARN = 150,          // 警告输出
        EM_ERROR = 182,         // 错误输出
        EM_FATAL = 214,         // 致命错误输出
        EM_OFF = 255
    };

    /*!
     * \brief The CLogBase class    用于实现类似qDebug左移操作输出
     *                              如:logDebug() << a << b;
     *                              日志会输出: a b
     */
    class LOG4QTSHARED_EXPORT CLogBase
    {
    public:
        // 构造会设置日志类型
        explicit CLogBase(LogType emLogType, const char *pucFileName = 0,
                          const int lLine = 0);
        // 析构时会输出日志信息(仿qDebug实现)
        ~CLogBase();

        CLogBase &operator <<(const QVariant var);

    private:
        LogType     m_emLogType;    // 日志类型
        const char*       m_pucFileName;  // 文件名
        const int         m_lLine;        // 行号
        QList<QVariant>     m_listVarData;  // 日志信息
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

        /*!
         * \brief setHandleQtMessage    设置处理Qt内部输出(即将qt终端输出写入日志)
         * \param bSet                  是/否
         */
        void setHandleQtMessage(const bool bSet);

        /*!
         * \brief setThreshold          设置日志等级
         * \param emLogType             日志等级
         */
        void setThreshold(const LogType emLogType);

        /*!
         * \brief log           日志输出
         * \param emLogType     [in]    日志等级
         * \param sLog          [in]    日志信息
         */
        void log(const LogType emLogType, const QString &sLog);

        /*!
         * \brief debug         调试日志输出
         * \param sLog          [in]    日志信息
         */
        static void debug(const QString sLog);
        /*!
         * \brief debug         调试日志输出(主要用于输出日志输出的文件名和行号)
         * \param pucFilename   [in]    文件名
         * \param lLine         [in]    行号
         * \param sLog          [in]    日志信息
         */
        static void debug(const char* pucFilename, const int lLine, const QString sLog);

        /*!
         * \brief info          信息日志输出
         * \param sLog          [in]    日志信息
         */
        static void info(const QString sLog);
        /*!
         * \brief info          信息日志输出(主要用于输出日志输出的文件名和行号)
         * \param pucFilename   [in]    文件名
         * \param lLine         [in]    行号
         * \param sLog          [in]    日志信息
         */
        static void info(const char* pucFilename, const int lLine, const QString sLog);

        /*!
         * \brief warn          警告日志输出
         * \param sLog          [in]    日志信息
         */
        static void warn(const QString sLog);
        /*!
         * \brief warn          警告日志输出(主要用于输出日志输出的文件名和行号)
         * \param pucFilename   [in]    文件名
         * \param lLine         [in]    行号
         * \param sLog          [in]    日志信息
         */
        static void warn(const char* pucFilename, const int lLine, const QString sLog);

        /*!
         * \brief error         错误日志输出
         * \param sLog          [in]    日志信息
         */
        static void error(const QString sLog);
        /*!
         * \brief error         错误日志输出(主要用于输出日志输出的文件名和行号)
         * \param pucFilename   [in]    文件名
         * \param lLine         [in]    行号
         * \param sLog          [in]    日志信息
         */
        static void error(const char* pucFilename, const int lLine, const QString sLog);

        /*!
         * \brief fatal         致命错误日志输出
         * \param sLog          [in]    日志信息
         */
        static void fatal(const QString sLog);
        /*!
         * \brief fatal         致命错误日志输出(主要用于输出日志输出的文件名和行号)
         * \param pucFilename   [in]    文件名
         * \param lLine         [in]    行号
         * \param sLog          [in]    日志信息
         */
        static void fatal(const char* pucFilename, const int lLine, const QString sLog);

        /*!
         * \brief lDebug        日志debug输出对象
         * \return CLogBase对象
         */
        inline static CLogBase lDebug() { return CLogBase(EM_DEBUG); }
        inline static CLogBase lDebug(const char *pucFileName, const int lLine)
        { return CLogBase(EM_DEBUG, pucFileName, lLine); }
        /*!
         * \brief lInfo         日志info输出对象
         * \return CLogBase对象
         */
        inline static CLogBase lInfo()  { return CLogBase(EM_INFO); }
        inline static CLogBase lInfo(const char *pucFileName, const int lLine)
        { return CLogBase(EM_INFO, pucFileName, lLine); }
        /*!
         * \brief lWarn         日志warn输出对象
         * \return CLogBase对象
         */
        inline static CLogBase lWarn()  { return CLogBase(EM_WARN); }
        inline static CLogBase lWarn(const char *pucFileName, const int lLine)
        { return CLogBase(EM_WARN, pucFileName, lLine); }
        /*!
         * \brief lError        日志error输出对象
         * \return CLogBase对象
         */
        inline static CLogBase lError() { return CLogBase(EM_ERROR); }
        inline static CLogBase lError(const char *pucFileName, const int lLine)
        { return CLogBase(EM_ERROR, pucFileName, lLine); }
        /*!
         * \brief lFatal        日志fatal输出对象
         * \return CLogBase对象
         */
        inline static CLogBase lFatal() { return CLogBase(EM_FATAL); }
        inline static CLogBase lFatal(const char *pucFileName, const int lLine)
        { return CLogBase(EM_FATAL, pucFileName, lLine); }

    private:
        explicit CLog4qt(QObject *parent = 0);

        static CLog4qt      *m_pThis;

        QScopedPointer<CLog4qtPrivate>  d_ptr;
    };

#define g_Log       (Log4Qt::CLog4qt::getInstance())
#define g_Log_Del   (Log4Qt::CLog4qt::deleteInstance())
}

#endif // CLOG4QT_H
