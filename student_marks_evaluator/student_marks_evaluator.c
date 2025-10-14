#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define NAME_LENGTH 100
#define INPUT_LENGTH 200

typedef struct {
    char studentName[NAME_LENGTH];
    int rollNo;
    int studentMarks[3];
    float studentAverage;
} studentInfo;

void readStudentData(int noOfStudents, char inputs[][INPUT_LENGTH]) {
    getchar();
    for (int i = 0; i < noOfStudents; i++) {
        if (fgets(inputs[i], INPUT_LENGTH, stdin) == NULL) {
            printf("Error reading input.\n");
            exit(1);
        }
        if (strchr(inputs[i], '\n') == NULL) { 
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
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

void parseAndStoreStudentData(int noOfStudents, char inputs[][INPUT_LENGTH], studentInfo* allStudents) {
    for (int i = 0; i < noOfStudents; i++) {
        int parsed = sscanf(inputs[i], "%d %99[^0-9] %d %d %d",
                            &allStudents[i].rollNo,
                            allStudents[i].studentName,
                            &allStudents[i].studentMarks[0],
                            &allStudents[i].studentMarks[1],
                            &allStudents[i].studentMarks[2]);
        if (parsed != 5) {
            printf("Invalid input format for student %d.\n", i + 1);
            exit(1);
        }
        if (allStudents[i].rollNo < 0 ||
            allStudents[i].studentMarks[0] <= 0 ||
            allStudents[i].studentMarks[1] <= 0 ||
            allStudents[i].studentMarks[2] <= 0) {
            printf("Invalid roll number or marks for student %d.\n", i + 1);
            exit(1);
        }
        if (strlen(allStudents[i].studentName) == 0) {
            printf("Invalid name for student %d.\n", i + 1);
            exit(1);
        }
    }
    calculateAverages(noOfStudents, allStudents);
}

typedef enum {
    GRADE_A,
    GRADE_B,
    GRADE_C,
    GRADE_D,
    GRADE_F
} Grade;

Grade getGrade(float avg) {
    if (avg >= 85.0) return GRADE_A;
    else if (avg >= 70.0) return GRADE_B;
    else if (avg >= 50.0) return GRADE_C;
    else if (avg >= 35.0) return GRADE_D;
    else return GRADE_F;
}

void displayGrade(float average) {
    Grade g = getGrade(average);
    switch (g) {
        case GRADE_A: printf("Grade : A\nPerformance : *****\n\n"); break;
        case GRADE_B: printf("Grade : B\nPerformance : ****\n\n"); break;
        case GRADE_C: printf("Grade : C\nPerformance : ***\n\n"); break;
        case GRADE_D: printf("Grade : D\nPerformance : **\n\n"); break;
        default: printf("Grade : F\n\n"); break;
    }
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

    char inputs[noOfStudents][INPUT_LENGTH];
    readStudentData(noOfStudents, inputs);

    studentInfo* allStudents = (studentInfo *) malloc(noOfStudents * sizeof(studentInfo));
    parseAndStoreStudentData(noOfStudents, inputs, allStudents);

    displayAllStudentsDetails(noOfStudents, allStudents);
    displayRollNo(noOfStudents, allStudents, 0);

    free(allStudents);
    return 0;
}
