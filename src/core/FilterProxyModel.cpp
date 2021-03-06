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
#include "FilterProxyModel.h"

#include "Filter.h"
#include "LogFormat.h"
#include "LogModel.h"

#include <algorithm>

FilterProxyModel::FilterProxyModel(LogModel* logModel, const LogFormat* logFormat)
        : mLogModel(logModel), mLogFormat(logFormat) {
    setSourceModel(logModel);
    connect(mLogFormat, &LogFormat::filterChanged, this, &FilterProxyModel::invalidateFilter);
    connect(mLogFormat, &LogFormat::filterAdded, this, &FilterProxyModel::invalidateFilter);
    connect(mLogFormat, &LogFormat::filterRemoved, this, &FilterProxyModel::invalidateFilter);
    connect(mLogFormat, &LogFormat::changed, this, &FilterProxyModel::invalidateFilter);
}

bool FilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& /*sourceParent*/) const {
    bool anyMatch = std::any_of(mLogFormat->filters().begin(),
                                mLogFormat->filters().end(),
                                [this, sourceRow](const auto& filter) {
                                    if (!filter->condition()) {
                                        return false;
                                    }
                                    return mLogModel->lineMatches(sourceRow, filter->condition());
                                });

    if (mLogFormat->filterMode() == FilterMode::ShowMatchingLines) {
        return anyMatch;
    } else {
        return !anyMatch;
    }
}
