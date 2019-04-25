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
#include "MainWindow.h"

#include "Config.h"
#include "LineProvider.h"
#include "LogFormat.h"
#include "LogFormatDialog.h"
#include "LogFormatStore.h"
#include "LogModel.h"
#include "MainController.h"
#include "SearchBar.h"
#include "ui_MainWindow.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QTimer>
#include <QToolBar>
#include <QTreeView>
#include <QVBoxLayout>

MainWindow::MainWindow(Config* config, LogFormatStore* store, QWidget* parent)
        : QMainWindow(parent)
        , ui(std::make_unique<Ui::MainWindow>())
        , mController(std::make_unique<MainController>(config, store))
        , mOpenLogAction(new QAction(this))
        , mSelectLogFormatAction(new QAction(this))
        , mAutoScrollAction(new QAction(this))
        , mCopyLinesAction(new QAction(this))
        , mRecentFilesMenu(new QMenu(this))
        , mToolBar(addToolBar(tr("Toolbar"))) {
    ui->setupUi(this);
    setupActions();
    setupUi();
}

MainWindow::~MainWindow() {
}

void MainWindow::setLogFormat(LogFormat* logFormat) {
    mController->setLogFormat(logFormat);
}

void MainWindow::loadLog(const QString& filePath) {
    mController->loadLog(filePath);

    QString titlePath = mController->isStdin() ? "<stdin>" : filePath;
    setWindowTitle(QString("%1 - Lovi").arg(titlePath));

    auto model = mController->logModel();
    connect(model, &QAbstractItemModel::rowsInserted, this, &MainWindow::onRowsInserted);
    ui->treeView->setModel(model);

    // Must be done here because the selectionModel is (re)set by setModel()
    connect(ui->treeView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &MainWindow::onSelectionChanged);
    onSelectionChanged();
}

void MainWindow::setupUi() {
    auto resetMargins = [this] {
        auto* layout = centralWidget()->layout();
        layout->setMargin(0);
        layout->setSpacing(0);
    };

    connect(mController.get(), &MainController::currentRowChanged, this, [this](int row) {
        if (row < 0) {
            return;
        }
        auto index = ui->treeView->model()->index(row, 0);
        Q_ASSERT(index.isValid());
        ui->treeView->setCurrentIndex(index);
    });

    ui->treeView->setRootIsDecorated(false);
    ui->treeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeView->addAction(mCopyLinesAction);
    // Make scrolling fast
    ui->treeView->setUniformRowHeights(true);

    ui->searchBar->init(mController.get());

    resetMargins();

    mToolBar->addAction(mOpenLogAction);
    mToolBar->addAction(mSelectLogFormatAction);
    mToolBar->addAction(mAutoScrollAction);

    mToolBar->setMovable(false);
    mToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    resize(800, 600);
}

static void appendShortcutToToolTip(QAction* action) {
    auto shortcut = action->shortcut().toString(QKeySequence::NativeText);
    action->setToolTip(QString("%1 (%2)").arg(action->toolTip(), shortcut));
}

void MainWindow::setupActions() {
    mOpenLogAction->setText(tr("Open"));
    mOpenLogAction->setToolTip(tr("Open log file"));
    mOpenLogAction->setIcon(QIcon::fromTheme("document-open"));
    mOpenLogAction->setShortcut(QKeySequence::Open);
    mOpenLogAction->setMenu(mRecentFilesMenu);
    connect(mOpenLogAction, &QAction::triggered, this, &MainWindow::showOpenLogDialog);
    connect(mRecentFilesMenu, &QMenu::aboutToShow, this, &MainWindow::fillRecentFilesMenu);

    mSelectLogFormatAction->setText(tr("Format"));
    mSelectLogFormatAction->setToolTip(tr("Select log format"));
    mSelectLogFormatAction->setShortcut(Qt::SHIFT | Qt::Key_F);
    mSelectLogFormatAction->setIcon(QIcon::fromTheme("object-columns"));
    connect(mSelectLogFormatAction, &QAction::triggered, this, &MainWindow::showLogFormatDialog);

    mAutoScrollAction->setText(tr("Auto scroll"));
    mAutoScrollAction->setShortcut(Qt::SHIFT | Qt::Key_S);
    mAutoScrollAction->setToolTip(tr("Automatically scroll down when new lines are logged"));
    mAutoScrollAction->setIcon(QIcon::fromTheme("go-bottom"));
    mAutoScrollAction->setCheckable(true);
    connect(mAutoScrollAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled) {
            ui->treeView->scrollToBottom();
        }
    });

    for (auto action : {mOpenLogAction, mSelectLogFormatAction, mAutoScrollAction}) {
        appendShortcutToToolTip(action);
    }

    mCopyLinesAction->setText(tr("Copy"));
    mCopyLinesAction->setShortcut(QKeySequence::Copy);
    mCopyLinesAction->setEnabled(false);
    connect(mCopyLinesAction, &QAction::triggered, this, &MainWindow::copySelectedLines);
}

void MainWindow::onRowsInserted() {
    if (mAutoScrollAction->isChecked()) {
        // Delay the call a bit to ensure the view has created the rows
        QTimer::singleShot(0, this, [this] { ui->treeView->scrollToBottom(); });
    }
}

void MainWindow::onSelectionChanged() {
    auto selectionModel = ui->treeView->selectionModel();
    int lineCount = selectionModel->selectedRows().count();
    mCopyLinesAction->setEnabled(lineCount > 0);
    mCopyLinesAction->setText(tr("Copy %n line(s)", "", lineCount));

    auto index = selectionModel->currentIndex();
    mController->setCurrentRow(index.isValid() ? index.row() : -1);
}

void MainWindow::showOpenLogDialog() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilters({tr("Log files (*.log *.log.* *.txt)"), tr("All files (*)")});
    dialog.setWindowTitle(tr("Open log file"));
    QString logPath = mController->logPath();
    if (!logPath.isEmpty()) {
        QString logDir = QFileInfo(logPath).absolutePath();
        dialog.setDirectory(logDir);
    }
    if (!dialog.exec()) {
        return;
    }
    loadLog(dialog.selectedFiles().first());
}

void MainWindow::showLogFormatDialog() {
    if (mLogFormatDialog) {
        mLogFormatDialog->setLogFormat(mController->logFormat());
        mLogFormatDialog->show();
        mLogFormatDialog->activateWindow();
        return;
    }
    mLogFormatDialog =
        new LogFormatDialog(mController->logFormatStore(), mController->logFormat(), this);
    connect(mLogFormatDialog.data(),
            &LogFormatDialog::logFormatChanged,
            mController.get(),
            &MainController::setLogFormat);
    connect(mController.get(),
            &MainController::logFormatChanged,
            mLogFormatDialog.data(),
            &LogFormatDialog::setLogFormat);
    mLogFormatDialog->show();
}

void MainWindow::copySelectedLines() {
    auto selectedRows = ui->treeView->selectionModel()->selectedRows();
    if (selectedRows.empty()) {
        return;
    }
    QStringList list;
    auto lineProvider = mController->lineProvider();
    for (const auto& index : selectedRows) {
        list << lineProvider->lineAt(index.row()).toString();
    }
    qApp->clipboard()->setText(list.join("\n"));
}

void MainWindow::fillRecentFilesMenu() {
    mRecentFilesMenu->clear();
    for (const auto& filePath : mController->config()->recentLogFiles()) {
        mRecentFilesMenu->addAction(filePath, this, [this, filePath] { loadLog(filePath); });
    }
}
