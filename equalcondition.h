#ifndef EQUALCONDITION_H
#define EQUALCONDITION_H

#include "condition.h"

class EqualCondition : public Condition {
public:
    explicit EqualCondition(int role, const QString& expected);

    bool eval(const QString& value) const override;

private:
    QString mExpected;
};


#endif // EQUALCONDITION_H
