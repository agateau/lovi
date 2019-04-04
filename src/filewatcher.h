#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QDateTime>
#include <QObject>

class QTimer;
class QFileSystemWatcher;

/**
 * A single file watcher, which also works when a file is deleted then recreated
 */
class FileWatcher : public QObject {
    Q_OBJECT
public:
    explicit FileWatcher(QObject* parent = nullptr);

    void setFilePath(const QString& path);
    QString filePath() const;

signals:
    void fileChanged();

private:
    void scheduleNotification();
    void onChangeDetected();

    QFileSystemWatcher* mWatcher = nullptr;
    QTimer* mTimer = nullptr;
    QString mPath;
    QDateTime mLastModified;
};

#endif // FILEWATCHER_H
