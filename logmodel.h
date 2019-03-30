#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractTableModel>
#include <QColor>

#include <memory>

class LogFormat;
class LineProvider;

struct LogCell {
    QString text;
    QColor bgColor;
    QColor fgColor;
};

struct LogLine {
    std::vector<LogCell> cells;

    bool isValid() const {
        return !cells.empty();
    }
};

class LogModel : public QAbstractTableModel {
public:
    LogModel(const LineProvider* lineProvider, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;

    int columnCount(const QModelIndex& parent = {}) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QStringList columns() const;

    void setLogFormat(const LogFormat* logFormat);

private:
    std::unique_ptr<LogFormat> mEmptyLogFormat;
    const LogFormat* mLogFormat = nullptr;
    const LineProvider* mLineProvider = nullptr;
    QStringList mColumns;
    mutable QHash<int, LogLine> mLogLineCache;

    LogLine processLine(const QString& line) const;

    void applyHighlights(LogCell* logCell, int column) const;

    void onLineCountChanged(int newCount, int oldCount);
};


#endif // LOGMODEL_H
