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
#include "FilterModel.h"

#include "LogFormat.h"

FilterModel::FilterModel(QObject* parent) : QAbstractListModel(parent) {
}

void FilterModel::setLogFormat(LogFormat* logFormat) {
    if (mLogFormat == logFormat) {
        return;
    }
    beginResetModel();
    if (mLogFormat) {
        mLogFormat->disconnect(this);
    }
    mLogFormat = logFormat;
    if (mLogFormat) {
        connect(mLogFormat, &LogFormat::filterChanged, this, &FilterModel::onFilterChanged);
        connect(mLogFormat, &LogFormat::filterAdded, this, &FilterModel::onFilterAdded);
        connect(mLogFormat, &LogFormat::filterRemoved, this, &FilterModel::onFilterRemoved);
    }
    endResetModel();
}

int FilterModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid() || !mLogFormat) {
        return 0;
    }
    return mLogFormat->filters().size();
}

QVariant FilterModel::data(const QModelIndex& index, int role) const {
    if (!mLogFormat) {
        return {};
    }
    int row = index.row();
    if (row < 0 || row >= mLogFormat->filters().size()) {
        return {};
    }
    const Filter* filter = mLogFormat->filters().at(row).get();
    if (role == Qt::DisplayRole) {
        QString definition = filter->conditionDefinition();
        return definition.isEmpty() ? tr("<empty>") : definition;
    }
    return {};
}

void FilterModel::onFilterChanged(int row) {
    Q_ASSERT(row >= 0 && row < mLogFormat->filters().size());
    auto idx = index(row, 0);
    dataChanged(idx, idx);
}

void FilterModel::onFilterAdded() {
    int row = mLogFormat->filters().size() - 1;
    beginInsertRows({}, row, row);
    endInsertRows();
}

void FilterModel::onFilterRemoved(int row) {
    beginRemoveRows({}, row, row);
    endRemoveRows();
}
