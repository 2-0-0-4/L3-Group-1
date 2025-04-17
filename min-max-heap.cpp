#include "min-max-heap.hpp"



template <class T>
bool MinMaxHeap<T>::isMinLevel(int i){
    return log2(zindex + 1) % 2 == 0;
}

template <class T>
int MinMaxHeap<T>::grandparent(int i){
   return parent(parent(i));
}

template <class T>
int MinMaxHeap<T>::parent(int i)
{
    return (i - 1) / 2;
}

template <class T>
int MinMaxHeap<T>::left_child(int i)
{
    return 2 * i + 1;
}

template <class T>
int MinMaxHeap<T>::right_child(int i)
{
    return 2 * i + 2;
}

template <class T>
void MinMaxHeap<T>::bubble_up(int i){

    if(i==0)
    return; //cant bubble upe from here 

    int parent = parent(i); //get parent

    if (isMinLevel(i)){ //for min level
        if(arr[i]>arr[parent]) {// if value is greater than parent shift it to a max level
            T temp = arr[i]; //swap
            arr[i] = arr[parent];
            arr[parent] = temp;
            bubble_up_max(i); //it is now in max level 
        }else{
            bubble_up_min(i);
        }
    }else{
        if(arr[i]<arr[parent]) {// if value is lesser than parent shift it to a min level
            T temp = arr[i]; //swap
            arr[i] = arr[parent];
            arr[parent] = temp;
            bubble_up_min(i); //it is now in min level 
        }else{
            bubble_up_max(i);
        }
    }


}

template <class T>
void MinMaxHeap<T>::bubble_up_min(int i){
    int grandparent = grandparent(i);
    while(i >2 && ( arr[i] < arr[grandparent]) ){ //check for i >2 to make sure i has a grandparent (is a deeper level node)
        T temp = arr[i]; //swap
        arr[i] = arr[grandparent];
        arr[grandparent] = temp;
        i = grandparent; 
        grandparent = grandparent(i);
    }

}

template <class T>
void MinMaxHeap<T>::bubble_up_max(int i){
    int grandparent = grandparent(i);
    while(i >2 && ( arr[i] > arr[grandparent]) ){
        T temp = arr[i]; //swap
        arr[i] = arr[grandparent];
        arr[grandparent] = temp;
        i = grandparent; 
        grandparent = grandparent(i);
    }


}

template <class T>
void MinMaxHeap<T>::insert(T x){

    //resize?
    arr[size] = x;
    size++;
    bubble_up(size-1);

} 


template <class T>
void MinMaxHeap<T>::trickle_down(int i){

    
}



template <class T>
bool MinMaxHeap<T>::replace(int i, T x){

}

template <class T>
bool MinMaxHeap<T>::delete_value(int i){

}

