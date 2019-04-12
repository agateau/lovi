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
#include "logformat.h"

#include "conditions.h"

#include <QDebug>

using std::unique_ptr;

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

LogFormat* LogFormat::getEmpty() {
    static auto instance = createEmpty();
    return instance.get();
}

unique_ptr<LogFormat> LogFormat::createEmpty() {
    unique_ptr<LogFormat> logFormat = std::make_unique<LogFormat>();
    logFormat->setParserPattern("(?<line>.*)");
    return logFormat;
}
