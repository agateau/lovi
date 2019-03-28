#include "logmodel.h"

#include "config.h"

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
    case BackgroundColorRole:
        return logLine.bgColor;
    default:
        Q_ASSERT(role < mColumns.count());
        return logLine.cells[role];
    };
}

QStringList LogModel::columns() const {
    return mColumns;
}

QHash<int, QByteArray> LogModel::roleNames() const {
    QHash<int, QByteArray> hash;
    int idx = 0;
    for (const auto& column : mColumns) {
        hash[idx++] = column.toUtf8();
    }
    return hash;
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
