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
#include "Conditions.h"

Condition::Condition(int column) : mColumn(column) {
}

int Condition::column() const {
    return mColumn;
}

ExactCondition::ExactCondition(int column, const QString& expected)
        : Condition(column), mExpected(expected) {
}

bool ExactCondition::eval(const QString& value) const {
    return value == mExpected;
}

ContainsCondition::ContainsCondition(int column, const QString& expected)
        : Condition(column), mExpected(expected) {
}

bool ContainsCondition::eval(const QString& value) const {
    return value.contains(mExpected);
}

RegExCondition::RegExCondition(int column, const QRegularExpression& regEx)
        : Condition(column), mRegEx(regEx) {
    mRegEx.optimize();
}

bool RegExCondition::eval(const QString& value) const {
    return mRegEx.match(value).hasMatch();
}
