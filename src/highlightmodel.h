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
#ifndef HIGHLIGHTMODEL_H
#define HIGHLIGHTMODEL_H

#include <QAbstractItemModel>

#include <memory>

class LogFormat;

class HighlightModel : public QAbstractListModel {
    Q_OBJECT
public:
    HighlightModel(QObject* parent = nullptr);

    void setLogFormat(const std::shared_ptr<LogFormat>& logFormat);

    int rowCount(const QModelIndex& parent = {}) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
    std::shared_ptr<LogFormat> mLogFormat;
};

#endif // HIGHLIGHTMODEL_H
