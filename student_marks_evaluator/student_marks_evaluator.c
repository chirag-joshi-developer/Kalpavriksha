#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NAME_LENGTH 100
#define INPUT_LENGTH 200

typedef enum {
    A,
    B,
    C,
    D,
    F
} Grade;

typedef struct {
    char studentName[NAME_LENGTH];
    int rollNo;
    int studentMarks[3];
    float studentAverage;
} studentInfo;

void readStudentData(int noOfStudents, char **inputs) {
    getchar();
    for (int i = 0; i < noOfStudents; i++) {
        if (fgets(inputs[i], INPUT_LENGTH, stdin) == NULL) {
            printf("Error reading input for student %d.\n", i + 1);
            exit(1);
        }
        if (strchr(inputs[i], '\n') == NULL) { 
            int leftoverChar;
            while ((leftoverChar = getchar()) != '\n' && leftoverChar != EOF);
        }
        inputs[i][strcspn(inputs[i], "\n")] = '\0';
    }
}

void calculateAverages(int noOfStudents, studentInfo* allStudents) {
    for (int i = 0; i < noOfStudents; i++) {
        allStudents[i].studentAverage = 
            (allStudents[i].studentMarks[0] +
             allStudents[i].studentMarks[1] +
             allStudents[i].studentMarks[2]) / 3.0;
    }
}

int parseAndStoreStudentData(int noOfStudents, char **inputs, studentInfo* allStudents) {
    int validCount = 0;
    for (int i = 0; i < noOfStudents; i++) {
        int parsed = sscanf(inputs[i], "%d %99[^0-9] %d %d %d",
                            &allStudents[validCount].rollNo,
                            allStudents[validCount].studentName,
                            &allStudents[validCount].studentMarks[0],
                            &allStudents[validCount].studentMarks[1],
                            &allStudents[validCount].studentMarks[2]);

        if (parsed != 5) {
            printf("Warning: Invalid input format for student %d. Skipping entry.\n", i + 1);
            continue;
        }
        if (allStudents[validCount].rollNo <= 0 ||
            allStudents[validCount].studentMarks[0] < 0 ||
            allStudents[validCount].studentMarks[1] < 0 ||
            allStudents[validCount].studentMarks[2] < 0) {
            printf("Warning: Invalid roll number or marks for student %d. Skipping entry.\n", i + 1);
            continue;
        }
        if (strlen(allStudents[validCount].studentName) == 0) {
            printf("Warning: Invalid name for student %d. Skipping entry.\n", i + 1);
            continue;
        }
        validCount++;
    }
    if (validCount == 0) {
        printf("No valid student entries found.\n");
        return 0;
    }
    calculateAverages(validCount, allStudents);
    return validCount; 
}

Grade getGrade(float studentAverageMarks) {
    if (studentAverageMarks >= 85.0) return A;
    else if (studentAverageMarks >= 70.0) return B;
    else if (studentAverageMarks >= 50.0) return C;
    else if (studentAverageMarks >= 35.0) return D;
    else return F;
}

void displayGradeLetter(Grade grade) {
    switch (grade) {
        case A: printf("Grade : A\n"); break;
        case B: printf("Grade : B\n"); break;
        case C: printf("Grade : C\n"); break;
        case D: printf("Grade : D\n"); break;
        case F: printf("Grade : F\n"); break;
    }
}

void displayPerformanceStars(Grade grade) {
    switch (grade) {
        case A: printf("Performance : *****\n\n"); break;
        case B: printf("Performance : ****\n\n"); break;
        case C: printf("Performance : ***\n\n"); break;
        case D: printf("Performance : **\n\n"); break;
    }
}

void displayGrade(float studentAverageMarks) {
    Grade grade = getGrade(studentAverageMarks);
    displayGradeLetter(grade);
    displayPerformanceStars(grade);
}


void displayAllStudentsDetails(int noOfStudents, studentInfo* allStudents) {
    for (int i = 0; i < noOfStudents; i++) {
        printf("\nRoll : %d\n", allStudents[i].rollNo);
        printf("Name : %s\n", allStudents[i].studentName);
        printf("Total : %d\n",
               allStudents[i].studentMarks[0] +
               allStudents[i].studentMarks[1] +
               allStudents[i].studentMarks[2]);
        printf("Average : %.2f\n", allStudents[i].studentAverage);
        displayGrade(allStudents[i].studentAverage);
    }
}

void displayRollNo(int noOfStudents, studentInfo* allStudents, int current) {
    if (current == noOfStudents){
    	return;
    }
    printf("%d ", allStudents[current].rollNo);
    displayRollNo(noOfStudents, allStudents, current + 1);
}


int main() {
    int noOfStudents;

    while (1) {
        if (scanf("%d", &noOfStudents) == 1 && noOfStudents > 0)
            break;

        printf("Invalid input. Please enter a positive number.\n");
        while (getchar() != '\n');
    }

    char **inputs = malloc(noOfStudents * sizeof(char *));
    for (int i = 0; i < noOfStudents; i++) {
        inputs[i] = malloc(INPUT_LENGTH * sizeof(char));
    }

    readStudentData(noOfStudents, inputs);

    studentInfo *allStudents = malloc(noOfStudents * sizeof(studentInfo));
    int validCount = parseAndStoreStudentData(noOfStudents, inputs, allStudents);

    if (validCount > 0) {
        displayAllStudentsDetails(validCount, allStudents);
        displayRollNo(validCount, allStudents, 0);
    }

    for (int i = 0; i < noOfStudents; i++) {
        free(inputs[i]);
    }
    free(inputs);
    free(allStudents);

    return 0;
}
