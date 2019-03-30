#ifndef LOGFORMATLOADER_H
#define LOGFORMATLOADER_H

#include <QObject>

#include <memory>

class FileWatcher;
class LogFormat;

class LogFormatLoader : public QObject {
    Q_OBJECT
public:
    explicit LogFormatLoader(QObject* parent = nullptr);
    ~LogFormatLoader();

    void load(const QString& filePath);

    LogFormat* logFormat() const;

signals:
    void logFormatChanged(LogFormat* logFormat);

private:
    void reload();

    const std::unique_ptr<FileWatcher> mWatcher;
    std::unique_ptr<LogFormat> mLogFormat;
};

#endif // LOGFORMATLOADER_H
