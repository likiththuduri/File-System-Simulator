
# File System Simulator in C

## Project Description

The File System Simulator is a menu-driven application developed using the C programming language. It demonstrates basic file management operations, disk block allocation techniques, and file permission management concepts used in operating systems.

The project simulates a virtual disk containing 20 blocks and supports three file allocation methods: contiguous allocation, linked allocation, and indexed allocation.

## Features

- Create and delete files
- Write and read file contents
- Contiguous file allocation
- Linked file allocation
- Indexed file allocation
- Display file details and allocated blocks
- Display virtual disk usage
- Set file permissions
- Check read, write, and execute permissions
- Validate user input and handle allocation failures

## File Allocation Methods

### 1. Contiguous Allocation
Allocates consecutive disk blocks to a file. This method provides simple and efficient access to the allocated blocks.

### 2. Linked Allocation
Allocates free disk blocks that do not need to be consecutive. The simulator maintains the allocated block numbers for each file.

### 3. Indexed Allocation
Reserves one index block and additional blocks for file data. The index block is stored separately from the data blocks.

## Technologies Used

- C Programming Language
- Structures
- Arrays
- Functions
- String Handling
- Standard Input/Output
- Operating Systems Concepts

## Project Configuration

- Disk Capacity: 20 blocks
- Maximum Number of Files: 20
- Maximum File Name Length: 63 characters
- Maximum Stored Content: 255 characters
- Default File Permission: `rw-`

## How to Compile and Run

### Prerequisites
Install a C compiler such as GCC.

### Compile

```bash
gcc -std=c11 -Wall -Wextra -pedantic filesystem.c -o filesystem
```

### Run on Windows

```powershell
.\filesystem.exe
```

### Run on Linux

```bash
./filesystem
```

## Main Menu

The application provides the following operations:

1. Create File
2. Write File
3. Read File
4. Delete File
5. Display Files
6. Display Disk
7. Set File Permissions
8. Check Access
9. Exit

## Output Screenshots

### Output 1

![Output 1](Screenshot%202026-10-12%20100415.png)

### Output 2

![Output 2](Screenshot%202026-10-12%20100426.png)

### Output 3

![Output 3](Screenshot%202026-10-12%20100446.png)

### Output 4

![Output 4](Screenshot%202026-10-12%20100459.png)

### Output 5

![Output 5](Screenshot%202026-10-12%20100511.png)

### Output 6

![Output 6](Screenshot%202026-10-12%20100521.png)

### Output 7

![Output 7](Screenshot%202026-10-12%20100533.png)

## Limitations

- This project simulates a file system in memory.
- Files and their contents are not permanently stored after the program exits.
- Execute permission is checked but does not execute an actual program.
- File contents are stored in a fixed-size character array.

## Learning Outcomes

- Understanding file allocation methods in operating systems
- Learning how disk blocks can be allocated and released
- Implementing file management operations using C
- Understanding basic file access permissions
- Practicing structures, arrays, functions, and string handling

## Author

B.Tech Computer Science and Engineering Student

## License

This project is intended for educational and academic purposes.
