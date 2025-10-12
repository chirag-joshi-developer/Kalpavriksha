#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char studentName[100];
    int rollNo;
    int studentMarks[3];
    float studentAverage;
} studentInfo;

void readStudentData(int noOfStudents, char inputs[][200]) {
    getchar();
    for (int i = 0; i < noOfStudents; i++) {
        fgets(inputs[i], sizeof(inputs[i]), stdin);
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

void organizeStudentData(int noOfStudents, char inputs[][200], studentInfo* allStudents) {
    for (int i = 0; i < noOfStudents; i++) {
        sscanf(inputs[i], "%d %[^0-9] %d %d %d",
               &allStudents[i].rollNo,
               allStudents[i].studentName,
               &allStudents[i].studentMarks[0],
               &allStudents[i].studentMarks[1],
               &allStudents[i].studentMarks[2]);
    }
    calculateAverages(noOfStudents, allStudents);
}

void printGrade(float average) {
    if (average >= 85.0) {
        printf("Grade : A\n");
        printf("Performance : *****\n");
    } else if (average >= 70.0) {
        printf("Grade : B\n");
        printf("Performance : ****\n");
    } else if (average >= 50.0) {
        printf("Grade : C\n");
        printf("Performance : ***\n");
    } else if (average >= 35.0) {
        printf("Grade : D\n");
        printf("Performance : **\n");
    } else {
        printf("Grade : F\n");
    }
}

void printAllStudentsDetails(int noOfStudents, studentInfo* allStudents) {
    for (int i = 0; i < noOfStudents; i++) {
        printf("Roll : %d\n", allStudents[i].rollNo);
        printf("Name : %s\n", allStudents[i].studentName);
        printf("Total : %d\n",
               allStudents[i].studentMarks[0] +
               allStudents[i].studentMarks[1] +
               allStudents[i].studentMarks[2]);
        printf("Average : %.2f\n", allStudents[i].studentAverage);
        if (allStudents[i].studentAverage < 35.0) {
            printf("Grade : F\n\n");
            continue;
        }
        printGrade(allStudents[i].studentAverage);
        printf("\n");
    }
}

void printRollNo(int noOfStudents, studentInfo* allStudents, int current) {
    if (current == noOfStudents){
    	return;
    }
    printf("%d ", allStudents[current].rollNo);
    printRollNo(noOfStudents, allStudents, current + 1);
}

int main() {
    int noOfStudents;
    scanf("%d", &noOfStudents);

    char inputs[noOfStudents][200];
    readStudentData(noOfStudents, inputs);

    studentInfo* allStudents = (studentInfo *) malloc(noOfStudents * sizeof(studentInfo));
    organizeStudentData(noOfStudents, inputs, allStudents);

    printf("\n");
    printAllStudentsDetails(noOfStudents, allStudents);
    printRollNo(noOfStudents, allStudents, 0);

    free(allStudents);
    return 0;
}
