#include "logformatdialog.h"
#include "ui_logformatdialog.h"

#include <QFileSystemModel>
#include <QStandardPaths>

static QString logFormatDirPath() {
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logformats";
}

/**
 * Override QFileSystemModel to hide the icon and the filename extension
 * Faster to write and use than using a proxy model
 */
class MyModel : public QFileSystemModel {
public:
    MyModel(QObject* parent = nullptr) : QFileSystemModel(parent) {}

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
    , mDirPath(logFormatDirPath())
    , mInitialLogFormatPath(logFormatPath) {
    ui->setupUi(this);

    mModel->setFilter(QDir::Files);
    mModel->sort(0);
    connect(mModel.get(), &QAbstractItemModel::rowsInserted, this, &LogFormatDialog::onRowsInserted);
    ui->listView->setModel(mModel.get());
    ui->listView->setRootIndex(mModel->setRootPath(mDirPath));
    connect(ui->listView, &QAbstractItemView::doubleClicked, this, [this](const QModelIndex& index) {
        if (index.isValid()) {
            accept();
        }
    });
}

LogFormatDialog::~LogFormatDialog() {
}

QString LogFormatDialog::logFormatPath() const {
    auto index = ui->listView->currentIndex();
    if (!index.isValid()) {
        return {};
    }
    return index.data(QFileSystemModel::FilePathRole).toString();
}

void LogFormatDialog::onRowsInserted(const QModelIndex& parent, int first, int last) {
    if (mInitialLogFormatPath.isEmpty()) {
        return;
    }
    for (int row = first; row <= last; ++row) {
        auto index = mModel->index(row, 0, parent);
        if (index.data(QFileSystemModel::FilePathRole).toString() == mInitialLogFormatPath) {
            ui->listView->setCurrentIndex(index);
            mInitialLogFormatPath.clear();
            return;
        }
    }
}
