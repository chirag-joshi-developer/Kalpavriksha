#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "users.txt"
#define NAME_LENGTH 100

typedef struct {
    int id;
    char name[NAME_LENGTH];
    int age;
} User;

void createUser() {
    FILE *file = fopen(FILE_NAME, "r");
    if (!file) {
        printf("File not found. Creating a new one...\n");
        FILE *temp = fopen(FILE_NAME, "w");
        if (!temp) {
            printf("Error creating file.\n");
            return;
        }
        fclose(temp);
        file = fopen(FILE_NAME, "r");
    }

    int id = 0;
    printf("Enter user ID:\n");
    scanf("%d", &id);

    User u;
    while (fscanf(file, "%d|%[^|]|%d\n", &u.id, u.name, &u.age) == 3) {
        if (u.id == id) {
            printf("\nUser with this ID already exists.\n");
            fclose(file);
            return;
        }
    }
    fclose(file);

    file = fopen(FILE_NAME, "a");
    if (!file) {
        printf("Error opening file for writing.\n");
        return;
    }

    u.id = id;
    printf("\nEnter user full name:\n");
    getchar();  // clear newline
    fgets(u.name, NAME_LENGTH, stdin);
    u.name[strcspn(u.name, "\n")] = '\0';

    if (strlen(u.name) == 0) {
        printf("Invalid name.\n");
        fclose(file);
        return;
    }

    printf("\nEnter user age:\n");
    scanf("%d", &u.age);
    if (u.age < 0) {
        printf("Invalid age.\n");
        fclose(file);
        return;
    }

    fprintf(file, "%d|%s|%d\n", u.id, u.name, u.age);
    fclose(file);
    printf("\nA new user has been added.\n");
}

void readUsers() {
    FILE *file = fopen(FILE_NAME, "r");
    if (!file) {
        printf("\nThis file doesn't exist yet.\n");
        return;
    }

    User u;
    printf("\nAll Users:\n");
    while (fscanf(file, "%d|%[^|]|%d\n", &u.id, u.name, &u.age) == 3) {
        printf("\nID: %d, Name: %s, Age: %d\n", u.id, u.name, u.age);
    }
    fclose(file);
}

void updateUser() {
    FILE *file = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) {
        printf("\nError opening file.\n");
        return;
    }

    int id = 0, found = 0;
    printf("\nEnter ID to update:\n");
    scanf("%d", &id);

    User u;
    while (fscanf(file, "%d|%[^|]|%d\n", &u.id, u.name, &u.age) == 3) {
        if (u.id == id) {
            found = 1;
            printf("\nEnter new full name: \n");
            getchar();
            fgets(u.name, NAME_LENGTH, stdin);
            u.name[strcspn(u.name, "\n")] = '\0';

            printf("\nEnter new age: \n");
            scanf("%d", &u.age);
        }
        fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
    }

    fclose(file);
    fclose(temp);

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found)
        printf("\nUser updated successfully.\n");
    else
        printf("\nUser with ID %d not found.\n", id);
}

void deleteUser() {
    FILE *file = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) {
        printf("\nError opening file.\n");
        return;
    }

    int id = 0, found = 0;
    printf("\nEnter ID to delete:\n");
    scanf("%d", &id);

    User u;
    while (fscanf(file, "%d|%[^|]|%d\n", &u.id, u.name, &u.age) == 3) {
        if (id == u.id)
            found = 1;
        else
            fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
    }

    fclose(file);
    fclose(temp);

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found)
        printf("\nUser deleted successfully.\n");
    else
        printf("\nUser not found.\n");
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
