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
#include "logformatdialog.h"

#include "logformatloader.h"
#include "ui_logformatdialog.h"

#include <QFileSystemModel>
#include <QStandardPaths>

/**
 * Override QFileSystemModel to hide the icon and the filename extension
 * Faster to write and use than using a proxy model
 */
class MyModel : public QFileSystemModel {
public:
    MyModel(QObject* parent = nullptr) : QFileSystemModel(parent) {
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (role == Qt::DecorationRole) {
            return {};
        }
        auto value = QFileSystemModel::data(index, role);
        if (role == Qt::DisplayRole) {
            QString name = value.toString();
            return QFileInfo(name).baseName();
        }
        return value;
    }
};

LogFormatDialog::LogFormatDialog(const QString& logFormatPath, QWidget* parent)
        : QDialog(parent)
        , ui(std::make_unique<Ui::LogFormatDialog>())
        , mModel(std::make_unique<MyModel>())
        , mInitialLogFormatPath(logFormatPath) {
    ui->setupUi(this);

    mModel->setFilter(QDir::Files);
    mModel->sort(0);
    connect(
        mModel.get(), &QAbstractItemModel::rowsInserted, this, &LogFormatDialog::onRowsInserted);
    ui->listView->setModel(mModel.get());
    QString dirPath = LogFormatLoader::logFormatsDirPath();
    ui->listView->setRootIndex(mModel->setRootPath(dirPath));
    connect(
        ui->listView, &QAbstractItemView::doubleClicked, this, [this](const QModelIndex& index) {
            if (index.isValid()) {
                accept();
            }
        });
}

LogFormatDialog::~LogFormatDialog() {
}

QString LogFormatDialog::logFormatName() const {
    auto index = ui->listView->currentIndex();
    if (!index.isValid()) {
        return {};
    }
    return index.data().toString();
}

void LogFormatDialog::onRowsInserted(const QModelIndex& parent, int first, int last) {
    auto selectInitialLogFormat = [this, parent, first, last]() {
        for (int row = first; row <= last; ++row) {
            auto index = mModel->index(row, 0, parent);
            if (index.data(QFileSystemModel::FilePathRole).toString() == mInitialLogFormatPath) {
                ui->listView->setCurrentIndex(index);
                mInitialLogFormatPath.clear();
                return;
            }
        }
    };

    if (!mInitialLogFormatPath.isEmpty()) {
        selectInitialLogFormat();
    }
}
