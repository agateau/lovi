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
#ifndef FILTER_H
#define FILTER_H

#include "Conditions.h"

#include <memory>

class LogFormat;

class Filter {
public:
    explicit Filter(LogFormat* logFormat);
    ~Filter();

    void setConditionDefinition(const QString& definition);
    QString conditionDefinition() const;
    void updateCondition();

    Condition* condition() const {
        return mCondition.get();
    }

private:
    LogFormat* const mLogFormat;
    QString mConditionDefinition;
    std::unique_ptr<Condition> mCondition;
};

#endif // FILTER_H
