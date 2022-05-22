#ifndef ARRAYUTIL_H
#define ARRAYUTIL_H

template <typename T>
void shiftLeft(T* array, int size, T replaceLast) {
    for (int i = 1; i < size; ++i) {
        array[i-1] = array[i];
    }

    array[size-1] = replaceLast;
}

#endif