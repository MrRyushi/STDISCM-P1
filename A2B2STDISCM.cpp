#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <mutex>
#include <sstream>
#include <string>
#include <chrono>

using namespace std;

mutex primeCheckMutex;  // Mutex for synchronizing access to prime numbers and the flag
mutex printMutex;  // Mutex for synchronizing console output
vector<int> primeNumbers;  // Shared resource to store primes

bool readConfigFile(int& xNumThreads, int& yEndRange) {
    ifstream file("config.txt");
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
            xNumThreads = stoi(value);
        } else if (key == "y") {
            yEndRange = stoi(value);
        } else {
            cerr << "Error: Unknown configuration key." << endl;
            return false;
        }
    }
    return true;
}

void checkDivisibility(int n, int startDiv, int endDiv, bool& isPrimeFlag, int threadId) {
    // Print when the thread starts checking
    {
        lock_guard<mutex> lock(printMutex);  // Synchronize the console output
        cout << "Thread " << threadId << " starting to check divisibility for number " << n
             << " in range [" << startDiv << ", " << endDiv << "]" << endl;
    }

    // Check divisibility within the range [startDiv, endDiv]
    for (int divisor = startDiv; divisor <= endDiv; ++divisor) {
        if (n % divisor == 0) {
            isPrimeFlag = false;
            break;
        }
    }

    // Lock and print when the thread finishes checking
    {
        lock_guard<mutex> lock(printMutex);  // Synchronize the console output
        cout << "Thread " << threadId << " finished checking divisibility for number " << n
             << " in range [" << startDiv << ", " << endDiv << "]" << endl;
    }
}

void processNumbers(int xNumThreads, int yEndRange) {
    for (int i = 2; i <= yEndRange; i++) {
        bool isPrimeFlag = true;  // Reset the flag for each number

        vector<thread> threads;
        int rangeSize = (i - 1) / xNumThreads + 1;  // Divide the divisor range into xNumThreads parts

        // Create threads and distribute the divisor checking range
        for (int t = 0; t < xNumThreads; ++t) {
            int startDiv = t * rangeSize + 2;  // Start divisor for this thread
            int endDiv = min((t + 1) * rangeSize + 1, i - 1);  // End divisor for this thread

            if (startDiv <= i - 1) {
                threads.emplace_back(checkDivisibility, i, startDiv, endDiv, ref(isPrimeFlag), t + 1);
            }
        }

        // Wait for all threads to finish checking the divisibility
        for (auto& t : threads) {
            t.join();
        }

        // Once all threads finish, if the number is still prime, we store it
        if (isPrimeFlag) {
            lock_guard<mutex> lock(primeCheckMutex);  // Lock when modifying the shared resource
            primeNumbers.push_back(i);
        }
    }
}

string getCurrentTimestamp() {
    time_t currentTime = time(nullptr);
    tm localTime;
#ifdef _WIN32
    localtime_s(&localTime, &currentTime);
#else
    localtime_r(&currentTime, &localTime);
#endif
    char timeBuffer[100];
    strftime(timeBuffer, sizeof(timeBuffer), "%m/%d/%Y, %I:%M:%S %p", &localTime);
    return string(timeBuffer);
}

int main() {
    int xNumThreads, yEndRange;

    cout << "Variant 2 (A2 + B2)" << endl;
    cout << "A2 - Wait until all threads are done then print everything" << endl;
    cout << "B2 - The search is linear but the threads are for divisibility testing of individual numbers." << endl;

    if (!readConfigFile(xNumThreads, yEndRange)) {
        cerr << "Configuration error. Exiting." << endl;
        return 1;
    }

    auto startTime = getCurrentTimestamp();
    cout << "Start Time: " << startTime << endl << endl;

    // Process numbers concurrently and store primes
    processNumbers(xNumThreads, yEndRange);

    auto endTime = getCurrentTimestamp();

    cout << "Primes Found:" << endl;
    for (int prime : primeNumbers) {
        cout << prime << " ";
    }
    cout << "\n\nEnd Time: " << endTime << endl;

    return 0;
}
