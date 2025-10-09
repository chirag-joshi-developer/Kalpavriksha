#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char operationStack[100000];
int operationCounter = 0;

int valueStack[100000];
int valueCounter = 0;

void pushOperation(char operation) {
    if (operationCounter >= 100000) {
        printf("Error: Operation stack overflow.\n");
        exit(1);
    }
    operationStack[operationCounter++] = operation;
}

void popOperation() {
    if (operationCounter <= 0) {
        printf("Error: Operation stack underflow.\n");
        exit(1);
    }
    operationCounter--;
}

char topOperation() {
    if (operationCounter <= 0) {
        printf("Error: Operation stack is empty.\n");
        exit(1);
    }
    return operationStack[operationCounter - 1];
}

int isEmpty() {
    return operationCounter == 0;
}

void pushValue(int value) {
    if (valueCounter >= 100000) {
        printf("Error: Value stack overflow.\n");
        exit(1);
    }
    valueStack[valueCounter++] = value;
}

void popValue() {
    if (valueCounter <= 0) {
        printf("Error: Value stack underflow.\n");
        exit(1);
    }
    valueCounter--;
}

int topValue() {
    if (valueCounter <= 0) {
        printf("Error: Value stack is empty.\n");
        exit(1);
    }
    return valueStack[valueCounter - 1];
}

int getOperatorPrecedence(char operation) {
    if (operation == '-' || operation == '+') return 1;
    if (operation == '/' || operation == '*') return 2;
    return 0;
}

int calculate(int a, int b, char operation, int *error) {
    if (operation == '+') return a + b;
    if (operation == '-') return a - b;
    if (operation == '*') return a * b;
    if (operation == '/') {
        if (b == 0) {
            *error = 1;
            return 0;
        }
        return a / b;
    }
    return 0;
}

int evaluate(char *str, int *error) {
    int length = strlen(str);
    int onlyWhiteSpace = 1;

    for (int i = 0; i < length; ++i) {
        if (!isspace(str[i])) {
            onlyWhiteSpace = 0;
            break;
        }
    }

    if (length == 0 || onlyWhiteSpace) {
        *error = 2;
        return 0;
    }

    int expectNumber = 1;
    int i = 0;

    while (str[i] != '\0') {
        if (isspace(str[i])) {
            i++;
            continue;
        } else if (isdigit(str[i])) {
            if (!expectNumber) {
                *error = 2;
                return 0;
            }

            int value = 0;
            while (i < length && isdigit(str[i])) {
                value = value * 10 + (str[i] - '0');
                i++;
            }
            pushValue(value);
            expectNumber = 0;
        } else if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/') {
            if (expectNumber) {
                *error = 2;
                return 0;
            }

            while (!isEmpty() && getOperatorPrecedence(topOperation()) >= getOperatorPrecedence(str[i])) {
                int b = topValue();
                popValue();
                int a = topValue();
                popValue();
                char operation = topOperation();
                popOperation();
                int result = calculate(a, b, operation, error);
                if (*error) return 0;
                pushValue(result);
            }
            pushOperation(str[i]);
            i++;
            expectNumber = 1;
        } else {
            *error = 2;
            return 0;
        }
    }

    if (expectNumber) {
        *error = 2;
        return 0;
    }

    while (!isEmpty()) {
        int b = topValue();
        popValue();
        int a = topValue();
        popValue();
        char operation = topOperation();
        popOperation();
        int result = calculate(a, b, operation, error);
        if (*error) return 0;
        pushValue(result);
    }

    return topValue();
}

int main() {
    char str[100000];
    scanf("%[^\n]s", str);
    int error = 0;
    int result = evaluate(str, &error);
    if (error == 1) {
        printf("Error: Division by zero.\n");
    } else if (error == 2) {
        printf("Error: Invalid expression.\n");
    } else {
        printf("%d\n", result);
    }
    return 0;
}
