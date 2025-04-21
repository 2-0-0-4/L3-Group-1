#include "min-max-heap.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

// template <class T>
bool MinMaxHeap::isMinLevel(int i)
{
    return int(floor(log2(i + 1))) % 2 == 0;
}

void MinMaxHeap::swap(int i, int j)
{
    SensorReading temp = arr[i]; // swap
    arr[i] = arr[j];
    arr[j] = temp;
}

int MinMaxHeap::grandparent(int i)
{
    return parent(parent(i));
}

int MinMaxHeap::parent(int i)
{
    return (i - 1) / 2;
}

int MinMaxHeap::left_child(int i)
{
    return (2 * i) + 1;
}

int MinMaxHeap::right_child(int i)
{
    return (2 * i )+ 2;
}

std::vector<int> MinMaxHeap::get_children(int i)
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

// std::vector<int> MinMaxHeap::get_grandchildren(int i){

//     // std::vector<SensorReading> children;
//     std::vector<int> children_index;
//     if (i < size)
//     { // push children to arrray
//         children_index.push_back(right_child(i));
//         // children.push_back(arr[right_child(i)]);

//         children_index.push_back(left_child(i));
//         // children.push_back(arr[left_child(i)]);
//     }
//     for (int child : children_index)
//     { // iterate over children and push grand children
//         if (child < size)
//         {
//             children.push_back(arr[right_child(child)]);
//             children.push_back(arr[left_child(child)]);
//         }
//     }

//     return children;

// }

int MinMaxHeap::min_descendent(int i) // returns index of min descendent
{
    std::vector<int> children = get_children(i);

    if (children.empty())
    { // no descendents
        return -1;
    }
    int min = children[0];
    for (int child : children)
    {
        if (arr[child].temperature < arr[min].temperature)
        {
            min = child;
        }
    }

    return min;
}

int MinMaxHeap::max_descendent(int i)
{
    std::vector<int> children = get_children(i);

    if (children.empty())
    { // no descendents
        return -1;
    }
    int max = children[0];
    for (int child : children)
    {
        if (arr[child].temperature > arr[max].temperature)
        {
            max = child;
        }
    }

    return max;
}

void MinMaxHeap::bubble_up(int i)
{

    if (i == 0)
        return; // cant bubble upe from here

    int parent = this->parent(i); // get parent

    if (isMinLevel(i))
    { // for min level
        if (arr[i].temperature > arr[parent].temperature)
        { // if value is greater than parent shift it to a max level
            swap(i, parent);
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
        if (arr[i].temperature < arr[parent].temperature)
        { // if value is lesser than parent shift it to a min level
            swap(i, parent);
            bubble_up_min(parent); // it is now in min level
        }
        else
        {
            bubble_up_max(i);
            // bubble_up_max(parent);
        }
    }
}

void MinMaxHeap::bubble_up_min(int i)
{
    int grandparent = this->grandparent(i);
    while (i > 2 && (arr[i].temperature < arr[grandparent].temperature))
    { // check for i >2 to make sure i has a grandparent (is a deeper level node)
        swap(i, grandparent);
        i = grandparent;
        grandparent = this->grandparent(i);
    }
}

void MinMaxHeap::bubble_up_max(int i)
{
    int grandparent = this->grandparent(i);
    while (i > 2 && (arr[i].temperature > arr[grandparent].temperature))
    {
        swap(i, grandparent);
        i = grandparent;
        grandparent = this->grandparent(i);
    }
}

void MinMaxHeap::insert(SensorReading x)
{

    // resize?
    arr.push_back(x);
    size++;
    bubble_up(size - 1);
}

void MinMaxHeap::replace(SensorReading x) // replaces if alr there else inserts
{
    // delete value
    //  change temp
    //  insert

    for (int i = 0; i < size; i++)
    {
        if (this->arr[i].sensorID == x.sensorID)
        {
            this->delete_value(i);
            this->insert(x);
            return;
        }
    }
    this->insert(x);
}

void MinMaxHeap::trickle_down(int i)
{
    if (i < size)
    {

        if (isMinLevel(i))
        {
            trickle_down_min(i);
        }
        else
        {
            trickle_down_max(i);
        }
    }
}

void MinMaxHeap::trickle_down_min(int i)
{
    int min;
    int min_parent;

    while (size > left_child(i))
    { // while node has children
        // return;
        min = min_descendent(i);
        if (min == -1)
            break;
        min_parent = parent(min);

        if (arr[min].temperature < arr[i].temperature)
        {
            swap(min, i);
            if (min >= 4 * i + 3)
            { // is a grandchild
                if (arr[min_parent].temperature < arr[min].temperature)
                {
                    swap(min, min_parent);
                }
                // trickle_down_min(min);
                i = min;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
}

void MinMaxHeap::trickle_down_max(int i)
{
    int max;
    int max_parent;

    while (size > left_child(i))
    { // while node has children
        // return;
        max = max_descendent(i);
        if (max == -1)
            break;
        max_parent = parent(max);

        if (arr[max].temperature > arr[i].temperature)
        {
            swap(max, i);
            if (max >= 4 * i + 3)
            { // is a grandchild
                if (arr[max_parent].temperature > arr[max].temperature)
                {
                    swap(max, max_parent);
                }

                i = max;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
}

void MinMaxHeap::delete_value(int i)
{

    // call trickle down
    if (i >= size)
        return;

    if (i == size - 1)
    {
        arr.pop_back();
        size--;
        return;
    }

    arr[i] = arr.back();
    arr.pop_back();
    size--;

    if (i < size)
    {
        if (i > 0 && (isMinLevel(i)) && (arr[i].temperature < arr[parent(i)].temperature))
        { // if violates property w parents (if it has a aprent)
            bubble_up(i);
        }
        else
        {
            trickle_down(i);
        }
    }
}

std::vector<SensorReading> MinMaxHeap::top_k_min(int k)
{
    std::vector<SensorReading> min_readings;

    if (size == 0)
    {
        return min_readings;
    }

    min_readings.push_back(arr[0]);

    std::vector<int> counting = {0}; // begin at zero

    while (min_readings.size() < k && !counting.empty())
    {
        std::vector<int> Temp;
        std::vector<int> new_counting;
        for (int count : counting)
        { // get all grandchildren of values in count vector

            std::vector<int> children = this->get_children(count);
            for (int child : children)
            {
                if (child != left_child(count) && child != right_child(count))
                { // only grandchildren get appended
                    Temp.push_back(child);
                    new_counting.push_back(child); // keep appending the grandchildren to new counting vector
                }
            }
        }
        std::sort(Temp.begin(), Temp.end(), [this](int a, int b)
                  { return arr[a].temperature < arr[b].temperature; });

        for (int j : Temp)
        {
            if (min_readings.size() < k)
            {
                min_readings.push_back(arr[j]);
            } // push the sorted grandchildren into the min readings array
        }

        counting = new_counting; // make new counting the counting vector
    }

    return min_readings;
}

std::vector<SensorReading> MinMaxHeap::top_k_max(int k)
{
    std::vector<SensorReading> max_readings;

    if (size == 0)
    {
        return max_readings;
    }
    if (size == 1)
    {
        max_readings.push_back(arr[0]);
        return max_readings;
    }

    if (size == 2)
    {
        max_readings.push_back(arr[1]);
        return max_readings;
    }
    if (size == 3)
    {
        max_readings.push_back(arr[1]);
        max_readings.push_back(arr[1]);
        std::sort(max_readings.begin(), max_readings.end(), [](auto &a, auto &b)
                  { return a.temperature > b.temperature; });
        return max_readings;
    }

    std::vector<int> counting = {1, 2}; // begin at first max level

    while (max_readings.size() < k && !counting.empty())
    {
        std::vector<int> Temp;
        std::vector<int> new_counting;
        for (int count : counting)
        { // get all grandchildren of values in count vector

            std::vector<int> children = this->get_children(count);
            for (int child : children)
            {
                if (child != left_child(count) && child != right_child(count))
                { // only grandchildren get appended
                    Temp.push_back(child);
                    new_counting.push_back(child); // keep appending the grandchildren to new counting vector
                }
            }
        }
        std::sort(Temp.begin(), Temp.end(), [this](int a, int b)
                  { return arr[a].temperature > arr[b].temperature; });

        for (int j : Temp)
        {
            if (max_readings.size() < k)
            {
                max_readings.push_back(arr[j]);
            }
        }

        counting = new_counting; // make new counting the counting vector
    }

    return max_readings;
}

SensorReading MinMaxHeap::maximum_val()
{
    if (size == 1)
    { // if only one element return that
        return arr[0];
    }
    else if (size == 2)
    { // else go to level 2
        return arr[1];
    }
    else
    {
        if (arr[1].temperature > arr[2].temperature)
        { // if more, check between right and left children and return the max one
            return arr[1];
        }
        else
        {
            return arr[2];
        }
    }
}

int MinMaxHeap::get_size()
{

    return size;
}

void MinMaxHeap::print(){

    for(int i =0; i < size; i++){
        std::cout << "Sensor ID: " << arr[i].sensorID << std::endl;
        std::cout << "TimeStamp: " << arr[i].timestamp << std::endl;
        std::cout << "Temperature: " << arr[i].temperature << std::endl;

   }
}