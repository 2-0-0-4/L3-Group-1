#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <random>
#include <chrono>
#include <string>
#include <vector>

using namespace std;
using namespace chrono;

// Configuration
const int numSensors = 15;
const int delayMs = 100; // Interval between readings per sensor
const int anomalyEvery = 15; // Inject anomalies every 50 readings
const int seed = 42; // GLOBAL SEED for reproducibility

// SensorReading structure
struct SensorReading {
    int sensorID;
    long long timestamp;
    float temperature;

    void print() const {
        cout << "[Sensor " << sensorID << "] @ " << timestamp
             << " ms => Temp: " << temperature << " C\n";
    }
};

// Shared queue and mutex
queue<SensorReading> readingQueue;
mutex queueMutex;

// Random engines and distributions
default_random_engine globalGen(seed);
uniform_real_distribution<float> normalDist(40.0, 45.0);
uniform_real_distribution<float> spikeDist(75.0, 85.0);
uniform_real_distribution<float> coldDist(10.0, 25.0);

// Get current timestamp
long long currentTimestamp() {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

// Sensor simulation
void sensorStream(int sensorID) {
    int count = 0;
    while (true) {
        SensorReading reading;
        reading.sensorID = sensorID;
        reading.timestamp = currentTimestamp();
        reading.temperature = normalDist(globalGen);

        // These static variables maintain anomaly configuration for current window
        static int isolatedSpikeSensor = uniform_int_distribution<int>(1, numSensors)(globalGen);
        static int clusterStartSensor = uniform_int_distribution<int>(1, numSensors - 4)(globalGen);
        static int coldSpotSensor = uniform_int_distribution<int>(1, numSensors)(globalGen);
        static int coldClusterStartSensor = uniform_int_distribution<int>(1, numSensors - 4)(globalGen);
        static int coldMalfunctionSensor = uniform_int_distribution<int>(1, numSensors)(globalGen);

        // Inject anomalies every N readings
        if (count % anomalyEvery == 0) {
            if (sensorID == isolatedSpikeSensor) {
                reading.temperature = spikeDist(globalGen);
            }

            if (sensorID >= clusterStartSensor && sensorID < clusterStartSensor + 5) {
                reading.temperature = spikeDist(globalGen);
            }

            if (sensorID >= coldClusterStartSensor && sensorID < coldClusterStartSensor + 5) {
                reading.temperature = coldDist(globalGen);
            }


            if (sensorID == coldSpotSensor) {
                reading.temperature = coldDist(globalGen);
            }

            if (sensorID == coldMalfunctionSensor) {
                reading.temperature = coldDist(globalGen);
            }

            // Only one thread resets anomaly pattern to avoid race condition
            if (sensorID == 1) {
                isolatedSpikeSensor = uniform_int_distribution<int>(1, numSensors)(globalGen);
                clusterStartSensor = uniform_int_distribution<int>(1, numSensors - 4)(globalGen);
                coldSpotSensor = uniform_int_distribution<int>(1, numSensors)(globalGen);
                coldClusterStartSensor = uniform_int_distribution<int>(1, numSensors - 4)(globalGen);
                coldMalfunctionSensor = uniform_int_distribution<int>(1, numSensors)(globalGen);
            }
        }


        // Push to queue
        {
            lock_guard<mutex> lock(queueMutex);
            readingQueue.push(reading);
        }

        this_thread::sleep_for(milliseconds(delayMs));
        count++;
    }
}

// Monitor thread
void monitorReadings() {
    while (true) {
        SensorReading reading;
        {
            lock_guard<mutex> lock(queueMutex);
            if (!readingQueue.empty()) {
                reading = readingQueue.front();
                readingQueue.pop();
                reading.print(); // Replace this with heap insertion
            }
        }
        this_thread::sleep_for(milliseconds(50));
    }
}

int main() {
    // Start sensor threads
    for (int i = 1; i <= numSensors; ++i) {
        thread(sensorStream, i).detach();
    }

    // Start monitor thread
    monitorReadings();
    return 0;
}
