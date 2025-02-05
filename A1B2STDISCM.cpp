#include <iostream>
#include <thread>
#include <fstream>
#include <chrono>
#include <vector>
#include <sstream>
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

void checkDivisibility(int n, int divisor, bool& isPrimeFlag, int threadID) {
    bool divisible = (n % divisor == 0);

    if (divisible) {
        isPrimeFlag = false;
    }

    {
        lock_guard<mutex> lock(printMutex);
        cout << "Thread " << threadID << " | Time Stamp: " << getCurrentTimestamp()
             << " | Checking: " << n << " / " << divisor
             << " | Result: " << (divisible ? "Divisible" : "Not Divisible")
             << endl;
    }
}

void processNumber(int n, int numThreads) {
    if (n <= 1) return;

    bool isPrimeFlag = true;
    vector<thread> threads;
    int threadCounter = 1;  // Start thread ID from 1

    // First, check divisibility by 2
    if (n % 2 == 0 && n > 2) {
        isPrimeFlag = false;
        lock_guard<mutex> lock(printMutex);
        cout << "Thread 0 | Time Stamp: " << getCurrentTimestamp()
             << " | Checking: " << n << " / 2"
             << " | Result: Divisible (by 2)" << endl;
    }

    // Now, check divisibility by odd numbers starting from 3
    for (int i = 3; i <= n / 2; i += 2) {
        if (threadCounter > numThreads) {
            threadCounter = 1;  // Reset threadCounter if it exceeds the number of threads
        }
        threads.emplace_back(checkDivisibility, n, i, std::ref(isPrimeFlag), threadCounter);
        threadCounter++;
    }

    // Join all threads
    for (auto& t : threads) {
        t.join();
    }

    // After divisibility checks, if still marked prime, print "PRIME FOUND"
    if (isPrimeFlag) {
        lock_guard<mutex> lock(printMutex);
        cout << "PRIME FOUND: " << n << endl;
    }
}

int main() {
    int xNumThreads, yEndRange;

    cout << "Variant 2 (A1 + B2)" << endl;
    cout << "A1 - Print immediately (Thread id and time stamp should be included)" << endl;
    cout << "B2 - The search is linear but the threads are for divisibility testing of individual numbers." << endl;

    // Read config file
    if (!readConfigFile(xNumThreads, yEndRange)) {
        cerr << "Configuration error. Exiting." << endl;
        return 1;
    }

    auto startTime = getCurrentTimestamp();
    cout << "Start Time: " << startTime << endl << endl;

    // Process numbers in the specified range
    for (int i = 2; i <= yEndRange; ++i) {
        processNumber(i, xNumThreads);
    }

    auto endTime = getCurrentTimestamp();
    cout << "End Time: " << endTime << endl;

    return 0;
}
