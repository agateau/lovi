#ifndef FILELINEPROVIDER_H
#define FILELINEPROVIDER_H

#include "lineprovider.h"

class FileWatcher;

class FileLineProvider : public LineProvider {
    Q_OBJECT
public:
    explicit FileLineProvider(QObject* parent = nullptr);

    const QString& lineAt(int row) const override;
    int lineCount() const override;

    void setFilePath(const QString& filePath);

private:
    void readFile();

    FileWatcher* const mWatcher;
    QString mFilePath;
    QStringList mLines;
    qint64 mFileSize = 0;
};

#endif // FILELINEPROVIDER_H
