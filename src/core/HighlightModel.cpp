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
#include "HighlightModel.h"

#include "LogFormat.h"

HighlightModel::HighlightModel(QObject* parent) : QAbstractListModel(parent) {
}

void HighlightModel::setLogFormat(LogFormat* logFormat) {
    if (mLogFormat == logFormat) {
        return;
    }
    beginResetModel();
    if (mLogFormat) {
        mLogFormat->disconnect(this);
    }
    mLogFormat = logFormat;
    if (mLogFormat) {
        connect(
            mLogFormat, &LogFormat::highlightChanged, this, &HighlightModel::onHighlightChanged);
        connect(mLogFormat, &LogFormat::highlightAdded, this, &HighlightModel::onHighlightAdded);
        connect(
            mLogFormat, &LogFormat::highlightRemoved, this, &HighlightModel::onHighlightRemoved);
    }
    endResetModel();
}

LogFormat* HighlightModel::logFormat() const {
    return mLogFormat;
}

int HighlightModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid() || !mLogFormat) {
        return 0;
    }
    return mLogFormat->highlights().size();
}

QVariant HighlightModel::data(const QModelIndex& index, int role) const {
    if (!mLogFormat) {
        return {};
    }
    int row = index.row();
    if (row < 0 || row >= mLogFormat->highlights().size()) {
        return {};
    }
    const Highlight* highlight = mLogFormat->highlights().at(row).get();
    if (role == Qt::DisplayRole) {
        QString definition = highlight->conditionDefinition();
        return definition.isEmpty() ? tr("<empty>") : definition;
    } else if (role == Qt::BackgroundRole) {
        if (highlight->bgColor().has_value()) {
            return highlight->bgColor()->toColor(highlight->conditionDefinition());
        }
    } else if (role == Qt::ForegroundRole) {
        if (highlight->fgColor().has_value()) {
            return highlight->fgColor()->toColor(highlight->conditionDefinition());
        }
    }
    return {};
}

void HighlightModel::onHighlightChanged(int row) {
    Q_ASSERT(row >= 0 && row < mLogFormat->highlights().size());
    auto idx = index(row, 0);
    dataChanged(idx, idx);
}

void HighlightModel::onHighlightAdded() {
    int row = mLogFormat->highlights().size() - 1;
    beginInsertRows({}, row, row);
    endInsertRows();
}

void HighlightModel::onHighlightRemoved(int row) {
    beginRemoveRows({}, row, row);
    endRemoveRows();
}
