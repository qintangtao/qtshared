#include "MessagePattern.h"
#include <QMutexLocker>
#include <QVarLengthArray>
#include <QList>
#include <QCoreApplication>
#include <QThread>
#include <QElapsedTimer>
#include <QDateTime>

static QT_PREPEND_NAMESPACE(qint64) qt_gettid()
{
    QT_USE_NAMESPACE
    return qintptr(QThread::currentThreadId());
}

// tokens as recognized in QT_MESSAGE_PATTERN
static const char categoryTokenC[] = "%{category}";
static const char typeTokenC[] = "%{type}";
static const char messageTokenC[] = "%{message}";
static const char fileTokenC[] = "%{file}";
static const char lineTokenC[] = "%{line}";
static const char functionTokenC[] = "%{function}";
static const char pidTokenC[] = "%{pid}";
static const char appnameTokenC[] = "%{appname}";
static const char threadidTokenC[] = "%{threadid}";
static const char qthreadptrTokenC[] = "%{qthreadptr}";
static const char timeTokenC[] = "%{time"; //not a typo: this command has arguments
static const char backtraceTokenC[] = "%{backtrace"; //ditto
static const char ifCategoryTokenC[] = "%{if-category}";
static const char ifDebugTokenC[] = "%{if-debug}";
static const char ifInfoTokenC[] = "%{if-info}";
static const char ifWarningTokenC[] = "%{if-warning}";
static const char ifCriticalTokenC[] = "%{if-critical}";
static const char ifFatalTokenC[] = "%{if-fatal}";
static const char endifTokenC[] = "%{endif}";
static const char emptyTokenC[] = "";

static const char defaultPattern[] = "%{if-category}%{category}: %{endif}%{message}";


struct QMessagePattern {
    QMessagePattern();
    ~QMessagePattern();

    void setPattern(const QString &pattern);

    // 0 terminated arrays of literal tokens / literal or placeholder tokens
    const char **literals;
    const char **tokens;
    QList<QString> timeArgs;   // timeFormats in sequence of %{time

    QElapsedTimer timer;

    bool fromEnvironment;
    static QBasicMutex mutex;
};

QBasicMutex QMessagePattern::mutex;

QMessagePattern::QMessagePattern()
    : literals(0)
    , tokens(0)
    , fromEnvironment(false)
{
    timer.start();

    const QString envPattern = QString::fromLocal8Bit(qgetenv("QT_MESSAGE_PATTERN"));
    if (envPattern.isEmpty()) {
        setPattern(QLatin1String(defaultPattern));
    } else {
        setPattern(envPattern);
        fromEnvironment = true;
    }
}

QMessagePattern::~QMessagePattern()
{
    for (int i = 0; literals[i]; ++i)
        delete [] literals[i];
    delete [] literals;
    literals = 0;
    delete [] tokens;
    tokens = 0;
}

void QMessagePattern::setPattern(const QString &pattern)
{
    if (literals) {
        for (int i = 0; literals[i]; ++i)
            delete [] literals[i];
        delete [] literals;
    }
    delete [] tokens;
    timeArgs.clear();

    // scanner
    QList<QString> lexemes;
    QString lexeme;
    bool inPlaceholder = false;
    for (int i = 0; i < pattern.size(); ++i) {
        const QChar c = pattern.at(i);
        if ((c == QLatin1Char('%'))
                && !inPlaceholder) {
            if ((i + 1 < pattern.size())
                    && pattern.at(i + 1) == QLatin1Char('{')) {
                // beginning of placeholder
                if (!lexeme.isEmpty()) {
                    lexemes.append(lexeme);
                    lexeme.clear();
                }
                inPlaceholder = true;
            }
        }

        lexeme.append(c);

        if ((c == QLatin1Char('}') && inPlaceholder)) {
            // end of placeholder
            lexemes.append(lexeme);
            lexeme.clear();
            inPlaceholder = false;
        }
    }
    if (!lexeme.isEmpty())
        lexemes.append(lexeme);

    // tokenizer
    QVarLengthArray<const char*> literalsVar;
    tokens = new const char*[lexemes.size() + 1];
    tokens[lexemes.size()] = 0;

    bool nestedIfError = false;
    bool inIf = false;
    QString error;

    for (int i = 0; i < lexemes.size(); ++i) {
        const QString lexeme = lexemes.at(i);
        if (lexeme.startsWith(QLatin1String("%{"))
                && lexeme.endsWith(QLatin1Char('}'))) {
            // placeholder
            if (lexeme == QLatin1String(typeTokenC)) {
                tokens[i] = typeTokenC;
            } else if (lexeme == QLatin1String(categoryTokenC))
                tokens[i] = categoryTokenC;
            else if (lexeme == QLatin1String(messageTokenC))
                tokens[i] = messageTokenC;
            else if (lexeme == QLatin1String(fileTokenC))
                tokens[i] = fileTokenC;
            else if (lexeme == QLatin1String(lineTokenC))
                tokens[i] = lineTokenC;
            else if (lexeme == QLatin1String(functionTokenC))
                tokens[i] = functionTokenC;
            else if (lexeme == QLatin1String(pidTokenC))
                tokens[i] = pidTokenC;
            else if (lexeme == QLatin1String(appnameTokenC))
                tokens[i] = appnameTokenC;
            else if (lexeme == QLatin1String(threadidTokenC))
                tokens[i] = threadidTokenC;
            else if (lexeme == QLatin1String(qthreadptrTokenC))
                tokens[i] = qthreadptrTokenC;
            else if (lexeme.startsWith(QLatin1String(timeTokenC))) {
                tokens[i] = timeTokenC;
                int spaceIdx = lexeme.indexOf(QChar::fromLatin1(' '));
                if (spaceIdx > 0)
                    timeArgs.append(lexeme.mid(spaceIdx + 1, lexeme.length() - spaceIdx - 2));
                else
                    timeArgs.append(QString());
            } else if (lexeme.startsWith(QLatin1String(backtraceTokenC))) {
                error += QLatin1String("QT_MESSAGE_PATTERN: %{backtrace} is not supported by this Qt build\n");
                tokens[i] = "";
            }

#define IF_TOKEN(LEVEL) \
            else if (lexeme == QLatin1String(LEVEL)) { \
                if (inIf) \
                    nestedIfError = true; \
                tokens[i] = LEVEL; \
                inIf = true; \
            }
            IF_TOKEN(ifCategoryTokenC)
            IF_TOKEN(ifDebugTokenC)
            IF_TOKEN(ifInfoTokenC)
            IF_TOKEN(ifWarningTokenC)
            IF_TOKEN(ifCriticalTokenC)
            IF_TOKEN(ifFatalTokenC)
#undef IF_TOKEN
            else if (lexeme == QLatin1String(endifTokenC)) {
                tokens[i] = endifTokenC;
                if (!inIf && !nestedIfError)
                    error += QLatin1String("QT_MESSAGE_PATTERN: %{endif} without an %{if-*}\n");
                inIf = false;
            } else {
                tokens[i] = emptyTokenC;
                error += QStringLiteral("QT_MESSAGE_PATTERN: Unknown placeholder %1\n")
                        .arg(lexeme);
            }
        } else {
            char *literal = new char[lexeme.size() + 1];
            strncpy(literal, lexeme.toLatin1().constData(), lexeme.size());
            literal[lexeme.size()] = '\0';
            literalsVar.append(literal);
            tokens[i] = literal;
        }
    }
    if (nestedIfError)
        error += QLatin1String("QT_MESSAGE_PATTERN: %{if-*} cannot be nested\n");
    else if (inIf)
        error += QLatin1String("QT_MESSAGE_PATTERN: missing %{endif}\n");

    //if (!error.isEmpty())
    //    qt_message_print(error);

    literals = new const char*[literalsVar.size() + 1];
    literals[literalsVar.size()] = 0;
    memcpy(literals, literalsVar.constData(), literalsVar.size() * sizeof(const char*));
}

Q_GLOBAL_STATIC(QMessagePattern, qMessagePattern)

void qSetMessagePattern(const QString &pattern)
{
    QMutexLocker lock(&QMessagePattern::mutex);

    if (!qMessagePattern()->fromEnvironment)
        qMessagePattern()->setPattern(pattern);
}

QString qFormatLogMessage(QtMsgType type, const QString &str)
{
    QString message;

    QMutexLocker lock(&QMessagePattern::mutex);

    QMessagePattern *pattern = qMessagePattern();
    if (!pattern) {
        // after destruction of static QMessagePattern instance
        message.append(str);
        return message;
    }

    bool skip = false;
    int timeArgsIdx = 0;

    // we do not convert file, function, line literals to local encoding due to overhead
    for (int i = 0; pattern->tokens[i] != 0; ++i) {
        const char *token = pattern->tokens[i];
        if (token == endifTokenC) {
            skip = false;
        } else if (skip) {
            if (token == timeTokenC)
                timeArgsIdx++;
        } else if (token == messageTokenC) {
            message.append(str);
        } else if (token == categoryTokenC) {
            //message.append(QLatin1String(context.category));
        } else if (token == typeTokenC) {
            switch (type) {
            case QtDebugMsg:   message.append(QLatin1String("debug")); break;
            case QtInfoMsg:    message.append(QLatin1String("info")); break;
            case QtWarningMsg: message.append(QLatin1String("warning")); break;
            case QtCriticalMsg:message.append(QLatin1String("critical")); break;
            case QtFatalMsg:   message.append(QLatin1String("fatal")); break;
            }
        } else if (token == fileTokenC) {
#if 0
            if (context.file)
                message.append(QLatin1String(context.file));
            else
                message.append(QLatin1String("unknown"));
#endif
        } else if (token == lineTokenC) {
            //message.append(QString::number(context.line));
        } else if (token == functionTokenC) {
#if 0
            if (context.function)
                message.append(QString::fromLatin1(qCleanupFuncinfo(context.function)));
            else
                message.append(QLatin1String("unknown"));
#endif
        } else if (token == pidTokenC) {
            message.append(QString::number(QCoreApplication::applicationPid()));
        } else if (token == appnameTokenC) {
            message.append(QCoreApplication::applicationName());
        } else if (token == threadidTokenC) {
            // print the TID as decimal
            message.append(QString::number(qt_gettid()));
        } else if (token == qthreadptrTokenC) {
            message.append(QLatin1String("0x"));
            message.append(QString::number(qlonglong(QThread::currentThread()->currentThread()), 16));
        } else if (token == timeTokenC) {
            QString timeFormat = pattern->timeArgs.at(timeArgsIdx);
            timeArgsIdx++;
            if (timeFormat == QLatin1String("process")) {
                    quint64 ms = pattern->timer.elapsed();
                    message.append(QString::asprintf("%6d.%03d", uint(ms / 1000), uint(ms % 1000)));
            } else if (timeFormat ==  QLatin1String("boot")) {
                QElapsedTimer now;
                now.start();
                uint ms = now.msecsSinceReference();
                message.append(QString::asprintf("%6d.%03d", uint(ms / 1000), uint(ms % 1000)));
            } else if (timeFormat.isEmpty()) {
                    message.append(QDateTime::currentDateTime().toString(Qt::ISODate));
            } else {
                message.append(QDateTime::currentDateTime().toString(timeFormat));
            }
        } else if (token == ifCategoryTokenC) {
#if 0
            if (isDefaultCategory(context.category))
                skip = true;
#endif
#define HANDLE_IF_TOKEN(LEVEL)  \
        } else if (token == if##LEVEL##TokenC) { \
            skip = type != Qt##LEVEL##Msg;
        HANDLE_IF_TOKEN(Debug)
        HANDLE_IF_TOKEN(Info)
        HANDLE_IF_TOKEN(Warning)
        HANDLE_IF_TOKEN(Critical)
        HANDLE_IF_TOKEN(Fatal)
#undef HANDLE_IF_TOKEN
        } else {
            message.append(QLatin1String(token));
        }
    }
    return message;
}
