// Monitor thread
// all the alert signals/checks will go here
// void monitorReadings()
// {
//     std::set<std::string> alert_list;
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
