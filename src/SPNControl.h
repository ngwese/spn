#pragma once

#include <MLSymbol.h>

struct SPNControl {
    ml::Symbol name;
    float x;
    float y;
    float z;
    int n1;
    int n2;

    SPNControl(const ml::Symbol &name, float x, float y, float z, int n1,
               int n2 = 0)
        : name(name), x(x), y(y), z(z), n1(n1), n2(n2) {}
};
