#include "condition.h"

Condition::Condition(int column) : mColumn(column) {}

int Condition::column() const {
    return mColumn;
}
