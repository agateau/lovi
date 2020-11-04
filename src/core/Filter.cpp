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
#include "Filter.h"

#include "ConditionIO.h"
#include "LogFormat.h"

using std::unique_ptr;

Filter::~Filter() = default;

Filter::Filter(LogFormat* logFormat) : mLogFormat(logFormat) {
}

void Filter::setConditionDefinition(const QString& definition) {
    mConditionDefinition = definition;
    updateCondition();
}

QString Filter::conditionDefinition() const {
    return mConditionDefinition;
}

void Filter::updateCondition() {
    using ConditionPtr = unique_ptr<Condition>;
    auto condition = ConditionIO::parse(mConditionDefinition, mLogFormat->columnHash());
    mCondition = std::holds_alternative<ConditionPtr>(condition)
                     ? std::move(std::get<ConditionPtr>(condition))
                     : nullptr;
    mLogFormat->emitFilterChanged(this);
}
