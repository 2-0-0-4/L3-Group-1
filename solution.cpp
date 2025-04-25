// copy streeam.cpp to modify

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <random>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <set>
#include "min-max-heap.hpp"

using namespace std;
using namespace chrono;

// Configuration
const int numSensors = 15;
const int delayMs = 100;                              // Interval between readings per sensor
const int anomalyEvery = 15;                          // Inject anomalies every 50 readings
const int seed = 8969;                                // GLOBAL SEED for reproducibility
vector<pair<int, pair<long long, int>>> cold_counter; // Shared queue and mutex
queue<SensorReading> readingQueue;
mutex queueMutex;

// Random engines and distributions
default_random_engine globalGen(seed);
uniform_real_distribution<float> normalDist(40.0, 45.0);
uniform_real_distribution<float> spikeDist(75.0, 85.0);
uniform_real_distribution<float> coldDist(10.0, 25.0);

const long long VALID_MS = 60000;  // 60 seconds

std::deque<SensorReading> Buffer;  // A buffer to hold the readings and check fi they cross 60 sec


// Get current timestamp
long long currentTimestamp()
{
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

// Sensor simulation
void sensorStream(int sensorID)
{
    // MinMaxHeap SensorStream; //declare a heap
    int count = 0;
    while (true)
    {
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
        if (count % anomalyEvery == 0)
        {
            if (sensorID == isolatedSpikeSensor)
            {
                reading.temperature = spikeDist(globalGen);
            }

            if (sensorID >= clusterStartSensor && sensorID < clusterStartSensor + 5)
            {
                reading.temperature = spikeDist(globalGen);
            }

            if (sensorID >= coldClusterStartSensor && sensorID < coldClusterStartSensor + 5)
            {
                reading.temperature = coldDist(globalGen);
            }

            if (sensorID == coldSpotSensor)
            {
                reading.temperature = coldDist(globalGen);
            }

            if (sensorID == coldMalfunctionSensor)
            {
                reading.temperature = coldDist(globalGen);
            }

            // Only one thread resets anomaly pattern to avoid race condition
            if (sensorID == 1)
            {
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
        // SensorStream.insert(reading); //insert into heap
        count++;
    }
}

void update_cold_counter(int sensorID, long long timestamp)
{
    bool found = false;

    // Check if sensorID exists in the vector
    for (auto &entry : cold_counter)
    {
        if (entry.first == sensorID)
        {
            entry.second.second += 1;       // Increment occurrence count
            entry.second.first = timestamp; // Update last timestamp
            found = true;
            break;
        }
    }
    // If sensorID is not found, add new entry
    if (!found) {
        cold_counter.push_back({sensorID, {timestamp, 1}});
    }
}

void removeExpiredReadings(long long now, MinMaxHeap heap) {
    while (!Buffer.empty() && now - Buffer.front().timestamp > VALID_MS) { // if buffer becomes greater than valid_ms
        SensorReading expired = Buffer.front(); //pop and delete from heap
        Buffer.pop_front();
        heap.delete_value(expired);  
    }
}
// Monitor thread
// all the alert signals/checks will go here
// void monitorReadings()
// {
//     std::set<std::string> loggedAlerts;
//     std::ofstream outfile("alert_logging.txt", std::ios::app);

//     MinMaxHeap Stream; // declare a heap
//     while (true)
//     {
//         // std::ofstream outfile("alert_logging.txt", std::ios::app);
//         SensorReading reading;
//         {
//             lock_guard<mutex> lock(queueMutex);
//             if (!readingQueue.empty())
//             {
//                 reading = readingQueue.front();
//                 readingQueue.pop();
//                 // reading.print(); // Replace this with heap insertion
//                 // for (int i = 0; i< Stream.get_size(); i++){
//                 //     if(Stream.arr

//                 // }
//                 Stream.replace(reading); // insert/replace into heap

//                 if (Stream.get_size() >= 1)
//                 {
//                     std::vector<SensorReading> top_min_1 = Stream.top_k_min(1);
//                     std::vector<SensorReading> top_max_1 = Stream.top_k_max(1);

//                     // ALERT if any threshold crossed:

//                     if (top_min_1[0].temperature < 15)
//                     {
//                         outfile << "[ALERT] Time: " << top_min_1[0].timestamp << " | Sensor: " << top_min_1[0].sensorID << " | Type: Lower Threshold Crossed | Temp: " << top_min_1[0].temperature << " C\n";
//                     }

//                     if (top_max_1[0].temperature > 48)
//                     {
//                         outfile << "[ALERT] Time: " << top_max_1[0].timestamp << " | Sensor: " << top_max_1[0].sensorID << " | Type: Upper Threshold Crossed | Temp: " << top_max_1[0].temperature << " C\n";
//                     }
//                 }
//                 // add all the different alert cases
//                 // replace if time is expired
//             }
//         }

//         // Isolated low/ high spike
//         if (Stream.get_size() > 5)
//         {

//             std::vector<SensorReading> clusterMin;
//             std::vector<SensorReading> clusterMax;
//             std::vector<SensorReading> top_min = Stream.top_k_min(5);
//             std::vector<SensorReading> top_max = Stream.top_k_max(5);
//             int AvgTemp;
//             // min alerts
//             std::vector<SensorReading> SensorIdsMin;
//             for (SensorReading i : top_min)
//             {
//                 SensorIdsMin.push_back(i);
//             }
//             std::sort(SensorIdsMin.begin(), SensorIdsMin.end(), [](const SensorReading &a, const SensorReading &b)
//                       { return a.sensorID < b.sensorID; });

//             for (int i = 0; i < SensorIdsMin.size() - 1; i++)
//             {
//                 // if SensorIds !=0
//                 if (i != 0 && i != SensorIdsMin.size() - 1)
//                 {
//                     if ((SensorIdsMin[i - 1].sensorID != (SensorIdsMin[i].sensorID) - 1) && (SensorIdsMin[i + 1].sensorID != (SensorIdsMin[i].sensorID) + 1))
//                     {
//                         outfile << "[ALERT] Time: " << SensorIdsMin[i].timestamp << " | Sensor: " << SensorIdsMin[i].sensorID << " | Type: Isolated Low Spike | Temp: " << SensorIdsMin[i].temperature << " C [NOTE] Neighbouring Sensors " << (SensorIdsMin[i].sensorID) - 1 << " and " << (SensorIdsMin[i].sensorID) + 1 << " are normal.\n";
//                     }
//                 }

//                 // cluster
//                 if (clusterMin.empty() || SensorIdsMin[i].sensorID == (clusterMin.back().sensorID) + 1)
//                 {
//                     clusterMin.push_back(SensorIdsMin[i]);
//                 }
//                 else
//                 {
//                     if (clusterMin.size() >= 3)
//                     {
//                         AvgTemp = 0;
//                         outfile << "[CRITICAL] Time: " << SensorIdsMin[i].timestamp << " | Sensors: ";

//                         for (int j = 0; j < clusterMin.size(); j++)
//                         {

//                             if (j == clusterMin.size() - 1)
//                             {
//                                 outfile << clusterMin[j].sensorID; // for the last line
//                             }
//                             else
//                             {
//                                 outfile << clusterMin[j].sensorID << ", ";
//                             }
//                             AvgTemp += clusterMin[j].temperature;
//                         }

//                         outfile << " | Type: Clustered Low Spike | Avg Temp: " << (AvgTemp) / clusterMin.size() << " C" << '\n';
//                     }
//                     clusterMin = {SensorIdsMin[i]};
//                 }

//                 if (clusterMin.size() >= 3)
//                 {
//                     AvgTemp = 0;
//                     outfile << "[CRITICAL] Time: " << SensorIdsMin[i].timestamp << " | Sensors: ";

//                     for (int j = 0; j < clusterMin.size(); j++)
//                     {

//                         if (j == clusterMin.size() - 1)
//                         {
//                             outfile << clusterMin[j].sensorID; // for the last line
//                         }
//                         else
//                         {
//                             outfile << clusterMin[j].sensorID << ", ";
//                         }
//                         AvgTemp += clusterMin[j].temperature;
//                     }

//                     outfile << " | Type: Clustered Low Spike | Avg Temp: " << (AvgTemp) / clusterMin.size() << " C" << '\n';
//                 }
//             }

//             // max alerts
//             std::vector<SensorReading> SensorIdsMax;
//             for (SensorReading i : top_max)
//             {
//                 SensorIdsMax.push_back(i);
//             }

//             std::sort(SensorIdsMax.begin(), SensorIdsMax.end(), [](const SensorReading &a, const SensorReading &b)
//                       { return a.sensorID > b.sensorID; });
//             for (int i = 0; i < SensorIdsMax.size() - 1; i++)
//             {
//                 // if SensorIds !=0
//                 if (i != 0 && i != SensorIdsMax.size() - 1)
//                 {
//                     if ((SensorIdsMax[i - 1].sensorID != (SensorIdsMax[i].sensorID) - 1) && (SensorIdsMax[i + 1].sensorID != (SensorIdsMax[i].sensorID) + 1))
//                     {
//                         outfile << "[ALERT] Time: " << SensorIdsMax[i].timestamp << " | Sensor: " << SensorIdsMax[i].sensorID << " | Type: Isolated High Spike | Temp: " << SensorIdsMax[i].temperature << " C [NOTE] Neighbouring Sensors " << (SensorIdsMax[i].sensorID) - 1 << " and " << (SensorIdsMax[i].sensorID) + 1 << " are normal. \n";
//                     }
//                 }
//                 // cluster
//                 if (clusterMax.empty() || SensorIdsMax[i].sensorID == clusterMax.back().sensorID + 1)
//                 {
//                     clusterMax.push_back(SensorIdsMax[i]);
//                 }
//                 else
//                 {
//                     if (clusterMax.size() >= 3)
//                     {
//                         AvgTemp = 0;
//                         outfile << "[CRITICAL] Time: " << SensorIdsMax[i].timestamp << " | Sensors: ";

//                         for (int j = 0; j < clusterMax.size(); j++)
//                         {

//                             if (j == clusterMax.size() - 1)
//                             {
//                                 outfile << clusterMax[j].sensorID; // for the last line
//                             }
//                             else
//                             {
//                                 outfile << clusterMax[j].sensorID << ", ";
//                             }
//                             AvgTemp += clusterMax[j].temperature;
//                         }

//                         outfile << " | Type: Clustered High Spike | Avg Temp: " << (AvgTemp) / clusterMax.size() << " C" << '\n';
//                     }
//                     clusterMax = {SensorIdsMax[i]};
//                 }

//                 if (clusterMax.size() >= 3)
//                 {
//                     AvgTemp = 0;
//                     outfile << "[CRITICAL] Time: " << SensorIdsMax[i].timestamp << " | Sensors: ";

//                     for (int j = 0; j < clusterMax.size(); j++)
//                     {

//                         if (j == clusterMax.size() - 1)
//                         {
//                             outfile << clusterMax[j].sensorID; // for the last line
//                         }
//                         else
//                         {
//                             outfile << clusterMax[j].sensorID << ", ";
//                         }
//                         AvgTemp += clusterMax[j].temperature;
//                     }

//                     outfile << " | Type: Clustered High Spike | Avg Temp: " << (AvgTemp) / clusterMax.size() << " C" << '\n';
//                 }
//             }
//             // COLD DELTECTION LOGIC HERE:::::
//             for (auto &reading : top_min)
//             {
//                 if (reading.temperature < AvgTemp - 10)
//                 {
//                     update_cold_counter(reading.sensorID, reading.timestamp);

//                     // Check if sensor has reported low temperature for ≥10 seconds
//                     for (auto &entry : cold_counter)
//                     {
//                         if (entry.first == reading.sensorID && entry.second.second >= 5)
//                         { // Assuming ~2 readings per second
//                             outfile << "[WARNING] Time: " << to_string(reading.timestamp) << " | Sensor: " << to_string(reading.sensorID) << " | Type: Cold Spot Detected | Temp: " << to_string(reading.temperature) + "\n";
//                             break;
//                         }
//                     }
//                 }
//                 else
//                 {
//                     // If temperature normalizes, remove sensor from tracking vector
//                     cold_counter.erase(remove_if(cold_counter.begin(), cold_counter.end(),
//                                                  [&](pair<int, pair<long long, int>> &entry)
//                                                  {
//                                                      return entry.first == reading.sensorID;
//                                                  }),
//                                        cold_counter.end());
//                 }
//             }
//         } // if size > 5 ends here

//         this_thread::sleep_for(milliseconds(50));
//     } // end of while
//     outfile.close();
// }

void monitorReadings()
{
    std::ofstream outfile("alert_logging.txt", std::ios::app);
    MinMaxHeap Stream;                  // declare a heap
    std::set<std::string> loggedAlerts; // set to store alert keys

    while (true)
    {
        SensorReading reading;
        {
            lock_guard<mutex> lock(queueMutex);
            if (!readingQueue.empty())
            {
                reading = readingQueue.front();
                readingQueue.pop();
                // reading.print(); // Replace this with heap insertion
                // for (int i = 0; i< Stream.get_size(); i++){
                //     if(Stream.arr

                // }

                Buffer.push_back(reading); //inser tinto buffer

                long long now = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count(); //gets the time as ms

                removeExpiredReadings(now, Stream); //removes anythign crossing 60 sec

                Stream.replace(reading); // insert/replace into heap

                if (Stream.get_size() >= 1)
                {
                    std::vector<SensorReading> top_min_1 = Stream.top_k_min(1);
                    std::vector<SensorReading> top_max_1 = Stream.top_k_max(1);

                    // ALERT if any threshold crossed:
                    std::string alertKeyMin = std::to_string(top_min_1[0].sensorID) + "_" +
                                              std::to_string(top_min_1[0].timestamp) + "_LOW";
                    if (top_min_1[0].temperature < 15 && loggedAlerts.find(alertKeyMin) == loggedAlerts.end())
                    {
                        outfile << "[ALERT] Time: " << top_min_1[0].timestamp << " | Sensor: " << top_min_1[0].sensorID
                                << " | Type: Lower Threshold Crossed | Temp: " << top_min_1[0].temperature << " C\n";
                        loggedAlerts.insert(alertKeyMin);
                    }

                    std::string alertKeyMax = std::to_string(top_max_1[0].sensorID) + "_" +
                                              std::to_string(top_max_1[0].timestamp) + "_HIGH";
                    if (top_max_1[0].temperature > 48 && loggedAlerts.find(alertKeyMax) == loggedAlerts.end())
                    {
                        outfile << "[ALERT] Time: " << top_max_1[0].timestamp << " | Sensor: " << top_max_1[0].sensorID
                                << " | Type: Upper Threshold Crossed | Temp: " << top_max_1[0].temperature << " C\n";
                        loggedAlerts.insert(alertKeyMax);
                    }
                }
            }
        }

        // Extended alerts
        if (Stream.get_size() > 5)
        {
            std::vector<SensorReading> clusterMin;
            std::vector<SensorReading> clusterMax;
            std::vector<SensorReading> top_min = Stream.top_k_min(5);
            std::vector<SensorReading> top_max = Stream.top_k_max(5);
            int AvgTemp;

            // Isolated & clustered min
            std::vector<SensorReading> SensorIdsMin = top_min;
            std::sort(SensorIdsMin.begin(), SensorIdsMin.end(), [](const SensorReading &a, const SensorReading &b)
                      { return a.sensorID < b.sensorID; });

            for (int i = 0; i < SensorIdsMin.size(); i++)
            {
                // Isolated
                if (i > 0 && i < SensorIdsMin.size() - 1)
                {
                    if ((SensorIdsMin[i - 1].sensorID != SensorIdsMin[i].sensorID - 1) &&
                        (SensorIdsMin[i + 1].sensorID != SensorIdsMin[i].sensorID + 1))
                    {
                        std::string isoKey = std::to_string(SensorIdsMin[i].sensorID) + "_" +
                                             std::to_string(SensorIdsMin[i].timestamp) + "_ISO_LOW";
                        if (loggedAlerts.find(isoKey) == loggedAlerts.end())
                        {
                            outfile << "[ALERT] Time: " << SensorIdsMin[i].timestamp << " | Sensor: " << SensorIdsMin[i].sensorID
                                    << " | Type: Isolated Low Spike | Temp: " << SensorIdsMin[i].temperature
                                    << " C [NOTE] Neighbouring Sensors " << (SensorIdsMin[i].sensorID - 1)
                                    << " and " << (SensorIdsMin[i].sensorID + 1) << " are normal.\n";
                            loggedAlerts.insert(isoKey);
                        }
                    }
                }

                // Cluster
                if (clusterMin.empty() || SensorIdsMin[i].sensorID == clusterMin.back().sensorID + 1)
                {
                    clusterMin.push_back(SensorIdsMin[i]);
                }
                else
                {
                    if (clusterMin.size() >= 3)
                    {
                        std::string clusterKey = "CLUSTER_LOW_" + std::to_string(clusterMin.front().timestamp) + "_";
                        for (auto &r : clusterMin)
                            clusterKey += std::to_string(r.sensorID) + "_";

                        if (loggedAlerts.find(clusterKey) == loggedAlerts.end())
                        {
                            AvgTemp = 0;
                            outfile << "[CRITICAL] Time: " << SensorIdsMin[i].timestamp << " | Sensors: ";
                            for (int j = 0; j < clusterMin.size(); j++)
                            {
                                outfile << clusterMin[j].sensorID;
                                if (j != clusterMin.size() - 1)
                                    outfile << ", ";
                                AvgTemp += clusterMin[j].temperature;
                            }
                            outfile << " | Type: Clustered Low Spike | Avg Temp: "
                                    << (AvgTemp / clusterMin.size()) << " C\n";
                            loggedAlerts.insert(clusterKey);
                        }
                    }
                    clusterMin = {SensorIdsMin[i]};
                }
            }

            // Repeat similar logic for MAX
            std::vector<SensorReading> SensorIdsMax = top_max;
            std::sort(SensorIdsMax.begin(), SensorIdsMax.end(), [](const SensorReading &a, const SensorReading &b)
                      { return a.sensorID > b.sensorID; });

            for (int i = 0; i < SensorIdsMax.size(); i++)
            {
                if (i > 0 && i < SensorIdsMax.size() - 1)
                {
                    if ((SensorIdsMax[i - 1].sensorID != SensorIdsMax[i].sensorID - 1) &&
                        (SensorIdsMax[i + 1].sensorID != SensorIdsMax[i].sensorID + 1))
                    {
                        std::string isoKey = std::to_string(SensorIdsMax[i].sensorID) + "_" +
                                             std::to_string(SensorIdsMax[i].timestamp) + "_ISO_HIGH";
                        if (loggedAlerts.find(isoKey) == loggedAlerts.end())
                        {
                            outfile << "[ALERT] Time: " << SensorIdsMax[i].timestamp << " | Sensor: " << SensorIdsMax[i].sensorID
                                    << " | Type: Isolated High Spike | Temp: " << SensorIdsMax[i].temperature
                                    << " C [NOTE] Neighbouring Sensors " << (SensorIdsMax[i].sensorID - 1)
                                    << " and " << (SensorIdsMax[i].sensorID + 1) << " are normal. \n";
                            loggedAlerts.insert(isoKey);
                        }
                    }
                }

                if (clusterMax.empty() || SensorIdsMax[i].sensorID == clusterMax.back().sensorID + 1)
                {
                    clusterMax.push_back(SensorIdsMax[i]);
                }
                else
                {
                    if (clusterMax.size() >= 3)
                    {
                        std::string clusterKey = "CLUSTER_HIGH_" + std::to_string(clusterMax.front().timestamp) + "_";
                        for (auto &r : clusterMax)
                            clusterKey += std::to_string(r.sensorID) + "_";

                        if (loggedAlerts.find(clusterKey) == loggedAlerts.end())
                        {
                            AvgTemp = 0;
                            outfile << "[CRITICAL] Time: " << SensorIdsMax[i].timestamp << " | Sensors: ";
                            for (int j = 0; j < clusterMax.size(); j++)
                            {
                                outfile << clusterMax[j].sensorID;
                                if (j != clusterMax.size() - 1)
                                    outfile << ", ";
                                AvgTemp += clusterMax[j].temperature;
                            }
                            outfile << " | Type: Clustered High Spike | Avg Temp: "
                                    << (AvgTemp / clusterMax.size()) << " C\n";
                            loggedAlerts.insert(clusterKey);
                        }
                    }
                    clusterMax = {SensorIdsMax[i]};
                }
            }

            // Cold spot logic
            for (auto &reading : top_min)
            {
                if (reading.temperature < AvgTemp - 10)
                {
                    update_cold_counter(reading.sensorID, reading.timestamp);

                    for (auto &entry : cold_counter)
                    {
                        if (entry.first == reading.sensorID && entry.second.second >= 5)
                        {
                            std::string coldKey = "COLD_" + std::to_string(reading.sensorID) + "_" + std::to_string(reading.timestamp);
                            if (loggedAlerts.find(coldKey) == loggedAlerts.end())
                            {
                                outfile << "[WARNING] Time: " << reading.timestamp
                                        << " | Sensor: " << reading.sensorID
                                        << " | Type: Cold Spot Detected | Temp: " << reading.temperature << "\n";
                                loggedAlerts.insert(coldKey);
                            }
                            break;
                        }
                    }
                }
                else
                {
                    cold_counter.erase(remove_if(cold_counter.begin(), cold_counter.end(),
                                                 [&](pair<int, pair<long long, int>> &entry)
                                                 {
                                                     return entry.first == reading.sensorID;
                                                 }),
                                       cold_counter.end());
                }
            }
        }

        this_thread::sleep_for(milliseconds(50));
    }

    outfile.close();
}

// void removeExpiredReadings(long long now, MinMaxHeap heap) {
//     while (!Buffer.empty() && now - Buffer.front().timestamp > VALID_MS) {
//         SensorReading expired = Buffer.front();
//         Buffer.pop_front();
//         heap.delete_value(expired);  
//     }
// }

int main()
{
    // Start sensor threads
    for (int i = 1; i <= numSensors; ++i)
    {
        thread(sensorStream, i).detach();
    }
    // for (auto s : counters)
    // {
    //     s.first = s.second = 0;
    // }
    // Start monitor thread
    monitorReadings();
    return 0;
}
