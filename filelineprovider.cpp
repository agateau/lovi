#include "filelineprovider.h"

#include "filewatcher.h"

#include <QDebug>
#include <QFile>

FileLineProvider::FileLineProvider(QObject* parent)
    : LineProvider(parent)
    , mWatcher(new FileWatcher(this)) {
    connect(mWatcher, &FileWatcher::fileChanged, this, &FileLineProvider::readFile);
}

const QString& FileLineProvider::lineAt(int row) const {
    return mLines.at(row);
}

int FileLineProvider::lineCount() const {
    return mLines.length();
}

void FileLineProvider::setFilePath(const QString& filePath) {
    if (mFilePath == filePath) {
        return;
    }
    mFilePath = filePath;
    mWatcher->setFilePath(mFilePath);
    readFile();
}

void FileLineProvider::readFile() {
    QFile file(mFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << mFilePath << ":" << file.errorString();
        return;
    }
    int oldLineCount = lineCount();
    QByteArray data = file.readAll();
    mLines = QString::fromUtf8(data).split('\n');
    if (mLines.last().isEmpty()) {
        mLines.removeLast();
    }
    lineCountChanged(lineCount(), oldLineCount);
}
