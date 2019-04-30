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
#include "LogFormatWidget.h"

#include "ConditionIO.h"
#include "HighlightModel.h"
#include "LineEditChecker.h"
#include "LogFormat.h"
#include "LogFormatIO.h"
#include "LogFormatModel.h"
#include "LogFormatStore.h"
#include "MainController.h"
#include "Searcher.h"
#include "WidgetFloater.h"
#include "ui_LogFormatWidget.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>

LogFormatWidget::LogFormatWidget(MainController* controller, QWidget* parent)
        : QWidget(parent)
        , mController(controller)
        , ui(std::make_unique<Ui::LogFormatWidget>())
        , mModel(std::make_unique<LogFormatModel>(controller->logFormatStore()))
        , mHighlightModel(std::make_unique<HighlightModel>()) {
    Q_ASSERT(mController);
    ui->setupUi(this);
    setupLogFormatSelector();
    setupEditor();
    setupSearchBar();
    setLogFormat(mController->logFormat());
}

LogFormatWidget::~LogFormatWidget() {
}

void LogFormatWidget::setLogFormat(LogFormat* logFormat) {
    Q_ASSERT(logFormat);
    selectLogFormat(logFormat->name());
    ui->parserLineEdit->setText(logFormat->parserPattern());
    mHighlightModel->setLogFormat(logFormat);
}

void LogFormatWidget::setupLogFormatSelector() {
    ui->logFormatComboBox->setModel(mModel.get());

    connect(ui->logFormatComboBox,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this,
            &LogFormatWidget::onCurrentChanged);
    connect(ui->addFormatButton, &QToolButton::clicked, this, &LogFormatWidget::onAddFormatClicked);

    connect(mController, &MainController::logFormatChanged, this, &LogFormatWidget::setLogFormat);
}

void LogFormatWidget::setupEditor() {
    // Parser edit
    connect(ui->parserLineEdit, &QLineEdit::editingFinished, this, &LogFormatWidget::applyChanges);
    new LineEditChecker(ui->parserLineEdit, [](const QString& text) -> QString {
        QRegularExpression rx(text);
        return rx.isValid() ? QString() : rx.errorString();
    });

    // Highlight list
    ui->highlightListView->setModel(mHighlightModel.get());

    connect(ui->highlightListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &LogFormatWidget::onCurrentHighlightChanged);

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

void LogFormatWidget::setupSearchBar() {
    ui->searchBar->layout()->setMargin(0);
    connect(mController->searcher(), &Searcher::finished, this, &LogFormatWidget::onSearchFinished);
    connect(ui->searchNextButton, &QToolButton::clicked, this, [this] {
        mController->startSearch(SearchDirection::Down);
    });
    connect(ui->searchPreviousButton, &QToolButton::clicked, this, [this] {
        mController->startSearch(SearchDirection::Up);
    });

    ui->searchNextButton->setShortcut(QKeySequence::Find);
}

void LogFormatWidget::onCurrentChanged(int row) {
    QModelIndex index = mModel->index(row);
    if (!index.isValid()) {
        return;
    }

    LogFormat* logFormat = mModel->logFormatForIndex(index);
    mController->setLogFormat(logFormat);
}

void LogFormatWidget::onCurrentHighlightChanged(const QModelIndex& index) {
    auto logFormat = mHighlightModel->logFormat();
    auto* highlight = index.isValid() ? logFormat->editableHighlightAt(index.row()) : nullptr;
    mController->setCurrentHighlight(highlight);
    ui->highlightWidget->setHighlight(highlight);
}

void LogFormatWidget::applyChanges() {
    int row = ui->logFormatComboBox->currentIndex();
    QModelIndex index = mModel->index(row, 0);
    if (!index.isValid()) {
        return;
    }
    LogFormat* logFormat = mModel->logFormatForIndex(index);
    logFormat->setParserPattern(ui->parserLineEdit->text());
}

void LogFormatWidget::onAddFormatClicked() {
    QString name = QInputDialog::getText(
        this, tr("Log format name"), tr("Enter a name for the new log format"));
    if (name.isEmpty()) {
        return;
    }
    auto error = mController->logFormatStore()->addLogFormat(name);
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

void LogFormatWidget::selectLogFormat(const QString& name) {
    for (int row = 0; row < ui->logFormatComboBox->count(); ++row) {
        if (ui->logFormatComboBox->itemText(row) == name) {
            ui->logFormatComboBox->setCurrentIndex(row);
            return;
        }
    }
}

void LogFormatWidget::onSearchFinished(const SearchResponse& response) {
    QString text;
    switch (response.matchType) {
    case SearchMatchType::Direct:
        ui->searchResultLabel->hide();
        return;
    case SearchMatchType::None:
        text = tr("No match found");
        break;
    case SearchMatchType::HitTop:
        text = tr("Hit top, continuing at bottom");
        break;
    case SearchMatchType::HitBottom:
        text = tr("Hit bottom, continuing at top");
        break;
    }
    ui->searchResultLabel->show();
    ui->searchResultLabel->setText(text);
}
