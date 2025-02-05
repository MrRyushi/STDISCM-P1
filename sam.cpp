#include <iostream>
#include <thread>
#include <fstream>
#include <ctime>
#include <chrono>
#include <vector>
#include <mutex>
#include <sstream>
#include <string>

using namespace std;

mutex printMutex;

// Returns true if the configuration was successfully read, false otherwise.
bool readConfigFile(int& xNumThreads, int& yEndRange) {
    const string fileName = "C:\\Users\\jptma\\Documents\\GitHub\\STDISCM P1\\config.txt";
    ifstream file(fileName);

    if (!file.is_open()) {
        cerr << "Error: Unable to open the configuration file." << endl;
        return false;
    }

    // Read the entire line from the file.
    string line;
    if (!getline(file, line)) {
        cerr << "Error: The configuration file is empty." << endl;
        return false;
    }

    istringstream iss(line);
    int x, y;

    // If line does not contain integer values
    if (!(iss >> x >> y)) {
        cerr << "Error: The configuration file must contain two integer values." << endl;
        return false;
    }

    // Check if there are extra tokens after the two numbers
    if (iss >> std::ws && !iss.eof()) {
        cerr << "Error: The configuration file contains extra data beyond the two numbers." << endl;
        return false;
    }

    // Successfully read exactly two numbers.
    xNumThreads = x;
    yEndRange = y;

    //cout << "X = number of threads" << endl;
    //cout << "Y = end range of numbers to search" << endl;
    cout << "Configuration: X - " << xNumThreads << ", Y - " << yEndRange << endl;
    cout << endl;
    return true;
}

bool isPrime(int n) {
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i <= n / 2; i++) {
        if (n % i == 0) {
            return false;
        }
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

void searchPrimeNumbers(int startRange, int endRange) {
    for (int i = startRange; i <= endRange; i++) {
        if (isPrime(i)) {
            lock_guard<mutex> guard(printMutex);  // Lock for synchronized output
            cout << "Thread ID: " << this_thread::get_id()
                << "\nTime Stamp: " << getCurrentTimestamp()
                << "\nPrime: " << i << "\n" << endl;
        }
    }
}

void createThreads(int xNumThreads, int yEndRange) {
    vector<thread> threads;
    int rangePerThread = yEndRange / xNumThreads;  // Divide the range among threads

    for (int i = 0; i < xNumThreads; i++) {
        int startRange = i * rangePerThread + 1;
        int endRange = (i == xNumThreads - 1) ? yEndRange : (i + 1) * rangePerThread;

        threads.push_back(thread(searchPrimeNumbers, startRange, endRange));
    }

    for (auto& t : threads) {
        t.join();
    }
}

int main() {
    int xNumThreads, yEndRange;

    cout << "Variant 1" << endl;
    cout << "A1 - Print immediately (Thread id and time stamp should be included)" << endl;
    cout << "B1 - Straight division of search range. (ie for 1 - 1000 and 4 threads the division will be 1-250, 251-500, and so forth)" << endl;

    // If configuration reading fails, exit immediately.
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
