#pragma once

//both max and min 
template <class T>
class Heap{

    private:

    array<T> arr;
    int size;

    public:

    int parent(int i);
    int left_child(int i);
    int right_child(int i);
    T root(){ return arr[0];}
};

template <class T>
class MinHeap : public Heap{

    };

template <class T>
class MaxHeap : public Heap{

    };