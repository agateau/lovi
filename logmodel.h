#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractListModel>

class Config;

class LogLine {
public:
    QString bgColor;
    QString fgColor;
    QStringList cells;

    bool isValid() const {
        return !cells.empty();
    }
};

class LogModel : public QAbstractListModel {
public:
    enum {
        BackgroundColorRole = Qt::UserRole
    };

    LogModel(const Config& config, const QStringList& lines);

    int rowCount(const QModelIndex& parent = {}) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QStringList columns() const;

    QHash<int, QByteArray> roleNames() const override;

private:
    const Config& mConfig;
    const QStringList mLines;
    QStringList mColumns;
    mutable QHash<int, LogLine> mLogLineCache;

    LogLine processLine(const QString& line) const;

    void applyHighlights(LogLine* logLine, int column, const QString& value) const;
};


#endif // LOGMODEL_H
