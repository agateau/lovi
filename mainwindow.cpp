#include "mainwindow.h"

#include "filelineprovider.h"
#include "logformat.h"
#include "logformatloader.h"
#include "logmodel.h"

#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QToolBar>
#include <QTreeView>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mLogFormatLoader(std::make_unique<LogFormatLoader>())
    , mOpenLogAction(new QAction(this))
    , mOpenLogFormatAction(new QAction(this))
    , mAutoScrollAction(new QAction(this))
    , mToolBar(addToolBar(tr("Toolbar")))
    , mTreeView(new QTreeView(this)) {
    setupUi();
    setupActions();
}

MainWindow::~MainWindow() {
}

void MainWindow::loadLogFormat(const QString& filePath) {
    mLogFormatLoader->load(filePath);
}

void MainWindow::loadLog(const QString &filePath) {
    auto fileLineProvider = std::make_unique<FileLineProvider>();
    fileLineProvider->setFilePath(filePath);
    mLineProvider = std::move(fileLineProvider);

    mLogModel = std::make_unique<LogModel>(mLineProvider.get());
    mLogModel->setLogFormat(mLogFormatLoader->logFormat());
    connect(mLogModel.get(), &QAbstractItemModel::rowsInserted, this, &MainWindow::onRowsInserted);

    connect(mLogFormatLoader.get(), &LogFormatLoader::logFormatChanged, mLogModel.get(), &LogModel::setLogFormat);

    mTreeView->setModel(mLogModel.get());
}

void MainWindow::setupUi() {
    mTreeView->setRootIsDecorated(false);
    setCentralWidget(mTreeView);
}

void MainWindow::setupActions() {
    mOpenLogAction->setText(tr("Open log"));
    mOpenLogAction->setIcon(QIcon::fromTheme("document-open"));
    mOpenLogAction->setShortcut(QKeySequence::Open);
    connect(mOpenLogAction, &QAction::triggered, this, &MainWindow::showOpenLogDialog);

    mOpenLogFormatAction->setText(tr("Open log format"));
    mOpenLogFormatAction->setIcon(QIcon::fromTheme("document-open"));
    connect(mOpenLogFormatAction, &QAction::triggered, this, &MainWindow::showOpenLogFormatDialog);

    mAutoScrollAction->setText(tr("Auto Scroll"));
    mAutoScrollAction->setIcon(QIcon::fromTheme("go-bottom"));
    mAutoScrollAction->setCheckable(true);
    connect(mAutoScrollAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled) {
            mTreeView->scrollToBottom();
        }
    });

    mToolBar->addAction(mOpenLogAction);
    mToolBar->addAction(mOpenLogFormatAction);
    mToolBar->addAction(mAutoScrollAction);
    mToolBar->setMovable(false);
    mToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

void MainWindow::onRowsInserted() {
    if (mAutoScrollAction->isChecked()) {
        mTreeView->scrollToBottom();
    }
}

void MainWindow::showOpenLogDialog() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilters({tr("Log files (*.log *.log.* *.txt)"),
                           tr("All files (*)")});
    dialog.setWindowTitle(tr("Open log file"));
    if (!dialog.exec()) {
        return;
    }
    loadLog(dialog.selectedFiles().first());
}

void MainWindow::showOpenLogFormatDialog() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Log format files (*.json)"));
    dialog.setWindowTitle(tr("Open log format file"));
    if (!dialog.exec()) {
        return;
    }
    loadLogFormat(dialog.selectedFiles().first());
}
