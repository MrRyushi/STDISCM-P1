/**
 * A1. PRINT IMMEDIATELY and
 * B1. Straight division of search range.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <iomanip>
#include <algorithm>

std::mutex printMutex;

bool isPrime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

void searchPrimeNumbers(int start, int end, int id) {
    auto startTime = std::chrono::system_clock::now();
    auto duration = startTime.time_since_epoch();
    
    // Extract seconds and milliseconds
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
    std::time_t timeStamp = std::chrono::system_clock::to_time_t(startTime);
    std::tm timeInfo;
    localtime_s(&timeInfo, &timeStamp);

    char timeBuffer[9];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeInfo);  

    // Print immediately
    for (int i = start; i <= end; ++i) {
        if (isPrime(i)) {
            std::lock_guard<std::mutex> lock(printMutex);
            std::cout << "Thread ID: " << id
                << " | Timestamp: " << timeBuffer << ":" << std::setfill('0') << std::setw(3) << millis
                << " | Prime: " << i << std::endl;
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

    std::vector<std::thread> threads;
    int rangeSize = yNumber / xNumThreads;

    auto start = std::chrono::system_clock::now();

    // get the start and end index for each thread
    for (int i = 0; i < xNumThreads; ++i) {
        int start = i * rangeSize + 1;
        int end = (i == xNumThreads - 1) ? yNumber : (i + 1) * rangeSize;
        threads.emplace_back(searchPrimeNumbers, start, end, i);
    }

    for (auto& t : threads) {
        t.join();
    }


    auto end = std::chrono::system_clock::now();
    printStartAndEnd(start, end);
    configFile.close();

    return 0;
}