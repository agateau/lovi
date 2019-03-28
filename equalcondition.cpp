#include "equalcondition.h"

EqualCondition::EqualCondition(int column, const QString& expected)
    : Condition(column)
    , mExpected(expected) {
}

bool EqualCondition::eval(const QString& value) const {
    return value == mExpected;
}
