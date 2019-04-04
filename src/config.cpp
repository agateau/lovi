#include "config.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

Config::Config(const QString& configPath, QObject* parent)
        : QObject(parent), mConfigPath(configPath) {
    load();
}

QStringList Config::recentLogFiles() const {
    return mRecentLogFiles;
}

void Config::setRecentLogFiles(const QStringList& files) {
    mRecentLogFiles = files;
    save();
}

void Config::load() {
    QFile file(mConfigPath);
    if (!file.exists()) {
        return;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot read" << mConfigPath << ":" << file.errorString();
        return;
    }
    QByteArray json = file.readAll();

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Cannot parse" << mConfigPath << ":" << error.errorString();
        return;
    }

    auto root = doc.object();
    {
        auto array = root.value("recentLogFiles").toArray();
        mRecentLogFiles.clear();
        for (const QJsonValue value : array) {
            mRecentLogFiles << value.toString();
        }
    }
    qDebug() << mRecentLogFiles;
}

void Config::save() const {
    QFile file(mConfigPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot write to" << mConfigPath << ":" << file.errorString();
        return;
    }

    QJsonObject root;
    {
        QJsonArray array;
        for (const auto& value : mRecentLogFiles) {
            array << QJsonValue(value);
        }
        root["recentLogFiles"] = array;
    }
    file.write(QJsonDocument(root).toJson());
}
