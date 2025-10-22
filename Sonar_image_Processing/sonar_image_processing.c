#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MINIMUM_SIZE_INPUT 2
#define MAXIMUM_SIZE_INPUT 10
#define MINIMUM_PIXEL_SIZE 0
#define MAXIMUM_PIXEL_SIZE 255

void swapElements(int *firstElement, int *secondElement) {
    int temp = *firstElement;
    *firstElement = *secondElement;
    *secondElement = temp;
}

void displayMatrix(const int *matrix, int matrixSize) {
    for (int row = 0; row < matrixSize; row++) {
        for (int column = 0; column < matrixSize; column++) {
            printf("%4d", *(matrix + row * matrixSize + column));
        }
        printf("\n");
    }
}

void generateMatrix(int *matrix, int matrixSize, int minimumVal, int maximumVal) {
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            *(matrix + i * matrixSize + j) = minimumVal + rand() % (maximumVal - minimumVal + 1);
        }
    }
}

void rotateMatrixClockwise90(int *matrix, int matrixSize) {
    for (int i = 0; i < matrixSize; i++) {
        for (int j = i + 1; j < matrixSize; j++) {
            swapElements(matrix + i * matrixSize + j, matrix + j * matrixSize + i);
        }
    }

    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize / 2; j++) {
            swapElements(matrix + i * matrixSize + j, matrix + i * matrixSize + (matrixSize - 1 - j));
        }
    }
}

void applyMeanSmoothing(int *matrix, int matrixSize) {
    int *temp = (int *)malloc(matrixSize * matrixSize * sizeof(int));
    if (!temp) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            int sum = 0, count = 0;

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int x = i + dx, y = j + dy;
                    if (x >= 0 && x < matrixSize && y >= 0 && y < matrixSize) {
                        sum += *(matrix + x * matrixSize + y);
                        count++;
                    }
                }
            }

            *(temp + i * matrixSize + j) = (int)((float)sum / count + 0.5f);
        }
    }

    for (int i = 0; i < matrixSize * matrixSize; i++) {
        *(matrix + i) = *(temp + i);
    }

    free(temp);
}

int main(void) {
    int matrixSize;

    printf("Enter matrix size between (%d-%d): ",MINIMUM_SIZE_INPUT ,MAXIMUM_SIZE_INPUT);
    while (scanf("%d", &matrixSize) != 1 || matrixSize < MINIMUM_SIZE_INPUT || matrixSize > MAXIMUM_SIZE_INPUT) {
        fprintf(stderr, "Invalid size. Must be between %d and %d.\n",MINIMUM_SIZE_INPUT ,MAXIMUM_SIZE_INPUT);
        while (getchar() != '\n');
        printf("Enter matrix size between (%d-%d):",MINIMUM_SIZE_INPUT ,MAXIMUM_SIZE_INPUT);
    }

    int *matrixPtr = (int *)malloc(matrixSize * matrixSize * sizeof(int));
    if (!matrixPtr) {
        fprintf(stderr, "Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    srand((unsigned)time(NULL));

    generateMatrix(matrixPtr, matrixSize, MINIMUM_PIXEL_SIZE, MAXIMUM_PIXEL_SIZE);

    printf("\nOriginal Matrix:\n\n");
    displayMatrix(matrixPtr, matrixSize);

    rotateMatrixClockwise90(matrixPtr, matrixSize);
    printf("\nAfter 90 Degree Clockwise Rotation:\n\n");
    displayMatrix(matrixPtr, matrixSize);

    applyMeanSmoothing(matrixPtr, matrixSize);
    printf("\nAfter Applying Smoothing Filter:\n\n");
    displayMatrix(matrixPtr, matrixSize);

    free(matrixPtr);
    return EXIT_SUCCESS;
}