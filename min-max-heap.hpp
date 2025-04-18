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
    MinMaxHeap() { size = 0; };
    bool isMinLevel(int i);
    void swap(int i, int j);
    int grandparent(int i);
    int parent(int i);
    int left_child(int i);
    int right_child(int i);
    std::vector<int> get_children(int i);
    int min_descendent(int i);
    int max_descendent(int i);

    void bubble_up(int i);
    void bubble_up_min(int i);
    void bubble_up_max(int i);
    void insert(T x);

    void trickle_down(int i);
    void trickle_down_min(int i);
    void trickle_down_max(int i);

    bool replace(int i, T x);
    bool delete_value(int i);

    T minimum_val(){ return arr[0]; }
    T maximum_val();
};
