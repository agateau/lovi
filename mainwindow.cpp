#include "mainwindow.h"

#include "logformat.h"
#include "logformatloader.h"
#include "logmodel.h"

#include <QAction>
#include <QDebug>
#include <QToolBar>
#include <QTreeView>

MainWindow::MainWindow(LogModel* logModel, QWidget* parent)
    : QMainWindow(parent)
    , mLogFormatLoader(std::make_unique<LogFormatLoader>())
    , mModel(logModel) {

    createUi();
    createActions();

    connect(mModel, &QAbstractItemModel::rowsInserted, this, &MainWindow::onRowsInserted);
    connect(mLogFormatLoader.get(), &LogFormatLoader::logFormatChanged, mModel, &LogModel::setLogFormat);
}

MainWindow::~MainWindow() {

}

void MainWindow::loadLogFormat(const QString& filePath) {
    mLogFormatLoader->load(filePath);
}

void MainWindow::createUi() {
    mToolBar = addToolBar(tr("Toolbar"));

    mTreeView = new QTreeView();
    mTreeView->setModel(mModel);
    mTreeView->setRootIsDecorated(false);
    setCentralWidget(mTreeView);
}

void MainWindow::createActions() {
    mAutoScrollAction = new QAction("Auto Scroll");
    mAutoScrollAction->setCheckable(true);
    connect(mAutoScrollAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled) {
            mTreeView->scrollToBottom();
        }
    });

    mToolBar->addAction(mAutoScrollAction);
}

void MainWindow::onRowsInserted() {
    if (mAutoScrollAction->isChecked()) {
        mTreeView->scrollToBottom();
    }
}
