#ifndef LINEPROVIDER_H
#define LINEPROVIDER_H

#include <QObject>

class LineProvider : public QObject {
    Q_OBJECT
public:
    explicit LineProvider(QObject* parent = nullptr);
    virtual const QString& lineAt(int row) const = 0;
    virtual int lineCount() const = 0;

signals:
    void lineCountChanged(int newCount, int oldCount);
};


#endif // LINEPROVIDER_H
