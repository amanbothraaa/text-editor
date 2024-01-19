#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 100

// Define a structure to represent a line of text
typedef struct Line {
    char text[MAX_LINE_LEN];
    struct Line *next;
} Line;

Line *createLine(const char *text) {
    Line *newLine = (Line *)malloc(sizeof(Line));
    if (newLine != NULL) {
        strncpy(newLine->text, text, MAX_LINE_LEN - 1);
        newLine->text[MAX_LINE_LEN - 1] = '\0';
        newLine->next = NULL;
    }
    return newLine;
}

void displayText(Line *head) {
    Line *current = head;
    int lineNum = 1;
    while (current != NULL) {
        printf("%d: %s\n", lineNum, current->text);
        current = current->next;
        lineNum++;
    }
}


void insertLine(Line **head, const char *text) {
    Line *newLine = createLine(text);
    if (newLine == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    if (*head == NULL) {
        *head = newLine;
    } else {
        Line *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newLine;
    }
}

void deleteLine(Line **head, int lineNum) {
    if (lineNum < 1) {
        printf("Invalid line number.\n");
        return;
    }

    if (lineNum == 1) {
        Line *temp = *head;
        *head = (*head)->next;
        free(temp);
    } else {
        Line *current = *head;
        Line *prev = NULL;
        int count = 1;
        while (count < lineNum && current != NULL) {
            prev = current;
            current = current->next;
            count++;
        }

        if (current == NULL) {
            printf("Line not found.\n");
        } else {
            prev->next = current->next;
            free(current);
        }
    }
}




char* copyLine(Line *head, int lineNum) {
    if (lineNum < 1) {
        printf("Invalid line number.\n");
        return NULL;
    }

    Line *current = head;
    int count = 1;
    while (count < lineNum && current != NULL) {
        current = current->next;
        count++;
    }

    if (current == NULL) {
        printf("Line not found.\n");
        return NULL;
    }

    char *copy = (char *)malloc(MAX_LINE_LEN);
    if (copy != NULL) {
        strcpy(copy, current->text);
    }
    return copy;
}

void pasteLine(Line **head, const char *copiedText) {
    if (copiedText == NULL) {
        printf("Nothing to paste.\n");
        return;
    }

    insertLine(head, copiedText);
}

void saveToFile(const char *filename, Line *text) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Failed to open the file for saving.\n");
        return;
    }

    Line *current = text;
    while (current != NULL) {
        fprintf(file, "%s\n", current->text);
        current = current->next;
    }

    fclose(file);
}

Line *loadFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open the file for loading.\n");
        return NULL;
    }

    Line *text = NULL;
    char buffer[MAX_LINE_LEN];

    while (fgets(buffer, MAX_LINE_LEN, file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline character
        insertLine(&text, buffer);
    }

    fclose(file);
    return text;
}

void freeDocument(Line *head) {
    Line *current = head;
    while (current != NULL) {
        Line *temp = current;
        current = current->next;
        free(temp);
    }
}

int calculateWordCount(Line *text) {
    int wordCount = 0;

    Line *current = text;
    while (current != NULL) {
        char *token = strtok(current->text, " \t\n");
        while (token != NULL) {
            wordCount++;
            token = strtok(NULL, " \t\n");
        }
        current = current->next;
    }

    return wordCount;
}

// Function to insert a line at a specific position
void insertLineAtPosition(Line **head, const char *text, int position) {
    if (position < 1) {
        printf("Invalid position.\n");
        return;
    }

    Line *newLine = createLine(text);
    if (newLine == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    if (position == 1) {
        newLine->next = *head;
        *head = newLine;
    } else {
        Line *current = *head;
        int count = 1;
        while (count < position - 1 && current != NULL) {
            current = current->next;
            count++;
        }

        if (current == NULL) {
            printf("Position out of bounds.\n");
            free(newLine);
            return;
        }

        newLine->next = current->next;
        current->next = newLine;
    }
}

// Function to move a line to a new position
void moveLine(Line **head, int sourcePosition, int targetPosition) {
    if (sourcePosition == targetPosition || sourcePosition < 1 || targetPosition < 1) {
        printf("Invalid positions.\n");
        return;
    }

    Line *sourcePrev = NULL;
    Line *sourceCurrent = *head;
    int sourceCount = 1;
    while (sourceCount < sourcePosition && sourceCurrent != NULL) {
        sourcePrev = sourceCurrent;
        sourceCurrent = sourceCurrent->next;
        sourceCount++;
    }

    if (sourceCurrent == NULL) {
        printf("Source position not found.\n");
        return;
    }

    if (sourcePrev == NULL) {
        *head = sourceCurrent->next;
    } else {
        sourcePrev->next = sourceCurrent->next;
    }

    Line *targetPrev = NULL;
    Line *targetCurrent = *head;
    int targetCount = 1;
    while (targetCount < targetPosition && targetCurrent != NULL) {
        targetPrev = targetCurrent;
        targetCurrent = targetCurrent->next;
        targetCount++;
    }

    if (targetCurrent == NULL) {
        printf("Target position not found.\n");
        // Revert changes
        if (sourcePrev == NULL) {
            sourceCurrent->next = *head;
            *head = sourceCurrent;
        } else {
            sourceCurrent->next = sourcePrev->next;
            sourcePrev->next = sourceCurrent;
        }
        return;
    }

    if (targetPrev == NULL) {
        sourceCurrent->next = *head;
        *head = sourceCurrent;
    } else {
        sourceCurrent->next = targetCurrent;
        targetPrev->next = sourceCurrent;
    }
}

// Function to undo the last operation
void undoOperation(Line **head, Line **undoStack) {
    if (*head == NULL) {
        printf("Nothing to undo.\n");
        return;
    }

    Line *top = *head;
    *head = (*head)->next;

    // Push the line onto the undo stack
    top->next = *undoStack;
    *undoStack = top;

    printf("Undo successful.\n");
}

// Function to clear the undo stack
void clearUndoStack(Line **undoStack) {
    freeDocument(*undoStack);
    *undoStack = NULL;
}

// Function to reverse the order of lines in the document
void reverseLines(Line **head) {
    Line *prev = NULL;
    Line *current = *head;
    Line *next = NULL;

    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    *head = prev;
}


int main() {
    Line *text = NULL;
    Line *undoStack = NULL;  
    int choice;
    char buffer[MAX_LINE_LEN];
    char *copiedLine = NULL;

    while (1) {
        printf("\n1. Insert Line\n2. Delete Line\n3. Copy Line\n4. Paste Line\n5. Save to File\n6. Open from File\n7. Word Count\n8. Display Text\n9. Insert Line at Position\n10. Move Line\n11. Undo\n12. Clear Undo Stack\n13. Reverse Lines\n14. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter text to insert: ");
                scanf(" %[^\n]", buffer);
                insertLine(&text, buffer);
                break;
            case 2:
                printf("Enter line number to delete: ");
                int lineNum;
                scanf("%d", &lineNum);
                deleteLine(&text, lineNum);
                break;
            case 3:
                printf("Enter line number to copy: ");
                int copyLineNum;
                scanf("%d", &copyLineNum);
                copiedLine = copyLine(text, copyLineNum);
                break;
            case 4:
                printf("Pasting the copied line...\n");
                pasteLine(&text, copiedLine);
                free(copiedLine); // Free the copied line buffer after pasting
                copiedLine = NULL;
                break;
            case 5:
                printf("Enter the filename to save: ");
                char saveFilename[MAX_LINE_LEN];
                scanf(" %[^\n]", saveFilename);
                saveToFile(saveFilename, text);
                break;
            case 6:
                printf("Enter the filename to open: ");
                char openFilename[MAX_LINE_LEN];
                scanf(" %[^\n]", openFilename);
                Line *loadedText = loadFromFile(openFilename);
                if (loadedText != NULL) {
                    freeDocument(text);
                    text = loadedText;
                }
                break;
            case 7:
                int wordCount = calculateWordCount(text);
                printf("Word Count: %d\n", wordCount);
                break;
            case 8:
                displayText(text);
                break;
            case 9:
                printf("Enter text to insert: ");
                scanf(" %[^\n]", buffer);
                int position;
                printf("Enter position to insert: ");
                scanf("%d", &position);
                insertLineAtPosition(&text, buffer, position);
                break;
            case 10:
                printf("Enter source position: ");
                int sourcePos;
                scanf("%d", &sourcePos);
                printf("Enter target position: ");
                int targetPos;
                scanf("%d", &targetPos);
                moveLine(&text, sourcePos, targetPos);
                break;
            case 11:
                undoOperation(&text, &undoStack);
                break;
            case 12:
                clearUndoStack(&undoStack);
                printf("Undo stack cleared.\n");
                break;
            case 13:
                reverseLines(&text);
                printf("Lines reversed.\n");
                break;
            case 14:
                // Free allocated memory before exiting
                freeDocument(text);
                free(copiedLine); // Free any remaining copied line buffer
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}
