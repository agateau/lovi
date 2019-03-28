#include "logmodel.h"

#include "config.h"

#include <QColor>
#include <QDebug>

LogModel::LogModel(const Config& config, const QStringList& lines)
    : mConfig(config)
    , mLines(lines) {
    mColumns = mConfig.parser.namedCaptureGroups();
    mColumns.removeFirst();
}

int LogModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return mLines.count();
}

int LogModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return mColumns.count();
}

QVariant LogModel::data(const QModelIndex& index, int role) const {
    int row = index.row();
    if (row < 0 || row >= mLines.count()) {
        return {};
    }
    auto it = mLogLineCache.find(row);
    LogLine logLine;
    if (it == mLogLineCache.end()) {
        QString line = mLines.at(row);
        logLine = processLine(line);
        mLogLineCache[row] = logLine;
    } else {
        logLine = it.value();
    }
    if (!logLine.isValid()) {
        QString line = mLines.at(row);
        qDebug() << "Line" << row + 1 << "does not match:" << line;
        return role == 0 ? QVariant(line) : QVariant();
    }
    switch (role) {
    case Qt::BackgroundColorRole:
        return logLine.bgColor.isValid() ? QVariant(logLine.bgColor) : QVariant();
    case Qt::DisplayRole:
        return logLine.cells.at(index.column());
    };
    return {};
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Vertical) {
        return {};
    }
    if (role == Qt::DisplayRole) {
        return mColumns.at(section);
    }
    return {};
}

QStringList LogModel::columns() const {
    return mColumns;
}

LogLine LogModel::processLine(const QString& line) const {
    auto match = mConfig.parser.match(line);
    if (!match.hasMatch()) {
        return {};
    }
    LogLine logLine;
    int count = mColumns.count();

    logLine.cells.reserve(count);
    for (int role = 0; role < count; ++role) {
        QString value = match.captured(role + 1);
        applyHighlights(&logLine, role, value);
        logLine.cells << value;
    }
    return logLine;
}

void LogModel::applyHighlights(LogLine* logLine, int column, const QString& value) const {
    for (const Highlight& highlight : mConfig.highlights) {
        if (highlight.condition->column() == column) {
            if (highlight.condition->eval(value)) {
                logLine->bgColor = highlight.bgColor;
                return;
            }
        }
    }
}
