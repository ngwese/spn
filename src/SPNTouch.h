#pragma once

struct SPNTouch {
    int index;
    float x;
    float y;
    float z;
    float note;
    int state;

    SPNTouch(int index, float x, float y, float z, float note, int state)
        : index(index), x(x), y(y), z(z), note(note), state(state) {}
};
