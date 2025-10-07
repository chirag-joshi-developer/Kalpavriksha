#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "users.txt"
#define NAME_LENGTH 100
#define LINE_LENGTH 10000

typedef struct {
    int id;
    char name[NAME_LENGTH];
    int age;
} User;

void flushInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void createUser() {
    FILE *file = fopen(FILE_NAME, "r");
    if (!file) {
        FILE *temp = fopen(FILE_NAME, "w");
        if (!temp) return;
        fclose(temp);
        file = fopen(FILE_NAME, "r");
    }

    int id;
    printf("Enter user ID:\n");
    scanf("%d", &id);
    getchar();

    char line[LINE_LENGTH];
    User u;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|%99[^|]|%d", &u.id, u.name, &u.age) == 3) {
            if (u.id == id) {
                printf("User with this ID already exists.\n");
                fclose(file);
                return;
            }
        }
    }
    fclose(file);

    file = fopen(FILE_NAME, "a");
    if (!file) return;

    u.id = id;
    char inputBuffer[LINE_LENGTH];
    while (1) {
        printf("Enter user full name (max 99 characters):\n");
        if (!fgets(inputBuffer, sizeof(inputBuffer), stdin)) {
            printf("Error reading input.\n");
            fclose(file);
            return;
        }
        if (inputBuffer[strlen(inputBuffer) - 1] != '\n') {
            printf("Name too long. Max 99 characters allowed.\n");
            flushInput();
            continue;
        }
        inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
        if (strlen(inputBuffer) >= NAME_LENGTH) {
            printf("Name too long. Max 99 characters allowed.\n");
        } else if (strlen(inputBuffer) == 0) {
            printf("Name cannot be empty.\n");
        } else {
            strcpy(u.name, inputBuffer);
            break;
        }
    }

    printf("Enter user age:\n");
    scanf("%d", &u.age);
    if (u.age < 0) {
        printf("Invalid age.\n");
        fclose(file);
        return;
    }

    fprintf(file, "%d|%s|%d\n", u.id, u.name, u.age);
    fclose(file);
    printf("A new user has been added.\n");
}

void readUsers() {
    FILE *file = fopen(FILE_NAME, "r");
    if (!file) {
        printf("No users found.\n");
        return;
    }

    char line[LINE_LENGTH];
    User u;
    printf("All Users:\n");
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|%99[^|]|%d", &u.id, u.name, &u.age) == 3)
            printf("ID: %d, Name: %s, Age: %d\n", u.id, u.name, u.age);
    }
    fclose(file);
}

void updateUser() {
    FILE *file = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) {
        printf("Error opening file.\n");
        return;
    }

    int id, found = 0;
    printf("Enter ID to update:\n");
    scanf("%d", &id);
    getchar();

    char line[1024];
    User u;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|%99[^|]|%d", &u.id, u.name, &u.age) == 3) {
            if (u.id == id) {
                found = 1;
                char inputBuffer[1024];
                while (1) {
                    printf("Enter new full name (max 99 characters):\n");
                    if (!fgets(inputBuffer, sizeof(inputBuffer), stdin)) {
                        printf("Error reading input.\n");
                        fclose(file);
                        fclose(temp);
                        return;
                    }
                    if (inputBuffer[strlen(inputBuffer) - 1] != '\n') {
                        printf("Name too long. Max 99 characters allowed.\n");
                        flushInput();
                        continue;
                    }
                    inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
                    if (strlen(inputBuffer) >= NAME_LENGTH) {
                        printf("Name too long. Max 99 characters allowed.\n");
                    } else if (strlen(inputBuffer) == 0) {
                        printf("Name cannot be empty.\n");
                    } else {
                        strcpy(u.name, inputBuffer);
                        break;
                    }
                }
                printf("Enter new age:\n");
                scanf("%d", &u.age);
            }
            fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
        }
    }

    fclose(file);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found)
        printf("User updated successfully.\n");
    else
        printf("User with ID %d not found.\n", id);
}

void deleteUser() {
    FILE *file = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) {
        printf("Error opening file.\n");
        return;
    }

    int id, found = 0;
    printf("Enter ID to delete:\n");
    scanf("%d", &id);

    char line[LINE_LENGTH];
    User u;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d|%99[^|]|%d", &u.id, u.name, &u.age) == 3) {
            if (u.id == id)
                found = 1;
            else
                fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
        }
    }

    fclose(file);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found)
        printf("User deleted successfully.\n");
    else
        printf("User not found.\n");
}

int main() {
    int task;
    while (1) {
        printf("\nSelect one of these CRUD operations:\n");
        printf("1. Create new user\n");
        printf("2. Read all users\n");
        printf("3. Update existing user\n");
        printf("4. Delete existing user\n");
        printf("0. Exit program\n");
        printf("Enter your choice: ");
        scanf("%d", &task);

        switch (task) {
            case 1:
                createUser();
                break;
            case 2:
                readUsers();
                break;
            case 3:
                updateUser();
                break;
            case 4:
                deleteUser();
                break;
            case 0:
                printf("Exiting program.\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}
