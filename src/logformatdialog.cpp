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

#include "conditionio.h"
#include "highlightmodel.h"
#include "logformat.h"
#include "logformatio.h"
#include "logformatstore.h"
#include "ui_logformatdialog.h"

#include <QPushButton>
#include <QStandardPaths>

using std::shared_ptr;

//- LogFormatModel -----------------------------
LogFormatModel::LogFormatModel(LogFormatStore* store, QObject* parent)
        : QAbstractListModel(parent), mStore(store) {
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

//- LogFormatDialog ----------------------------
LogFormatDialog::LogFormatDialog(LogFormatStore* store,
                                 LogFormat* currentLogFormat,
                                 QWidget* parent)
        : QDialog(parent)
        , ui(std::make_unique<Ui::LogFormatDialog>())
        , mModel(std::make_unique<LogFormatModel>(store))
        , mHighlightModel(std::make_unique<HighlightModel>()) {
    ui->setupUi(this);
    setupSideBar(currentLogFormat);
    setupEditor();
    onCurrentChanged(ui->listView->currentIndex());
}

LogFormatDialog::~LogFormatDialog() {
}

void LogFormatDialog::setupSideBar(LogFormat* currentLogFormat) {
    ui->listView->setModel(mModel.get());

    if (!currentLogFormat->name.isEmpty()) {
        for (int row = 0; row < ui->listView->model()->rowCount(); ++row) {
            auto index = ui->listView->model()->index(row, 0);
            if (index.data().toString() == currentLogFormat->name) {
                ui->listView->setCurrentIndex(index);
                break;
            }
        }
    }

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

    ui->highlightListView->setModel(mHighlightModel.get());

    connect(ui->highlightListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &LogFormatDialog::onCurrentHighlightChanged);

    connect(ui->parserLineEdit, &QLineEdit::editingFinished, this, &LogFormatDialog::applyChanges);

    connect(ui->highlightWidget,
            &HighlightWidget::highlightChanged,
            this,
            &LogFormatDialog::onHighlightEdited);

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

    LogFormat* logFormat = mModel->logFormatForIndex(index);
    ui->parserLineEdit->setText(logFormat->parserPattern());
    mHighlightModel->setLogFormat(logFormat);

    logFormatChanged(logFormat);
}

void LogFormatDialog::onCurrentHighlightChanged(const QModelIndex& index) {
    if (!index.isValid()) {
        ui->highlightWidget->setHighlight(nullptr);
        return;
    }
    int row = index.row();
    auto logFormat = mHighlightModel->logFormat();
    ui->highlightWidget->setHighlight(&logFormat->highlights.at(row));
}

void LogFormatDialog::onHighlightEdited() {
    auto index = ui->highlightListView->currentIndex();
    Q_ASSERT(index.isValid());
    mHighlightModel->notifyHighlightChanged(index);

    auto logFormat = mHighlightModel->logFormat();
    auto highlight = ui->highlightWidget->highlight();
    highlight->condition =
        ConditionIO::parse(highlight->conditionDefinition, logFormat->columnHash());

    LogFormatIO::save(logFormat);
    logFormatChanged(logFormat);
}

void LogFormatDialog::applyChanges() {
    auto index = ui->listView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    LogFormat* logFormat = mModel->logFormatForIndex(index);
    logFormat->setParserPattern(ui->parserLineEdit->text());
    LogFormatIO::save(logFormat);
    logFormatChanged(logFormat);
}
