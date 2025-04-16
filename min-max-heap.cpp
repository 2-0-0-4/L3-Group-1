#include "min-max-heap.hpp"

template <class T>
int Heap<T>::parent(int i)
{
    return (i - 1) / 2;
}

template <class T>
int Heap<T>::left_child(int i)
{
    return 2 * i + 1;
}

template <class T>
int Heap<T>::right_child(int i)
{
    return 2 * i + 2;
}
