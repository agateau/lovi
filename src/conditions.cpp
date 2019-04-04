#include "conditions.h"

Condition::Condition(int column) : mColumn(column) {
}

int Condition::column() const {
    return mColumn;
}

ExactCondition::ExactCondition(int column, const QString& expected)
        : Condition(column), mExpected(expected) {
}

bool ExactCondition::eval(const QString& value) const {
    return value == mExpected;
}

ContainsCondition::ContainsCondition(int column, const QString& expected)
        : Condition(column), mExpected(expected) {
}

bool ContainsCondition::eval(const QString& value) const {
    return value.contains(mExpected);
}

RegExCondition::RegExCondition(int column, const QRegularExpression& regEx)
        : Condition(column), mRegEx(regEx) {
    mRegEx.optimize();
}

bool RegExCondition::eval(const QString& value) const {
    return mRegEx.match(value).hasMatch();
}
