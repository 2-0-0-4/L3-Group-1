#pragma once
#include <vector>
#include <string>
#include <iostream>
// #include <stream.cpp>
// both max and min
// template <class T>

struct SensorReading
{
    int sensorID;
    long long timestamp;
    float temperature;

    void print() const
    {
        // append all the readings output on the console to a sensor_output.txt file
        // std::ofstream outFile("sensor_output.txt", std::ios::app);
        // outFile << sensorID << ' ' << timestamp
        //         << ' ' << temperature << "\n";
        // outFile.close();

        std::cout << "[Sensor " << sensorID << "] @ " << timestamp
             << " ms => Temp: " << temperature << " C\n";
    }
};
class MinMaxHeap
{

private:
    std::vector<SensorReading> arr;
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
    // std::vector<SensorReading> get_grandchildren_readings(int i);
    int min_descendent(int i);
    int max_descendent(int i);

    void bubble_up(int i);
    void bubble_up_min(int i);
    void bubble_up_max(int i);
    void insert(SensorReading);
    void replace(SensorReading x); //replaces/inserts depending on if value alr exists 

    void trickle_down(int i);
    void trickle_down_min(int i);
    void trickle_down_max(int i);

    void delete_value(int i);

    std::vector<SensorReading> top_k_min(int k);
    std::vector<SensorReading> top_k_max(int k);

    SensorReading minimum_val() { return arr[0]; }
    SensorReading maximum_val();

    int get_size();

    void print();















    
};
