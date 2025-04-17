#pragma once
#include <vector>

// both max and min
template <class T>
class MinMaxHeap
{

private:
    std::vector<T> arr;
    int size;

public:
    MinMaxHeap(){size = 0;};
    bool isMinLevel(int i);
    int grandparent(int i);
    int parent(int i);
    int left_child(int i);
    int right_child(int i);
    T root() { return arr[0]; }
    
    void bubble_up(int i);
    void bubble_up_min(int i);
    void bubble_up_max(int i);
    void insert(T x);

    void trickle_down(int i);
    void insert(T x);
    bool replace(int i, T x);
    bool delete_value(int i);

};
