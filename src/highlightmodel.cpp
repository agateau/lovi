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
#include "highlightmodel.h"

#include "logformat.h"

HighlightModel::HighlightModel(QObject* parent) : QAbstractListModel(parent) {
}

void HighlightModel::setLogFormat(LogFormat* logFormat) {
    if (mLogFormat == logFormat) {
        return;
    }
    beginResetModel();
    mLogFormat = logFormat;
    endResetModel();
}

LogFormat* HighlightModel::logFormat() const {
    return mLogFormat;
}

int HighlightModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid() || !mLogFormat) {
        return 0;
    }
    return mLogFormat->highlights.size();
}

QVariant HighlightModel::data(const QModelIndex& index, int role) const {
    if (!mLogFormat) {
        return {};
    }
    int row = index.row();
    if (row < 0 || row >= mLogFormat->highlights.size()) {
        return {};
    }
    const auto& highlight = mLogFormat->highlights.at(row);
    if (role == Qt::DisplayRole) {
        return highlight.conditionDefinition;
    } else if (role == Qt::BackgroundRole) {
        if (highlight.bgColor.has_value()) {
            return highlight.bgColor->toColor(highlight.conditionDefinition);
        }
    } else if (role == Qt::ForegroundRole) {
        if (highlight.fgColor.has_value()) {
            return highlight.fgColor->toColor(highlight.conditionDefinition);
        }
    }
    return {};
}

void HighlightModel::notifyHighlightChanged(const QModelIndex& index) {
    dataChanged(index, index);
}
