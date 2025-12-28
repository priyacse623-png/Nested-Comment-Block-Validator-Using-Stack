

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 1000
#define MAX_LINE 256
typedef struct {
    char open[10];
    char close[10];
    char type[30];
    int line;
} CommentBlock;
typedef struct {
    CommentBlock items[MAX_SIZE];
    int top;
} Stack;
void initStack(Stack *s);
bool isEmpty(Stack *s);
bool isFull(Stack *s);
void push(Stack *s, CommentBlock item);
CommentBlock pop(Stack *s);
CommentBlock peek(Stack *s);
bool startsWith(const char *str, const char *prefix, int pos);
void validateFile(const char *filename);
void validateString(const char *text);
void printResults(int validCount, int errorCount);

void initStack(Stack *s) {
    s->top = -1;
}

bool isEmpty(Stack *s) {
    return s->top == -1;
}

bool isFull(Stack *s) {
    return s->top == MAX_SIZE - 1;
}

void push(Stack *s, CommentBlock item) {
    if (isFull(s)) {
        printf("Stack overflow!\n");
        return;
    }
    s->items[++(s->top)] = item;
}

CommentBlock pop(Stack *s) {
    if (isEmpty(s)) {
        CommentBlock empty = {"", "", "", -1};
        return empty;
    }
    return s->items[(s->top)--];
}

CommentBlock peek(Stack *s) {
    if (isEmpty(s)) {
        CommentBlock empty = {"", "", "", -1};
        return empty;
    }
    return s->items[s->top];
}

bool startsWith(const char *str, const char *prefix, int pos) {
    int len = strlen(prefix);
    return strncmp(str + pos, prefix, len) == 0;
}

void validateString(const char *text) {
    Stack stack;
    initStack(&stack);
    
    int validCount = 0;
    int errorCount = 0;
    int lineNum = 1;
    int length = strlen(text);
    int i;
    
    printf("\n========================================\n");
    printf("     VALIDATION RESULTS\n");
    printf("========================================\n\n");
    
    for (i = 0; i < length; i++) {
        bool matched = false;

        if (startsWith(text, "/*", i)) {
            CommentBlock block;
            strcpy(block.open, "/*");
            strcpy(block.close, "*/");
            strcpy(block.type, "C-style");
            block.line = lineNum;
            push(&stack, block);
            printf("? Line %d: Opening C-style comment '/*'\n", lineNum);
            i += 1; // Skip next character
            matched = true;
        }
        else if (startsWith(text, "<!--", i)) {
            CommentBlock block;
            strcpy(block.open, "<!--");
            strcpy(block.close, "-->");
            strcpy(block.type, "HTML");
            block.line = lineNum;
            push(&stack, block);
            printf("? Line %d: Opening HTML comment '<!--'\n", lineNum);
            i += 3;
            matched = true;
        }
        
        if (!matched) {
            if (startsWith(text, "*/", i)) {
                if (isEmpty(&stack)) {
                    printf("? Line %d: ERROR - Unexpected closing '*/' without opening\n", lineNum);
                    errorCount++;
                } else {
                    CommentBlock top = peek(&stack);
                    if (strcmp(top.close, "*/") == 0) {
                        CommentBlock closed = pop(&stack);
                        printf("? Line %d-%d: Valid %s comment block closed\n", 
                               closed.line, lineNum, closed.type);
                        validCount++;
                    } else {
                        printf("? Line %d: ERROR - Expected '%s' but found '*/'\n", 
                               lineNum, top.close);
                        errorCount++;
                        pop(&stack);
                    }
                }
                i += 1;
            }
            else if (startsWith(text, "-->", i)) {
                if (isEmpty(&stack)) {
                    printf("? Line %d: ERROR - Unexpected closing '-->' without opening\n", lineNum);
                    errorCount++;
                } else {
                    CommentBlock top = peek(&stack);
                    if (strcmp(top.close, "-->") == 0) {
                        CommentBlock closed = pop(&stack);
                        printf("? Line %d-%d: Valid %s comment block closed\n", 
                               closed.line, lineNum, closed.type);
                        validCount++;
                    } else {
                        printf("? Line %d: ERROR - Expected '%s' but found '-->'\n", 
                               lineNum, top.close);
                        errorCount++;
                        pop(&stack);
                    }
                }
                i += 2;
            }
        }
        
        // Track line numbers
        if (text[i] == '\n') {
            lineNum++;
        }
    }

    while (!isEmpty(&stack)) {
        CommentBlock unclosed = pop(&stack);
        printf("? Line %d: ERROR - Unclosed %s comment '%s'\n", 
               unclosed.line, unclosed.type, unclosed.open);
        errorCount++;
    }
    
    printResults(validCount, errorCount);
}

void validateFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file '%s'\n", filename);
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *content = (char *)malloc(fileSize + 1);
    if (content == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return;
    }
    
    fread(content, 1, fileSize, file);
    content[fileSize] = '\0';
    fclose(file);
    
    printf("\nValidating file: %s\n", filename);
    validateString(content);
    free(content);
}

void printResults(int validCount, int errorCount) {
    printf("\n========================================\n");
    printf("          SUMMARY\n");
    printf("========================================\n");
    printf("Valid Comment Blocks: %d\n", validCount);
    printf("Errors Found: %d\n", errorCount);
    
    if (errorCount == 0 && validCount > 0) {
        printf("\n? All comment blocks are properly nested!\n");
    } else if (errorCount > 0) {
        printf("\n? Validation failed with %d error(s)\n", errorCount);
    } else {
        printf("\n? No comment blocks found\n");
    }
    printf("========================================\n\n");
}

int main() {
    int choice;
    char filename[100];
    char input[MAX_SIZE];
    
    printf("\n??????????????????????????????????????????\n");
    printf("?  NESTED COMMENT BLOCK VALIDATOR       ?\n");
    printf("?  Using Stack Data Structure            ?\n");
    printf("??????????????????????????????????????????\n");
    
    printf("\n1. Validate from file\n");
    printf("2. Enter code manually\n");
    printf("3. Run test cases\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume newline
    
    switch(choice) {
        case 1:
            printf("Enter filename: ");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = 0; // Remove newline
            validateFile(filename);
            break;
            
        case 2:
            printf("Enter your code (end with Ctrl+D or Ctrl+Z):\n");
            int i = 0;
            char ch;
            while ((ch = getchar()) != EOF && i < MAX_SIZE - 1) {
                input[i++] = ch;
            }
            input[i] = '\0';
            validateString(input);
            break;
            
        case 3:
            printf("\n=== TEST CASE 1: Valid Nested Comments ===\n");
            validateString("/* Outer comment\n   /* Inner comment */\n   Still outer\n*/");
            
            printf("\n\n=== TEST CASE 2: Unclosed Comment ===\n");
            validateString("/* This comment is not closed\nint main() {\n");
            
            printf("\n\n=== TEST CASE 3: Mismatched Delimiters ===\n");
            validateString("/* C-style start --> HTML close");
            
            printf("\n\n=== TEST CASE 4: Multiple Valid Blocks ===\n");
            validateString("/* First */\n<!-- Second -->\n/* Third */");
            break;
            
        default:
            printf("Invalid choice!\n");
    }
    
    return 0;
}
