#ifndef VARIABLE_TYPE_H
#define VARIABLE_TYPE_H
#include <cstdint>

enum class VariableType : std::uint8_t {
    qCommonType = 1 << 0,
    qNamedType = 1 << 1,
    qArrayType = 1 << 2
};

template <VariableType TagValue>
struct VariableTypeTag {
    static constexpr VariableType Tag = TagValue;
};

#endif // VARIABLE_TYPE_H
