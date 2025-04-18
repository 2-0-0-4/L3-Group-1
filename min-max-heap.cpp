#include "min-max-heap.hpp"

template <class T>
bool MinMaxHeap<T>::isMinLevel(int i)
{
    return log2(i + 1) % 2 == 0;
}

template <class T>
int MinMaxHeap<T>::grandparent(int i)
{
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
std::vector<int> MinMaxHeap<T>::get_children(int i)
{ // returns vector of indexes of children + grand children

    std::vector<int> children;

    if (i < size)
    { // push children to arrray
        children.push_back(right_child(i));
        children.push_back(left_child(i));
    }
    for (int child : children)
    { // iterate over children and push grand children
        if (child < size)
        {
            children.push_back(right_child(child));
            children.push_back(left_child(child));
        }
    }

    return children;
}

template <class T>
int MinMaxHeap<T>::min_descendent(int i) // returns index of min descendent
{
    std::vector<int> children = get_children(i);

    if (children.empty())
    { // no descendents
        return -1;
    }
    int min = chiildren[0];
    for (int child : children)
    {
        if (arr[child] < arr[min])
        {
            min = child;
        }
    }

    return min;
}

template <class T>
int MinMaxHeap<T>::max_descendent(int i)
{
    std::vector<int> children = get_children(i);

    if (children.empty())
    { // no descendents
        return -1;
    }
    int max = chiildren[0];
    for (int child : children)
    {
        if (arr[child] > arr[min])
        {
            max = child;
        }
    }

    return max;
}

template <class T>
void MinMaxHeap<T>::bubble_up(int i)
{

    if (i == 0)
        return; // cant bubble upe from here

    int parent = parent(i); // get parent

    if (isMinLevel(i))
    { // for min level
        if (arr[i] > arr[parent])
        {                    // if value is greater than parent shift it to a max level
            swap(i,parent);
            bubble_up_max(parent); // it is now in max level
        }
        else
        {
            bubble_up_min(i);
            //  bubble_up_min(parent);
        }
    }
    else
    {
        if (arr[i] < arr[parent])
        {                    // if value is lesser than parent shift it to a min level
            swap(i,parent);
            bubble_up_min(parent); // it is now in min level
        }
        else
        {
            bubble_up_max(i);
            // bubble_up_max(parent);
        }
    }
}

template <class T>
void MinMaxHeap<T>::bubble_up_min(int i)
{
    int grandparent = grandparent(i);
    while (i > 2 && (arr[i] < arr[grandparent]))
    {                    // check for i >2 to make sure i has a grandparent (is a deeper level node)
        swap(i,grandparent);
        i = grandparent;
        grandparent = grandparent(i);
    }
}

template <class T>
void MinMaxHeap<T>::bubble_up_max(int i)
{
    int grandparent = grandparent(i);
    while (i > 2 && (arr[i] > arr[grandparent]))
    {
        swap(i,grandparent);
        i = grandparent;
        grandparent = grandparent(i);
    }
}

template <class T>
void MinMaxHeap<T>::insert(T x)
{

    // resize?
    arr.push_back(x);
    size++;
    bubble_up(size - 1);
}

template <class T>
void MinMaxHeap<T>::trickle_down(int i)
{
    if (i < size)
    {

        if (isMinLevel)
        {
            trickle_down_min(i);
        }
        else
        {
            trickle_down_max(i);
        }
    }
}

template <class T>
void MinMaxHeap<T>::trickle_down_min(int i)
{
    int min;
    int min_parent;

    while(heap.size() > left_child(i)){ //while node has children
        // return;
    min = min_descendent(i);
    if(min == -1)
        break;
    min_parent = parent(min);
    
    if(arr[min]<arr[i]){
        swap(min, i);
        if(min >= 4*i +3) {//is a grandchild
            if(arr[min_parent] < arr[min]){
                swap(min, min_parent)
            }
            // trickle_down_min(min);
            i = min;
        }else{
            break;
        }
    }else{
        break;
    }
}
}


template <class T>
void MinMaxHeap<T>::trickle_down_max(int i)
{
    int max;
    int max_parent;

    while(heap.size() > left_child(i)){ //while node has children
        // return;
    max = max_descendent(i);
    if(max == -1)
        break;
    max_parent = parent(max);
    
    if(arr[max]>arr[i]){
        swap(max, i);
        if(max >= 4*i +3) {//is a grandchild
            if(arr[max_parent] > arr[max]){
                swap(max, max_parent)
            }
            
            i = max;
        }else{
            break;
        }
    }else{
        break;
    }
}

}



template <class T>
bool MinMaxHeap<T>::replace(int i, T x)
{
    //delete value
    // change temp 
    // insert 
}

template <class T>
bool MinMaxHeap<T>::delete_value(int i)
{

//call trickle down 
}

template <class T>
void MinMaxHeap<T>::swap(int i, int j)
{
    T temp = arr[i]; // swap
    arr[i] = arr[j];
    arr[j] = temp;
}

template <class T>
T MinMaxHeap<T>::maximum_val(){
    if (size == 1){ //if only one element return that
			return arr[0];
    }else if (size == 2){ //else go to level 2
			return arr[1];
    }else{
        if(arr[1] > arr[2]){ // if more, check between right and left children and return the max one 
            return arr[1];
        }else{
            return arr[2]
        }
			
    }
}