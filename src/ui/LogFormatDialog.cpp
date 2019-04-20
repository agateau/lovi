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
#include "LogFormatDialog.h"

#include "ConditionIO.h"
#include "HighlightModel.h"
#include "LineEditChecker.h"
#include "LogFormat.h"
#include "LogFormatIO.h"
#include "LogFormatModel.h"
#include "LogFormatStore.h"
#include "WidgetFloater.h"
#include "ui_LogFormatDialog.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>

LogFormatDialog::LogFormatDialog(LogFormatStore* store,
                                 LogFormat* currentLogFormat,
                                 QWidget* parent)
        : QDialog(parent)
        , ui(std::make_unique<Ui::LogFormatDialog>())
        , mModel(std::make_unique<LogFormatModel>(store))
        , mHighlightModel(std::make_unique<HighlightModel>())
        , mLogFormatStore(store) {
    Q_ASSERT(store);
    Q_ASSERT(currentLogFormat);
    ui->setupUi(this);
    setupSideBar(currentLogFormat);
    setupEditor();
    onCurrentChanged(ui->logFormatComboBox->currentIndex());
}

LogFormatDialog::~LogFormatDialog() {
}

void LogFormatDialog::setupSideBar(LogFormat* currentLogFormat) {
    ui->logFormatComboBox->setModel(mModel.get());

    if (!currentLogFormat->name().isEmpty()) {
        selectLogFormat(currentLogFormat->name());
    }

    connect(ui->logFormatComboBox,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this,
            &LogFormatDialog::onCurrentChanged);
    connect(ui->addFormatButton, &QToolButton::clicked, this, &LogFormatDialog::onAddFormatClicked);
}

void LogFormatDialog::setupEditor() {
    // Parser edit
    connect(ui->parserLineEdit, &QLineEdit::editingFinished, this, &LogFormatDialog::applyChanges);
    new LineEditChecker(ui->parserLineEdit, [](const QString& text) -> QString {
        QRegularExpression rx(text);
        return rx.isValid() ? QString() : rx.errorString();
    });

    // Highlight list
    ui->highlightListView->setModel(mHighlightModel.get());

    connect(ui->highlightListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &LogFormatDialog::onCurrentHighlightChanged);

    // Highlight list context menu
    auto removeHighlightAction = new QAction(tr("Remove Highlight"));
    connect(removeHighlightAction, &QAction::triggered, this, [this] {
        auto index = ui->highlightListView->currentIndex();
        if (!index.isValid()) {
            return;
        }
        mHighlightModel->logFormat()->removeHighlightAt(index.row());
    });
    ui->highlightListView->addAction(removeHighlightAction);
    ui->highlightListView->setContextMenuPolicy(Qt::ActionsContextMenu);

    // Highlight add button
    auto addHighlightButton = new QToolButton;
    addHighlightButton->setIcon(QIcon::fromTheme("list-add"));
    connect(addHighlightButton, &QToolButton::pressed, this, [this] {
        mHighlightModel->logFormat()->addHighlight();
    });

    auto floater = new WidgetFloater(ui->highlightListView);
    floater->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    floater->setChildWidget(addHighlightButton);
}

void LogFormatDialog::onCurrentChanged(int row) {
    QModelIndex index = mModel->index(row);
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
    ui->highlightWidget->setHighlight(logFormat->editableHighlightAt(row));
}

void LogFormatDialog::applyChanges() {
    int row = ui->logFormatComboBox->currentIndex();
    QModelIndex index = mModel->index(row, 0);
    if (!index.isValid()) {
        return;
    }
    LogFormat* logFormat = mModel->logFormatForIndex(index);
    logFormat->setParserPattern(ui->parserLineEdit->text());
}

void LogFormatDialog::onAddFormatClicked() {
    QString name = QInputDialog::getText(
        this, tr("Log format name"), tr("Enter a name for the new log format"));
    if (name.isEmpty()) {
        return;
    }
    auto error = mLogFormatStore->addLogFormat(name);
    if (error.has_value()) {
        QString message = error.value();
        QMessageBox box(this);
        box.setIcon(QMessageBox::Warning);
        box.setText(tr("Could not add format."));
        box.setInformativeText(message);
        box.exec();
        return;
    }
    selectLogFormat(name);
}

void LogFormatDialog::selectLogFormat(const QString& name) {
    for (int row = 0; row < ui->logFormatComboBox->count(); ++row) {
        if (ui->logFormatComboBox->itemText(row) == name) {
            ui->logFormatComboBox->setCurrentIndex(row);
            return;
        }
    }
}
