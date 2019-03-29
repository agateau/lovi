#ifndef CONDITIONS_H
#define CONDITIONS_H

#include <QString>

class Condition {
public:
    explicit Condition(int role);
    virtual ~Condition() = default;
    virtual bool eval(const QString& value) const = 0;

    int column() const;

private:
    const int mColumn;
};

class EqualCondition : public Condition {
public:
    explicit EqualCondition(int role, const QString& expected);

    bool eval(const QString& value) const override;

private:
    QString mExpected;
};


#endif // CONDITIONS_H
