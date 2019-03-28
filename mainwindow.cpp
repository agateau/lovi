#include "mainwindow.h"

#include "logmodel.h"

#include <QTreeView>

MainWindow::MainWindow(LogModel* logModel, QWidget* parent)
    : QMainWindow(parent)
    , mModel(logModel) {

    QTreeView* view = new QTreeView();
    view->setModel(mModel);
    view->setRootIsDecorated(false);
    setCentralWidget(view);
}
