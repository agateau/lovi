#include "filewatcher.h"

#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QTimer>

#include <chrono>

using namespace std::chrono_literals;

static const std::chrono::duration DELAY_INTERVAL = 100ms;

FileWatcher::FileWatcher(QObject *parent)
    : QObject(parent)
    , mWatcher(new QFileSystemWatcher(this))
    , mTimer(new QTimer(this)) {

    connect(mWatcher, &QFileSystemWatcher::directoryChanged, this, &FileWatcher::onChangeDetected);
    connect(mWatcher, &QFileSystemWatcher::fileChanged, this, &FileWatcher::onChangeDetected);

    mTimer->setInterval(DELAY_INTERVAL);
    mTimer->setSingleShot(true);
    connect(mTimer, &QTimer::timeout, this, &FileWatcher::fileChanged);
}

void FileWatcher::setFilePath(const QString& path) {
    if (mPath == path) {
        return;
    }
    if (!mPath.isEmpty()) {
        mWatcher->removePath(mPath);
        mWatcher->removePath(QFileInfo(mPath).absolutePath());
    }
    mPath = path;
    QFileInfo info(mPath);
    mLastModified = info.lastModified();
    mWatcher->addPath(mPath);
    mWatcher->addPath(info.absolutePath());
}

void FileWatcher::onChangeDetected() {
    QFileInfo info(mPath);
    info.refresh();
    QDateTime lastModified = info.lastModified();
    if (mLastModified == lastModified) {
        // Another file in the dir changed
        return;
    }
    mLastModified = lastModified;
    mTimer->start();
}
