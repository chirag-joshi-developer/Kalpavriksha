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

void calculateAverages(int noOfStudents, studentInfo* allStudents) {
    for (int i = 0; i < noOfStudents; i++) {
        allStudents[i].studentAverage = 
            (allStudents[i].studentMarks[0] +
             allStudents[i].studentMarks[1] +
             allStudents[i].studentMarks[2]) / 3.0;
    }
}

void readAndParseStudentData(int noOfStudents, studentInfo *allStudents) {
    getchar(); 

    for (int i = 0; i < noOfStudents; i++) {
        char line[INPUT_LENGTH];

        if (fgets(line, INPUT_LENGTH, stdin) == NULL)
            exit(1);

        if (strchr(line, '\n') == NULL) {
            int leftoverChar;
            while ((leftoverChar = getchar()) != '\n' && leftoverChar != EOF);
        }

        line[strcspn(line, "\n")] = '\0';

        int parsed = sscanf(line, "%d %99[^0-9] %d %d %d",
                            &allStudents[i].rollNo,
                            allStudents[i].studentName,
                            &allStudents[i].studentMarks[0],
                            &allStudents[i].studentMarks[1],
                            &allStudents[i].studentMarks[2]);

        if (parsed != 5 ||
            allStudents[i].rollNo <= 0 ||
            allStudents[i].studentMarks[0] < 0 ||
            allStudents[i].studentMarks[1] < 0 ||
            allStudents[i].studentMarks[2] < 0 ||
            strlen(allStudents[i].studentName) == 0) {
            printf("Invalid data for student %d, skipping.\n", i + 1);
            i--;
            continue;
        }
    }

    calculateAverages(noOfStudents, allStudents);
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

    studentInfo *allStudents = malloc(noOfStudents * sizeof(studentInfo));
    readAndParseStudentData(noOfStudents, allStudents);

    displayAllStudentsDetails(noOfStudents, allStudents);
    displayRollNo(noOfStudents, allStudents, 0);

    free(allStudents);
    return 0;
}