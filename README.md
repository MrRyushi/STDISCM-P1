# Prime Number Search with Multithreading

## Problem Set for STDISCM Course

### Task
Write a program that creates an `x` number of threads to search for prime numbers up to a given `y` number. The values `x` (number of threads) and `y` (search range) should be configurable in a separate configuration file.

### Requirements

- **Different Printing Variations**
  - Print immediately (Include thread ID and timestamp)
  - Wait until all threads are done, then print everything

- **Different Task Division Schemes**
  - **Straight Division**: The search range is divided into equal sections among threads. (e.g., for range `1-1000` and `4` threads: `1-250`, `251-500`, `501-750`, `751-1000`).
  - **Linear Search with Divisibility Testing**: Threads work in parallel to test divisibility of individual numbers.

### Folder Structure and Variations
The source code is divided into four variations based on different printing and task division methods:

| Folder         | Printing Style       | Task Division Scheme |
|---------------|----------------------|-----------------------|
| **variation1** | Print immediately   | Straight division     |
| **variation2** | Print immediately   | Linear divisibility testing |
| **variation3** | Wait until all threads finish | Straight division     |
| **variation4** | Wait until all threads finish | Linear divisibility testing |

### How to Run the Code
Each variation contains a `cpp` file (e.g., `variation1.cpp`, `variation2.cpp`, etc.). To compile and run the programs, follow these steps in a terminal:

1. Navigate to the corresponding variation folder. For example, for `variation1`:
   ```sh
   cd variation1
   ```
2. Compile the C++ file using g++:
   ```sh
   g++ -o variation1 variation1.cpp -pthread
   ```
3. Run the executable:
   ```sh
   ./variation1
   ```

Repeat these steps for each variation by changing the folder name and file name accordingly.

