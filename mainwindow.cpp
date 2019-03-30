#include "mainwindow.h"

#include "filelineprovider.h"
#include "logformat.h"
#include "logformatloader.h"
#include "logmodel.h"

#include <QAction>
#include <QDebug>
#include <QToolBar>
#include <QTreeView>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mLogFormatLoader(std::make_unique<LogFormatLoader>())
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
    mAutoScrollAction->setText(tr("Auto Scroll"));
    mAutoScrollAction->setIcon(QIcon::fromTheme("go-bottom"));
    mAutoScrollAction->setCheckable(true);
    connect(mAutoScrollAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled) {
            mTreeView->scrollToBottom();
        }
    });

    mToolBar->addAction(mAutoScrollAction);
    mToolBar->setMovable(false);
    mToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

void MainWindow::onRowsInserted() {
    if (mAutoScrollAction->isChecked()) {
        mTreeView->scrollToBottom();
    }
}
