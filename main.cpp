#include "logformat.h"
#include "filelineprovider.h"
#include "filewatcher.h"
#include "logmodel.h"
#include "mainwindow.h"

#include <QAbstractListModel>
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

#include <iostream>
#include <memory>

using std::optional;
using std::cout;
using std::unique_ptr;

void dumpModel(LogModel* model) {
    int rowCount = model->rowCount();
    int columnCount = model->columns().count();
    cout << "<html><body>\n";
    cout << "<table>\n";
    for (int row = 0; row < rowCount; ++row) {
        auto idx = model->index(row, 0);
        QVariant bgColorVariant = idx.data(Qt::BackgroundColorRole);
        cout << "<tr";
        if (bgColorVariant.isValid()) {
            QColor color = bgColorVariant.value<QColor>();
            cout << " style='background-color:" << color.name().toStdString() << "'";
        }
        cout << ">";

        for (int column = 0; column < columnCount; ++column) {
            QString value = model->index(row, column).data().toString();
            cout << "<td>" << value.toStdString() << "</td>";
        }
        cout << "</tr>\n";
    }
    cout << "</table>\n";
    cout << "</body></html>\n";
}

unique_ptr<QCommandLineParser> createParser() {
    unique_ptr<QCommandLineParser> parser = std::make_unique<QCommandLineParser>();
    parser->setApplicationDescription(QCoreApplication::translate("main", "Log viewer"));
    parser->addHelpOption();
    parser->addVersionOption();
    parser->addPositionalArgument("log_file", QCoreApplication::translate("main", "Log file."));
    parser->addOption({{"f", "format"},
                     QCoreApplication::translate("main", "Log format definition."),
                     QCoreApplication::translate("main", "log_format.json"),
                     });
    return parser;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    unique_ptr<QCommandLineParser> parser = createParser();
    parser->process(app);

    QString logFileName = parser->positionalArguments().first();

    FileLineProvider lineProvider;
    lineProvider.setFilePath(logFileName);
    LogModel model(&lineProvider);

    //dumpModel(&model);
    MainWindow window(&model);
    if (parser->isSet("format")) {
        window.loadLogFormat(parser->value("format"));
    }
    window.show();
    return app.exec();
}
