#include <iostream>
#include "min-max-heap.hpp"


int main(){

    SensorReading a;
    a.sensorID = 1;
    a.timestamp = 4;
    a.temperature = 15.5;

    SensorReading b;
    b.sensorID = 2;
    b.timestamp = 7;
    b.temperature = 30.5;
    
    SensorReading c;
    c.sensorID = 3;
    c.timestamp = 4;
    c.temperature = 20.5;

    SensorReading d;
    d.sensorID = 4;
    d.timestamp = 8;
    d.temperature = 70.56;

    SensorReading e;
    e.sensorID = 5;
    e.timestamp = 4;
    e.temperature = 10.5;


    SensorReading f;
    f.sensorID = 6;
    f.timestamp = 4;
    f.temperature = 20.7;


    SensorReading g;
    g.sensorID = 7;
    g.timestamp = 4;
    g.temperature = 15.3;


    MinMaxHeap Heap;

    Heap.insert(a);
    Heap.insert(b);
    Heap.insert(c);
    Heap.insert(d);
    Heap.insert(e);
    Heap.insert(f);
    Heap.insert(g);
    // Heap.insert(e);
    // Heap.insert(e);
    // Heap.insert(e);
    // Heap.insert(e);


    // Heap.delete_value(0);

    Heap.print();



    return 0;
}