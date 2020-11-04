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
#include "ConditionLineEditChecker.h"

#include "ConditionIO.h"

ConditionLineEditChecker::ConditionLineEditChecker(QLineEdit* lineEdit)
        : LineEditChecker(lineEdit,
                          [this](const QString& text) -> QString { return check(text); }) {
}

void ConditionLineEditChecker::setLogFormat(const LogFormat* logFormat) {
    mLogFormat = logFormat;
}

QString ConditionLineEditChecker::check(const QString& text) const {
    if (!mLogFormat) {
        return {};
    }
    auto condition = ConditionIO::parse(text, mLogFormat->columnHash());
    if (std::holds_alternative<ConditionIO::ParseError>(condition)) {
        return std::get<ConditionIO::ParseError>(condition);
    }
    return {};
}
