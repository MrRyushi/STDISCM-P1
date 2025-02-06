/**
 * A1. PRINT IMMEDIATELY and
 * B2. The search is linear but the threads are for divisibility testing of individual numbers.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <iomanip>
#include <algorithm>
#include <cmath>

std::mutex printMutex;
std::mutex primeCheckMutex;
bool isPrimeFlag = true; // Shared flag for prime checking

void checkDivisibility(int n, int divisor, int threadID) {   
    if (n % divisor == 0) {
        std::lock_guard<std::mutex> lock(primeCheckMutex);
        isPrimeFlag = false;
        return;
    }
}

void processNumber(int n, int numThreads) {
    if (n <= 1) return;

    isPrimeFlag = true;
    std::vector<std::thread> threads;
    
    int limit = static_cast<int>(sqrt(n));
    int threadIndex = 0; // To keep track of thread index correctly

    // this is to check divisibility of n by all numbers from 2 to sqrt(n) only
    for (int i = 2; i <= limit; ++i) {
        threads.emplace_back(checkDivisibility, n, i, threadIndex++);

        // this is to join the threads if the number of threads is more than the required number
        if (threads.size() >= static_cast<size_t>(numThreads)) {
            for (auto &t : threads) t.join();
            threads.clear();
            threadIndex = 0; 
        }
    }

    // Join any remaining threads
    for (auto &t : threads) t.join();

    // Print "Prime found!" message if still prime
    if (isPrimeFlag) {
        auto startTime = std::chrono::system_clock::now();
        auto timeStamp = std::chrono::system_clock::to_time_t(startTime);
        auto duration = startTime.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

        std::tm timeInfo;
        localtime_s(&timeInfo, &timeStamp);
        char timeBuffer[9];
        std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeInfo);

        // print immediately
        {
            std::lock_guard<std::mutex> lock(printMutex);
            std::cout << "Thread " << threadIndex << " | Time: " << timeBuffer << ":" << millis
                      << " | Prime found! " << n << std::endl;
        }
    }
}



bool isNumValid(std::string value) {
    // Trim leading/trailing spaces
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);

    // Check if value is a valid number
    if (value.empty() || !std::all_of(value.begin(), value.end(), ::isdigit)) {
        std::cerr << "Error: Invalid input!" << std::endl;
        return false;
    }

    return true;
}

void printStartAndEnd(std::chrono::time_point<std::chrono::system_clock> start, std::chrono::time_point<std::chrono::system_clock> end) {
    std::chrono::duration<double> elapsed_seconds = end - start;

    // Extract milliseconds
    auto start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch()) % 1000;
    auto end_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end.time_since_epoch()) % 1000;

    // Convert to time_t for readable format
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    // Print timestamps with milliseconds
    std::cout << "Started computation at: " 
              << std::put_time(std::localtime(&start_time), "%H:%M:%S") 
              << ":" << std::setfill('0') << std::setw(3) << start_ms.count()
              << std::endl;

    std::cout << "Finished computation at: " 
              << std::put_time(std::localtime(&end_time), "%H:%M:%S") 
              << ":" << std::setfill('0') << std::setw(3) << end_ms.count()
              << std::endl;

    std::cout << "Elapsed time: " 
              << elapsed_seconds.count() << "s" 
              << std::endl;

}

int main()
{
    // Open file for reading
    std::ifstream configFile("config.txt");

    // Check if file was opened successfully
    if (!configFile) {
        std::cout << "Error: Could not open the file!" << std::endl;
        return 1;
    }

    int xNumThreads = 0, yNumber = 0;
    std::string line;

    // validate the input from config file
    while (std::getline(configFile, line)) {
        if (line.find("x=") != std::string::npos) {
            std::string value = line.substr(line.find('=') + 1);
            
            if (isNumValid(value)) {
                // Convert string to integer
                xNumThreads = std::stoi(value);    
            } else {
                return 1;
            }
            
        }

        else if (line.find("y=") != std::string::npos) {
            std::string value = line.substr(line.find('=') + 1);
            
            if (isNumValid(value)) {
                // Convert string to integer
                yNumber = std::stoi(value);
            } else {
                return 1;
            }
        }
    }
    
    auto start = std::chrono::system_clock::now();
    
    for (int i = 2; i <= yNumber; ++i) {
        processNumber(i, xNumThreads);
    }
    
    std::cout << "All threads done!" << std::endl;

    auto end = std::chrono::system_clock::now();
    printStartAndEnd(start, end);
    
    configFile.close();
    return 0;
}