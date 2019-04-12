/*
 * Copyright 2019 Aurélien Gâteau <mail@agateau.com>
 *
 * This file is part of Lovi.
 *
 * Lovi is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "logmodel.h"

#include "lineprovider.h"
#include "logformat.h"

#include <QColor>
#include <QDebug>

using std::optional;

LogModel::LogModel(const LineProvider* lineProvider, QObject* parent)
        : QAbstractTableModel(parent), mLineProvider(lineProvider) {
    mEmptyLogFormat = LogFormat::getEmpty();
    setLogFormat(nullptr);
    connect(mLineProvider, &LineProvider::lineCountChanged, this, &LogModel::onLineCountChanged);
}

int LogModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return mLineProvider->lineCount();
}

int LogModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return mColumns.count();
}

QVariant LogModel::data(const QModelIndex& index, int role) const {
    int row = index.row();
    if (row < 0 || row >= mLineProvider->lineCount()) {
        return {};
    }
    auto it = mLogLineCache.find(row);
    LogLine logLine;
    if (it == mLogLineCache.end()) {
        const QString& line = mLineProvider->lineAt(row);
        logLine = processLine(line);
        mLogLineCache[row] = logLine;
        if (!logLine.isValid()) {
            qWarning() << "Line" << row + 1 << "does not match:" << line;
        }
    } else {
        logLine = it.value();
    }
    if (!logLine.isValid()) {
        const QString& line = mLineProvider->lineAt(row);
        return role == Qt::DisplayRole && index.column() == mColumns.count() - 1 ? QVariant(line)
                                                                                 : QVariant();
    }
    const auto& cell = logLine.cells.at(index.column());
    switch (role) {
    case Qt::BackgroundColorRole:
        return cell.bgColor.isValid()
                   ? QVariant(cell.bgColor)
                   : logLine.bgColor.isValid() ? QVariant(logLine.bgColor) : QVariant();
    case Qt::TextColorRole:
        return cell.fgColor.isValid()
                   ? QVariant(cell.fgColor)
                   : logLine.fgColor.isValid() ? QVariant(logLine.fgColor) : QVariant();
    case Qt::DisplayRole:
        return cell.text;
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

void LogModel::setLogFormat(LogFormat* logFormat) {
    beginResetModel();
    if (logFormat) {
        mLogFormat = logFormat;
    } else {
        mLogFormat = mEmptyLogFormat;
    }
    mColumns = mLogFormat->parser().namedCaptureGroups();
    mColumns.removeFirst();
    mLogLineCache.clear();
    endResetModel();
}

LogLine LogModel::processLine(const QString& line) const {
    auto match = mLogFormat->parser().match(line);
    if (!match.hasMatch()) {
        return {};
    }
    LogLine logLine;
    int count = mColumns.count();

    logLine.cells.resize(count);
    for (int column = 0; column < count; ++column) {
        LogCell& cell = logLine.cells[column];
        cell.text = match.captured(column + 1).trimmed();
        applyHighlights(&logLine, &cell, column);
    }
    return logLine;
}

static QColor getColor(const optional<HighlightColor>& color, const QString& text) {
    return color.has_value() ? color.value().toColor(text) : QColor();
}

void LogModel::applyHighlights(LogLine* line, LogCell* cell, int column) const {
    for (const Highlight& highlight : mLogFormat->highlights) {
        const auto& condition = highlight.condition;
        if (condition && condition->column() == column && condition->eval(cell->text)) {
            if (highlight.scope == Highlight::Row) {
                line->bgColor = getColor(highlight.bgColor, cell->text);
                line->fgColor = getColor(highlight.fgColor, cell->text);
            } else {
                cell->bgColor = getColor(highlight.bgColor, cell->text);
                cell->fgColor = getColor(highlight.fgColor, cell->text);
            }
        }
    }
}

void LogModel::onLineCountChanged(int newCount, int oldCount) {
    if (newCount <= oldCount) {
        // Full refresh
        beginResetModel();
        mLogLineCache.clear();
        endResetModel();
        return;
    }
    // Assume append
    beginInsertRows({}, oldCount, newCount - 1);
    endInsertRows();
}
