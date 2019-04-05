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
#ifndef CONDITIONS_H
#define CONDITIONS_H

#include <QRegularExpression>
#include <QString>

class Condition {
public:
    explicit Condition(int column);
    virtual ~Condition() = default;
    virtual bool eval(const QString& value) const = 0;

    int column() const;

private:
    const int mColumn;
};

class ExactCondition : public Condition {
public:
    explicit ExactCondition(int column, const QString& expected);

    bool eval(const QString& value) const override;

private:
    const QString mExpected;
};

class ContainsCondition : public Condition {
public:
    explicit ContainsCondition(int column, const QString& expected);

    bool eval(const QString& value) const override;

private:
    const QString mExpected;
};

class RegExCondition : public Condition {
public:
    explicit RegExCondition(int column, const QRegularExpression& regEx);

    bool eval(const QString& value) const override;

private:
    const QRegularExpression mRegEx;
};

#endif // CONDITIONS_H
