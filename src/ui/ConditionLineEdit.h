/*
 * Copyright 2020 Aurélien Gâteau <mail@agateau.com>
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
#ifndef CONDITIONLINEEDIT_H
#define CONDITIONLINEEDIT_H

#include <QLineEdit>

#include <memory>

class ConditionLineEditChecker;
class LogFormat;

/**
 * A QLineEdit which indicates errors if its content is not a valid LogFormat condition
 */
class ConditionLineEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit ConditionLineEdit(QWidget* parent = nullptr);
    ~ConditionLineEdit();
    void setLogFormat(const LogFormat* logFormat);

private:
    const std::unique_ptr<ConditionLineEditChecker> mLineEditChecker;
};

#endif // CONDITIONLINEEDIT_H
