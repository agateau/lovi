#include <QAbstractListModel>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

#include <iostream>
#include <memory>
#include <vector>

using std::optional;
using std::cout;
using std::unique_ptr;
using std::vector;

class Condition {
public:
    explicit Condition(int role) : mRole(role) {}
    virtual ~Condition() = default;
    virtual bool eval(const QString& value) const = 0;

    int role() const {
        return mRole;
    }

protected:
    const int mRole;
};

class EqualCondition : public Condition {
public:
    explicit EqualCondition(int role, const QString& expected)
        : Condition(role)
        , mExpected(expected) {
    }

    bool eval(const QString& value) const override {
        return value == mExpected;
    }

private:
    QString mExpected;
};

class Highlight {
public:
    unique_ptr<Condition> condition;
    QString bgColor;
};

class Config {
public:
    QRegularExpression parser;
    vector<Highlight> highlights;

    static optional<Config> fromJsonDocument(const QJsonDocument& doc) {
        auto regex = doc.object().value("parser").toObject().value("regex").toString();
        if (regex.isEmpty()) {
            qWarning() << "No regex found";
            return {};
        }

        Config config;
        config.parser.setPattern(regex);
        if (!config.parser.isValid()) {
            qWarning() << "Invalid regex";
            return {};
        }
        config.parser.optimize();

        QHash<QString, int> roleForColumn;
        {
            int role = 0;
            for (const auto& column : config.parser.namedCaptureGroups()) {
                if (!column.isEmpty()) {
                    roleForColumn[column] = role++;
                }
            }
        }

        for (const QJsonValue& jsonValue : doc.object().value("highlights").toArray()) {
            QJsonObject highlightObj = jsonValue.toObject();
            auto conditionObj = highlightObj.value("condition").toObject();
            auto column = conditionObj.value("column").toString();
            auto value = conditionObj.value("value").toString();
            auto bgColor = highlightObj.value("bgColor").toString();

            auto it = roleForColumn.find(column);
            if (it == roleForColumn.end()) {
                qWarning() << "No column named" << column;
                return {};
            }
            Highlight highlight;
            highlight.condition = std::make_unique<EqualCondition>(it.value(), value);
            highlight.bgColor = bgColor;
            config.highlights.push_back(std::move(highlight));
        }

        return std::move(config);
    }
};

class LogModel : public QAbstractListModel {
public:
    LogModel(const Config& config, const QStringList& lines)
        : mConfig(config)
        , mLines(lines) {
        mColumns = mConfig.parser.namedCaptureGroups();
        mColumns.removeFirst();
    }

    int rowCount(const QModelIndex& parent = {}) const override {
        if (parent.isValid()) {
            return 0;
        }
        return mLines.count();
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        int row = index.row();
        if (row < 0 || row >= mLines.count()) {
            return {};
        }
        auto it = mKnownMatches.find(row);
        QRegularExpressionMatch match;
        if (it == mKnownMatches.end()) {
            QString line = mLines.at(row);
            match = mConfig.parser.match(line);
            mKnownMatches[row] = match;
        } else {
            match = it.value();
        }
        if (!match.hasMatch()) {
            QString line = mLines.at(row);
            qDebug() << "Line" << row + 1 << "does not match:" << line;
            return role == 0 ? QVariant(line) : QVariant();
        }
        return match.captured(role + 1);
    }

    QStringList columns() const {
        return mColumns;
    }

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> hash;
        int idx = 0;
        for (const auto& column : mColumns) {
            hash[idx++] = column.toUtf8();
        }
        return hash;
    }

private:
    const Config& mConfig;
    const QStringList mLines;
    QStringList mColumns;
    mutable QHash<int, QRegularExpressionMatch> mKnownMatches;
};

void dumpModel(LogModel* model, const Config& config) {
    int count = model->rowCount();
    int columnCount = model->columns().count();
    cout << "<html><body>\n";
    cout << "<table>\n";
    for (int row = 0; row < count; ++row) {
        auto idx = model->index(row);
        QString bgColor;
        for (int role = 0; role < columnCount; ++role) {
            QString value = idx.data(role).toString();
            for (const Highlight& highlight : config.highlights) {
                 if (highlight.condition->role() == role) {
                     if (highlight.condition->eval(value)) {
                         bgColor = highlight.bgColor;
                         break;
                     }
                 }
            }
        }

        cout << "<tr";
        if (!bgColor.isEmpty()) {
            cout << " style='background-color:" << bgColor.toStdString() << "'";
        }
        cout << ">";

        for (int role = 0; role < columnCount; ++role) {
            QString value = idx.data(role).toString();
            cout << "<td>" << value.toStdString() << "</td>";
        }
        cout << "</tr>\n";
    }
    cout << "</table>\n";
    cout << "</body></html>\n";
}

optional<QByteArray> readFile(const QString& fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Could not open" << fileName;
        return {};
    }
    return file.readAll();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString configFileName = argv[1];
    QString logFileName = argv[2];

    auto json = readFile(configFileName);
    if (!json.has_value()) {
        return 1;
    }

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(json.value(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCritical() << "Invalid Json in" << configFileName << ":" << error.errorString();
        return 1;
    }

    auto config = Config::fromJsonDocument(doc);
    if (!config) {
        qWarning() << "Failed to parse" << configFileName;
        return 1;
    }

    QStringList lines;
    {
        auto logContent = readFile(logFileName);
        if (!logContent.has_value()) {
            return 1;
        }
        lines = QString::fromUtf8(logContent.value()).split('\n');
    }

    LogModel model(config.value(), lines);

    dumpModel(&model, config.value());

    return 0;
}
