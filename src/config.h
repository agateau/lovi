#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

class Config : public QObject {
    Q_OBJECT
public:
    explicit Config(const QString& configPath, QObject* parent = nullptr);

    QStringList recentLogFiles() const;
    void setRecentLogFiles(const QStringList& files);

private:
    void load();
    void save() const;
    const QString mConfigPath;

    QStringList mRecentLogFiles;
};

#endif // CONFIG_H
