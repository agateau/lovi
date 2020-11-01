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
#include "LogFormat.h"

#include "Conditions.h"

#include <QDebug>

using std::unique_ptr;

LogFormat::LogFormat(QObject* parent) : QObject(parent) {
}

void LogFormat::emitHighlightChanged(Highlight* highlight) {
    auto it = std::find_if(mHighlights.begin(), mHighlights.end(), [highlight](const auto& ptr) {
        return ptr.get() == highlight;
    });
    Q_ASSERT(it != mHighlights.end());

    int row = int(it - mHighlights.begin());
    highlightChanged(row);
    changed();
}

void LogFormat::setName(const QString& name) {
    mName = name;
}

QString LogFormat::name() const {
    return mName;
}

void LogFormat::setParserPattern(const QString& pattern) {
    mParser.setPattern(pattern);
    mParser.optimize();

    mColumnHash.clear();
    int role = 0;
    for (const auto& name : mParser.namedCaptureGroups()) {
        if (!name.isEmpty()) {
            mColumnHash[name] = role++;
        }
    }
    for (const auto& highlight : mHighlights) {
        highlight->updateCondition();
    }
    for (const auto& filter : mFilters) {
        filter->updateCondition();
    }

    changed();
}

QString LogFormat::parserPattern() const {
    return mParser.pattern();
}

const QRegularExpression& LogFormat::parser() const {
    return mParser;
}

ColumnHash LogFormat::columnHash() const {
    return mColumnHash;
}

const stdq::Vector<unique_ptr<Highlight>>& LogFormat::highlights() const {
    return mHighlights;
}

Highlight* LogFormat::addHighlight() {
    mHighlights.push_back(std::make_unique<Highlight>(this));
    highlightAdded();
    changed();
    return mHighlights.back().get();
}

Highlight* LogFormat::editableHighlightAt(int row) {
    return mHighlights[row].get();
}

void LogFormat::removeHighlightAt(int row) {
    Q_ASSERT(row >= 0 && row < mHighlights.size());
    mHighlights.erase(mHighlights.begin() + row);
    highlightRemoved(row);
    changed();
}

const stdq::Vector<std::unique_ptr<Filter>>& LogFormat::filters() const {
    return mFilters;
}

Filter* LogFormat::addFilter() {
    mFilters.push_back(std::make_unique<Filter>(this));
    filterAdded();
    changed();
    return mFilters.back().get();
}

Filter* LogFormat::editableFilterAt(int row) {
    return mFilters[row].get();
}

void LogFormat::removeFilterAt(int row) {
    Q_ASSERT(row >= 0 && row < mFilters.size());
    mFilters.erase(mFilters.begin() + row);
    filterRemoved(row);
    changed();
}

unique_ptr<LogFormat> LogFormat::createEmpty() {
    unique_ptr<LogFormat> logFormat = std::make_unique<LogFormat>();
    logFormat->setParserPattern("(?<line>.*)");
    return logFormat;
}
