#include <iostream>
#include <thread>
#include <fstream>
#include <chrono>
#include <vector>
#include <string>
#include <mutex>

using namespace std;
mutex printMutex;

bool readConfigFile(int& xNumThreads, int& yEndRange) {
    const string fileName = "config.txt";
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error: Unable to open the configuration file." << endl;
        return false;
    }

    string line;
    while (getline(file, line)) {
        size_t pos = line.find('=');
        if (pos == string::npos) {
            cerr << "Error: Invalid configuration format." << endl;
            return false;
        }

        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);

        if (key == "x") {
            try {
                xNumThreads = stoi(value);
                if (xNumThreads < 1) {
                    cerr << "Error: x must be at least 1." << endl;
                    return false;
                }
            } catch (...) {
                cerr << "Error: Invalid value for x." << endl;
                return false;
            }
        } else if (key == "y") {
            try {
                yEndRange = stoi(value);
                if (yEndRange < 2) {
                    cerr << "Error: y must be at least 2." << endl;
                    return false;
                }
            } catch (...) {
                cerr << "Error: Invalid value for y." << endl;
                return false;
            }
        } else {
            cerr << "Error: Unknown configuration key." << endl;
            return false;
        }
    }

    if (xNumThreads > yEndRange) {
        cerr << "Warning: Number of threads exceeds the range size. Adjusting x to " << yEndRange << "." << endl;
        xNumThreads = yEndRange;
    }

    return true;
}

bool isPrime(int n) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (int i = 5; i * i <= n; i += 6) { // Skip even numbers
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

std::string getCurrentTimestamp() {
    time_t currentTime = time(nullptr);
    tm localTime;
#ifdef _WIN32
    localtime_s(&localTime, &currentTime);
#else
    localtime_r(&currentTime, &localTime);
#endif

    char timeBuffer[100];
    strftime(timeBuffer, sizeof(timeBuffer), "%m/%d/%Y, %I:%M:%S %p", &localTime);

    return std::string(timeBuffer);
}

void searchPrimeNumbers(int startRange, int endRange, int threadID) {
    for (int i = startRange; i <= endRange; i++) {
        if (isPrime(i)) {
            lock_guard<mutex> lock(printMutex);
            cout << "Thread ID: " << threadID
                 << "\nTime Stamp: " << getCurrentTimestamp()
                 << "\nPrime: " << i << "\n" << endl;
        }
    }
}

void createThreads(int xNumThreads, int yEndRange) {
    vector<thread> threads;
    int rangePerThread = yEndRange / xNumThreads;
    int startRange = 1;

    // Adjust the division of range if it's not perfectly divisible
    int remainder = yEndRange % xNumThreads;
    
    for (int i = 0; i < xNumThreads; i++) {
        // If it's the last thread or the thread has a remainder of the range, it will get the extra range
        int endRange = startRange + rangePerThread - 1;
        
        if (i == xNumThreads - 1) {
            endRange = yEndRange; // Ensure the last thread gets the remaining range
        } else if (i < remainder) {
            endRange += 1; // Distribute remainder among the first threads
        }

        threads.emplace_back(searchPrimeNumbers, startRange, endRange, i + 1); // Passing thread ID
        startRange = endRange + 1;  // Update startRange for the next thread
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }
}

int main() {
    int xNumThreads, yEndRange;

    cout << "Variant 1 (A1 + B1)" << endl;
    cout << "A1 - Print immediately (Thread id and time stamp should be included)" << endl;
    cout << "B1 - Straight division of search range. (ie for 1 - 1000 and 4 threads the division will be 1-250, 251-500, and so forth)" << endl;

    if (!readConfigFile(xNumThreads, yEndRange)) {
        cerr << "Configuration error. Exiting." << endl;
        return 1;
    }

    auto startTime = getCurrentTimestamp();
    cout << "Start Time: " << startTime << endl << endl;

    createThreads(xNumThreads, yEndRange);

    auto endTime = getCurrentTimestamp();
    cout << "End Time: " << endTime << endl;

    return 0;
}
