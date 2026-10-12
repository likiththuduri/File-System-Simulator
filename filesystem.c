
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISK_SIZE 20
#define MAX_FILES 20
#define MAX_NAME 64
#define MAX_CONTENT 256
#define MAX_BLOCKS 20

typedef struct {
    char name[MAX_NAME];
    int size;
    char allocation[16];
    int blocks[MAX_BLOCKS];
    int block_count;
    int index_block;
    char permission[4];
    char content[MAX_CONTENT];
    int active;
} File;

char disk[DISK_SIZE][MAX_NAME];
File files[MAX_FILES];
int file_count = 0;

/* Safely read a line of input */
int read_line(const char *prompt, char *buffer, size_t capacity) {
    size_t len;
    int ch;

    printf("%s", prompt);

    if (fgets(buffer, (int)capacity, stdin) == NULL)
        return 0;

    len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* Discard excess input */
        }
    }

    return 1;
}

/* Safely read an integer */
int read_integer(const char *prompt, int *value) {
    char line[100];
    char extra;

    if (!read_line(prompt, line, sizeof(line)))
        return 0;

    if (sscanf(line, " %d %c", value, &extra) != 1)
        return -1;

    return 1;
}

/* Initialize disk and file table */
void initialize_disk(void) {
    int i;

    for (i = 0; i < DISK_SIZE; i++)
        strcpy(disk[i], "FREE");

    for (i = 0; i < MAX_FILES; i++)
        memset(&files[i], 0, sizeof(files[i]));

    file_count = 0;
}

/* Find a file by name */
int find_file(const char *name) {
    int i;

    for (i = 0; i < MAX_FILES; i++) {
        if (files[i].active &&
            strcmp(files[i].name, name) == 0)
            return i;
    }

    return -1;
}

/* Contiguous allocation */
int contiguous_allocation(int size, int file_index, int out[]) {
    int start, j;

    for (start = 0; start <= DISK_SIZE - size; start++) {
        int free_space = 1;

        for (j = 0; j < size; j++) {
            if (strcmp(disk[start + j], "FREE") != 0) {
                free_space = 0;
                break;
            }
        }

        if (free_space) {
            for (j = 0; j < size; j++) {
                int b = start + j;

                snprintf(disk[b], MAX_NAME, "%s",
                         files[file_index].name);
                out[j] = b;
            }

            return size;
        }
    }

    return 0;
}

/* Linked allocation */
int linked_allocation(int size, int file_index, int out[]) {
    int count = 0;
    int i;

    for (i = 0; i < DISK_SIZE && count < size; i++) {
        if (strcmp(disk[i], "FREE") == 0) {
            snprintf(disk[i], MAX_NAME, "%s",
                     files[file_index].name);
            out[count++] = i;
        }
    }

    if (count < size) {
        for (i = 0; i < count; i++)
            strcpy(disk[out[i]], "FREE");

        return 0;
    }

    return count;
}

/* Indexed allocation: one index block plus data blocks */
int indexed_allocation(int size, int file_index, int out[]) {
    int total = size + 1;
    int count = 0;
    int i;

    if (total > DISK_SIZE || total > MAX_BLOCKS)
        return 0;

    for (i = 0; i < DISK_SIZE && count < total; i++) {
        if (strcmp(disk[i], "FREE") == 0) {
            snprintf(disk[i], MAX_NAME, "%s",
                     files[file_index].name);
            out[count++] = i;
        }
    }

    if (count < total) {
        for (i = 0; i < count; i++)
            strcpy(disk[out[i]], "FREE");

        return 0;
    }

    return count;
}

/* Create a file */
void create_file(void) {
    char name[MAX_NAME];
    int size, choice, slot = -1;
    int result, allocated = 0;
    int i;

    printf("\n=============== CREATE FILE ===============\n");

    if (!read_line("Enter file name: ", name, sizeof(name)) ||
        name[0] == '\0') {
        printf("Invalid file name or input error.\n");
        return;
    }

    if (strchr(name, ' ') != NULL) {
        printf("File name must not contain spaces.\n");
        return;
    }

    if (find_file(name) != -1) {
        printf("File already exists.\n");
        return;
    }

    result = read_integer("Enter file size in blocks: ", &size);

    if (result != 1 || size <= 0) {
        printf("Invalid size.\n");
        return;
    }

    if (size > DISK_SIZE) {
        printf("File size exceeds disk capacity.\n");
        return;
    }

    for (i = 0; i < MAX_FILES; i++) {
        if (!files[i].active) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        printf("Maximum file limit reached.\n");
        return;
    }

    printf("\nSelect File Allocation Method:\n");
    printf("1. Contiguous Allocation\n");
    printf("2. Linked Allocation\n");
    printf("3. Indexed Allocation\n");

    result = read_integer("Enter choice: ", &choice);

    if (result != 1 || choice < 1 || choice > 3) {
        printf("Invalid allocation method.\n");
        return;
    }

    if (choice == 3 && size + 1 > DISK_SIZE) {
        printf("Indexed allocation requires one extra index block.\n");
        printf("Insufficient disk capacity.\n");
        return;
    }

    memset(&files[slot], 0, sizeof(files[slot]));

    snprintf(files[slot].name, MAX_NAME, "%s", name);
    files[slot].size = size;
    files[slot].index_block = -1;
    strcpy(files[slot].permission, "rw-");

    switch (choice) {
        case 1:
            strcpy(files[slot].allocation, "Contiguous");
            allocated = contiguous_allocation(
                size, slot, files[slot].blocks);
            break;

        case 2:
            strcpy(files[slot].allocation, "Linked");
            allocated = linked_allocation(
                size, slot, files[slot].blocks);
            break;

        case 3:
            strcpy(files[slot].allocation, "Indexed");
            allocated = indexed_allocation(
                size, slot, files[slot].blocks);
            break;
    }

    if (allocated == 0) {
        printf("Not enough disk space for this allocation method.\n");
        memset(&files[slot], 0, sizeof(files[slot]));
        return;
    }

    files[slot].block_count = allocated;

    if (choice == 3)
        files[slot].index_block = files[slot].blocks[0];

    files[slot].active = 1;
    file_count++;

    printf("\nFile created successfully.\n");
    printf("File Name       : %s\n", files[slot].name);
    printf("File Size       : %d data blocks\n", files[slot].size);
    printf("Allocation      : %s\n", files[slot].allocation);

    if (choice == 3) {
        printf("Index Block     : %d\n",
               files[slot].index_block);
        printf("Data Blocks     : ");

        for (i = 1; i < allocated; i++)
            printf("%d%s", files[slot].blocks[i],
                   i == allocated - 1 ? "\n" : " ");
    } else {
        printf("Allocated Blocks: ");

        for (i = 0; i < allocated; i++)
            printf("%d%s", files[slot].blocks[i],
                   i == allocated - 1 ? "\n" : " ");
    }

    printf("Permissions     : %s\n", files[slot].permission);
}

/* Write data to a file */
void write_file(void) {
    char name[MAX_NAME];
    char data[MAX_CONTENT];
    int index;

    printf("\n================ WRITE FILE ================\n");

    if (!read_line("Enter file name: ", name, sizeof(name)) ||
        name[0] == '\0') {
        printf("Invalid file name or input error.\n");
        return;
    }

    index = find_file(name);

    if (index == -1) {
        printf("File not found.\n");
        return;
    }

    if (strchr(files[index].permission, 'w') == NULL) {
        printf("Access Denied: Write permission not available.\n");
        return;
    }

    if (!read_line("Enter data (maximum 255 characters): ",
                   data, sizeof(data))) {
        printf("Input error. Data was not written.\n");
        return;
    }

    snprintf(files[index].content, MAX_CONTENT, "%s", data);

    printf("\nData written successfully.\n");
}

/* Read a file */
void read_file(void) {
    char name[MAX_NAME];
    int index;

    printf("\n================ READ FILE =================\n");

    if (!read_line("Enter file name: ", name, sizeof(name)) ||
        name[0] == '\0') {
        printf("Invalid file name or input error.\n");
        return;
    }

    index = find_file(name);

    if (index == -1) {
        printf("File not found.\n");
        return;
    }

    if (strchr(files[index].permission, 'r') == NULL) {
        printf("Access Denied: Read permission not available.\n");
        return;
    }

    printf("\nFile Name: %s\n", files[index].name);
    printf("Content  : %s\n", files[index].content);
}

/* Delete a file and free its blocks */
void delete_file(void) {
    char name[MAX_NAME];
    int index, i;

    printf("\n=============== DELETE FILE ===============\n");

    if (!read_line("Enter file name: ", name, sizeof(name)) ||
        name[0] == '\0') {
        printf("Invalid file name or input error.\n");
        return;
    }

    index = find_file(name);

    if (index == -1) {
        printf("File not found.\n");
        return;
    }

    for (i = 0; i < files[index].block_count; i++)
        strcpy(disk[files[index].blocks[i]], "FREE");

    memset(&files[index], 0, sizeof(files[index]));
    file_count--;

    printf("\nFile deleted successfully.\n");
}

/* Display all files */
void display_files(void) {
    int i, j;

    printf("\n================ FILE LIST ================\n");

    if (file_count == 0) {
        printf("No files available.\n");
        return;
    }

    for (i = 0; i < MAX_FILES; i++) {
        if (!files[i].active)
            continue;

        printf("\nFile Name  : %s\n", files[i].name);
        printf("Size       : %d data blocks\n", files[i].size);
        printf("Allocation : %s\n", files[i].allocation);

        if (strcmp(files[i].allocation, "Indexed") == 0) {
            printf("Index Block: %d\n", files[i].index_block);
            printf("Data Blocks: ");

            for (j = 1; j < files[i].block_count; j++)
                printf("%d%s", files[i].blocks[j],
                       j == files[i].block_count - 1 ? "\n" : " ");
        } else {
            printf("Blocks     : ");

            for (j = 0; j < files[i].block_count; j++)
                printf("%d%s", files[i].blocks[j],
                       j == files[i].block_count - 1 ? "\n" : " ");
        }

        printf("Permission : %s\n", files[i].permission);
        printf("Content    : %s\n", files[i].content);
    }
}

/* Display virtual disk */
void display_disk(void) {
    int i;

    printf("\n================ VIRTUAL DISK ================\n");

    for (i = 0; i < DISK_SIZE; i++)
        printf("Block %-2d : %s\n", i, disk[i]);
}

/* Set file permissions */
void set_permissions(void) {
    char name[MAX_NAME];
    char permission[4];
    int index;

    printf("\n============= SET PERMISSIONS =============\n");

    if (!read_line("Enter file name: ", name, sizeof(name)) ||
        name[0] == '\0') {
        printf("Invalid file name or input error.\n");
        return;
    }

    index = find_file(name);

    if (index == -1) {
        printf("File not found.\n");
        return;
    }

    printf("\nPermission format:\n");
    printf("r-- = Read only\n");
    printf("rw- = Read and Write\n");
    printf("r-x = Read and Execute\n");
    printf("rwx = Read, Write and Execute\n");
    printf("--- = No permissions\n");

    if (!read_line("Enter permission (example rw-): ",
                   permission, sizeof(permission))) {
        printf("Input error.\n");
        return;
    }

    if (strlen(permission) != 3 ||
        (permission[0] != 'r' && permission[0] != '-') ||
        (permission[1] != 'w' && permission[1] != '-') ||
        (permission[2] != 'x' && permission[2] != '-')) {
        printf("Invalid permission format.\n");
        return;
    }

    strcpy(files[index].permission, permission);

    printf("\nPermissions updated successfully.\n");
    printf("New Permission: %s\n", permission);
}

/* Check access permissions */
void check_access(void) {
    char name[MAX_NAME];
    int index, operation, result;
    char *p;

    printf("\n============== CHECK ACCESS ===============\n");

    if (!read_line("Enter file name: ", name, sizeof(name)) ||
        name[0] == '\0') {
        printf("Invalid file name or input error.\n");
        return;
    }

    index = find_file(name);

    if (index == -1) {
        printf("File not found.\n");
        return;
    }

    printf("\n1. Read\n");
    printf("2. Write\n");
    printf("3. Execute\n");

    result = read_integer("Enter operation: ", &operation);

    if (result != 1) {
        printf("Invalid operation input.\n");
        return;
    }

    p = files[index].permission;

    switch (operation) {
        case 1:
            printf(strchr(p, 'r')
                ? "ACCESS GRANTED: Read operation allowed.\n"
                : "ACCESS DENIED: Read permission not available.\n");
            break;

        case 2:
            printf(strchr(p, 'w')
                ? "ACCESS GRANTED: Write operation allowed.\n"
                : "ACCESS DENIED: Write permission not available.\n");
            break;

        case 3:
            printf(strchr(p, 'x')
                ? "ACCESS GRANTED: Execute operation allowed.\n"
                : "ACCESS DENIED: Execute permission not available.\n");
            break;

        default:
            printf("Invalid operation.\n");
    }
}

/* Main menu */
int main(void) {
    int choice, result;

    initialize_disk();

    while (1) {
        printf("\n================================================\n");
        printf("          FILE SYSTEM SIMULATOR (C)\n");
        printf("================================================\n");
        printf("1.  Create File\n");
        printf("2.  Write File\n");
        printf("3.  Read File\n");
        printf("4.  Delete File\n");
        printf("5.  Display Files\n");
        printf("6.  Display Disk\n");
        printf("7.  Set File Permissions\n");
        printf("8.  Check Access\n");
        printf("9.  Exit\n");
        printf("================================================\n");

        result = read_integer("Enter your choice: ", &choice);

        if (result == 0) {
            printf("\nExiting File System Simulator...\n");
            break;
        }

        if (result != 1) {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1:
                create_file();
                break;
            case 2:
                write_file();
                break;
            case 3:
                read_file();
                break;
            case 4:
                delete_file();
                break;
            case 5:
                display_files();
                break;
            case 6:
                display_disk();
                break;
            case 7:
                set_permissions();
                break;
            case 8:
                check_access();
                break;
            case 9:
                printf("\nExiting File System Simulator...\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
