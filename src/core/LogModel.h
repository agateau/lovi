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
#ifndef LOGMODEL_H
#define LOGMODEL_H

#include "Searcher.h"

#include <QAbstractTableModel>
#include <QColor>
#include <QVector>

#include <memory>

class Condition;
class LogFormat;
class LineProvider;

struct LogCell {
    QString text;
    QColor bgColor;
    QColor fgColor;
};

struct LogLine {
    QColor bgColor;
    QColor fgColor;
    QVector<LogCell> cells;

    bool isValid() const {
        return !cells.empty();
    }
};

class LogModel : public QAbstractTableModel, public Searchable {
public:
    LogModel(const LineProvider* lineProvider, LogFormat* logFormat, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;

    int columnCount(const QModelIndex& parent = {}) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant
    headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QStringList columns() const;

    void setLogFormat(LogFormat* logFormat);

    LogFormat* logFormat() const;

    // Searchable interface
    int lineCount() const override;

    bool lineMatches(int row, const Condition* condition) const override;

private:
    LogLine processLine(const QStringRef& line) const;
    const LogLine& lineAt(int row) const;
    void applyHighlights(LogLine* logLine, LogCell* logCell, int column) const;
    void onLineCountChanged(int newCount, int oldCount);
    void resetAllState();
    void onLogFormatChanged();

    const LineProvider* const mLineProvider;

    LogFormat* mLogFormat = nullptr; // Never null

    QStringList mColumns;
    mutable QHash<int, LogLine> mLogLineCache;
};

#endif // LOGMODEL_H
