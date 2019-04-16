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
#include "LogFormatModel.h"

#include "LogFormat.h"
#include "LogFormatStore.h"

LogFormatModel::LogFormatModel(LogFormatStore* store, QObject* parent)
        : QAbstractListModel(parent), mStore(store) {
    connect(mStore, &LogFormatStore::logFormatAdded, this, &LogFormatModel::onLogFormatAdded);
}

LogFormatModel::~LogFormatModel() {
}

int LogFormatModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : mStore->count();
}

QVariant LogFormatModel::data(const QModelIndex& index, int role) const {
    int row = index.row();
    if (row < 0 || row >= mStore->count()) {
        return {};
    }
    if (role == Qt::DisplayRole) {
        return mStore->nameAt(row);
    }
    return {};
}

LogFormat* LogFormatModel::logFormatForIndex(const QModelIndex& index) const {
    return mStore->at(index.row());
}

void LogFormatModel::onLogFormatAdded() {
    int row = mStore->count() - 1;
    beginInsertRows({}, row, row);
    endInsertRows();
}
