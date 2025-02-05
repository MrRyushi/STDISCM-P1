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
#include <condition_variable>
#include <atomic>
#include <cmath>
#include <queue>

std::mutex printMutex;
std::mutex taskMutex;
std::condition_variable cv;
std::atomic<bool> isPrimeFlag;
bool stopThreads = false;
std::queue<std::pair<int, int>> taskQueue; // Stores (n, divisor)

void printTimestamp(int threadID, int n, int divisor, bool isDivisible) {
    auto now = std::chrono::system_clock::now();
    auto timeStamp = std::chrono::system_clock::to_time_t(now);
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

    std::tm timeInfo;
    localtime_s(&timeInfo, &timeStamp);
    char timeBuffer[9];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeInfo);

    //std::lock_guard<std::mutex> lock(printMutex);
    /*std::cout << "Thread " << threadID << " | Time: " << timeBuffer << ":" << millis
              << " | Checking: " << n << " / " << divisor
              << " | " << (isDivisible ? "Divisible" : "Not Divisible") << std::endl;*/
}

// Worker function for checking divisibility
void worker(int threadID) {
    while (true) {
        std::pair<int, int> task;

        {
            std::unique_lock<std::mutex> lock(taskMutex);
            cv.wait(lock, [] { return !taskQueue.empty() || stopThreads; });

            if (stopThreads && taskQueue.empty()) return; // Stop thread

            task = taskQueue.front();
            taskQueue.pop();
        }

        int n = task.first;
        int divisor = task.second;

        if (n % divisor == 0) {
            isPrimeFlag = false; // Mark non-prime
        }

        printTimestamp(threadID, n, divisor, (n % divisor == 0));
    }
}

// Process a single number for primality
void processNumber(int n, int numThreads) {
    if (n <= 1) return;

    isPrimeFlag = true;
    int sqrtN = std::sqrt(n);

    // Assign divisibility tasks to available threads
    for (int i = 2; i <= sqrtN; ++i) {
        {
            std::lock_guard<std::mutex> lock(taskMutex);
            taskQueue.push({n, i});
        }
        cv.notify_one();
    }

    // Allow time for all threads to complete tasks
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // If no divisor was found, print as prime
    if (isPrimeFlag) {
        std::lock_guard<std::mutex> lock(printMutex);
        std::cout << "PRIME FOUND: " << n << std::endl;
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

    configFile.close();

    // Start worker threads
    std::vector<std::thread> workers;
    for (int i = 0; i < xNumThreads; ++i) {
        workers.emplace_back(worker, i + 1);
    }

    auto start = std::chrono::system_clock::now();

    for (int i = 2; i <= yNumber; ++i) {
        processNumber(i, xNumThreads);
    }

    // Stop threads
    {
        std::lock_guard<std::mutex> lock(taskMutex);
        stopThreads = true;
    }
    cv.notify_all();

    for (auto &t : workers) {
        t.join();
    }

    // End timing
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    // Print results
    std::cout << "Elapsed time: " << elapsed_seconds.count() << "s" << std::endl;

    return 0;
}