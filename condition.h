#ifndef CONDITION_H
#define CONDITION_H

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


#endif // CONDITION_H
