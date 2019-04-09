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

#include "logformat.h"
#include "logformatio.h"
#include "ui_logformatdialog.h"

#include <QPushButton>
#include <QStandardPaths>

using std::shared_ptr;

//- LogFormatModel -----------------------------
LogFormatModel::LogFormatModel(QObject* parent) : QFileSystemModel(parent) {
}

LogFormatModel::~LogFormatModel() {
}

QVariant LogFormatModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::DecorationRole) {
        return {};
    }
    if (role == Qt::DisplayRole) {
        return nameForIndex(index);
    }
    return {};
}

shared_ptr<LogFormat> LogFormatModel::logFormatForIndex(const QModelIndex& index) const {
    QString name = nameForIndex(index);
    LogFormatIO loader;
    loader.load(name);
    return loader.logFormat();
}

QString LogFormatModel::nameForIndex(const QModelIndex& index) const {
    QVariant value = QFileSystemModel::data(index, Qt::DisplayRole);
    QString name = value.toString();
    return QFileInfo(name).baseName();
}

//- LogFormatDialog ----------------------------
LogFormatDialog::LogFormatDialog(const QString& logFormatPath, QWidget* parent)
        : QDialog(parent)
        , ui(std::make_unique<Ui::LogFormatDialog>())
        , mModel(std::make_unique<LogFormatModel>())
        , mInitialLogFormatPath(logFormatPath) {
    ui->setupUi(this);
    setupSideBar();
    setupEditor();
}

LogFormatDialog::~LogFormatDialog() {
}

void LogFormatDialog::setupSideBar() {
    mModel->setFilter(QDir::Files);
    mModel->sort(0);
    connect(
        mModel.get(), &QAbstractItemModel::rowsInserted, this, &LogFormatDialog::onRowsInserted);
    ui->listView->setModel(mModel.get());
    QString dirPath = LogFormatIO::logFormatsDirPath();
    ui->listView->setRootIndex(mModel->setRootPath(dirPath));
    connect(ui->listView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &LogFormatDialog::onCurrentChanged);
    connect(
        ui->listView, &QAbstractItemView::doubleClicked, this, [this](const QModelIndex& index) {
            if (index.isValid()) {
                accept();
            }
        });
}

void LogFormatDialog::setupEditor() {
    ui->containerWidget->layout()->setMargin(0);

    connect(ui->parserLineEdit, &QLineEdit::editingFinished, this, &LogFormatDialog::applyChanges);

    // Do not close the dialog when the user presses Enter
    ui->buttonBox->button(QDialogButtonBox::Close)->setAutoDefault(false);
}

QString LogFormatDialog::logFormatName() const {
    auto index = ui->listView->currentIndex();
    if (!index.isValid()) {
        return {};
    }
    return index.data().toString();
}

void LogFormatDialog::onCurrentChanged(const QModelIndex& index) {
    if (!index.isValid()) {
        return;
    }

    shared_ptr<LogFormat> logFormat = mModel->logFormatForIndex(index);
    ui->parserLineEdit->setText(logFormat->parser.pattern());

    logFormatChanged(logFormat);
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

void LogFormatDialog::applyChanges() {
    auto index = ui->listView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    shared_ptr<LogFormat> logFormat = mModel->logFormatForIndex(index);
    logFormat->parser.setPattern(ui->parserLineEdit->text());
    // TODO: serialize
    logFormatChanged(logFormat);
}
