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
#ifndef LOGFORMAT_H
#define LOGFORMAT_H

#include "Filter.h"
#include "Highlight.h"
#include "Vector.h"

#include <QObject>
#include <QRegularExpression>

#include <memory>

using ColumnHash = QHash<QString, int>;

class LogFormat : public QObject {
    Q_OBJECT
public:
    LogFormat(QObject* parent = nullptr);

    void emitHighlightChanged(Highlight* highlight);

    void setName(const QString& name);
    QString name() const;

    void setParserPattern(const QString& pattern);
    QString parserPattern() const;

    const QRegularExpression& parser() const;
    ColumnHash columnHash() const;

    const stdq::Vector<std::unique_ptr<Highlight>>& highlights() const;
    Highlight* addHighlight();
    Highlight* editableHighlightAt(int row);
    void removeHighlightAt(int row);

    const stdq::Vector<std::unique_ptr<Filter>>& filters() const;
    Filter* addFilter();
    Filter* editableFilterAt(int row);
    void removeFilterAt(int row);

    static std::unique_ptr<LogFormat> createEmpty();

signals:
    void highlightChanged(int row);
    void highlightAdded();
    void highlightRemoved(int row);

    void filterChanged(int row);
    void filterAdded();
    void filterRemoved(int row);

    void changed();

private:
    QString mName;
    QRegularExpression mParser;
    ColumnHash mColumnHash;
    stdq::Vector<std::unique_ptr<Highlight>> mHighlights;
    stdq::Vector<std::unique_ptr<Filter>> mFilters;
};

#endif // LOGFORMAT_H
