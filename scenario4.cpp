/**
 * A2. Wait until all threads are done then print everything and
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

std::mutex printMutex;
std::mutex primeCheckMutex;
bool isPrimeFlag = true; // Shared flag for prime checking
std::vector<std::string> outputs;

void checkDivisibility(int n, int divisor, int threadID) {
    auto startTime = std::chrono::system_clock::now();
    auto timeStamp = std::chrono::system_clock::to_time_t(startTime);
    
    std::tm timeInfo;
    localtime_s(&timeInfo, &timeStamp);
    char timeBuffer[9];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeInfo);
    
    // Check divisibility
    if (n % divisor == 0) {
        std::lock_guard<std::mutex> lock(primeCheckMutex);
        isPrimeFlag = false; // If divisible, number is not prime
    }
    
    // Store the output for later printing
    {
        std::lock_guard<std::mutex> lock(printMutex);
        outputs.push_back("Thread " + std::to_string(threadID) + " | Timestamp: " + timeBuffer
                  + " | Checking: " + std::to_string(n) + " / " + std::to_string(divisor)
                  + " | Result: " + (n % divisor == 0 ? "Divisible" : "Not Divisible"));
    }
}

void processNumber(int n, int numThreads) {
    if (n <= 1) return; // Ignore 0 and 1
    
    isPrimeFlag = true; // Reset flag for each number
    std::vector<std::thread> threads;
    
    for (int i = 2; i <= numThreads + 1 && i * i <= n; ++i) {
        threads.emplace_back(checkDivisibility, n, i, i - 1);
    }
    
    for (auto &t : threads) {
        t.join();
    }
    
    // 
    if (isPrimeFlag) {
        std::lock_guard<std::mutex> lock(printMutex);
        outputs.push_back("PRIME FOUND: " + std::to_string(n));
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
    
    auto start = std::chrono::system_clock::now();
    
    for (int i = 2; i <= yNumber; ++i) {
        processNumber(i, xNumThreads);
    }
    
    std::cout << "All threads done!" << std::endl;
    // Print all outputs
    for (const auto &output : outputs) {
        std::cout << output << std::endl;
    }

    // Simulate some computation
    std::this_thread::sleep_for(std::chrono::milliseconds(123));

    auto end = std::chrono::system_clock::now();

    // Calculate elapsed time
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
    
    
    configFile.close();
    return 0;
}