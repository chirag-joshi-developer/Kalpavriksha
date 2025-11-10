#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define BLOCK_SIZE 512
#define NUMBER_OF_BLOCKS 1024
#define FILE_NAME_MAXIMUM_SIZE 51
#define FILE_NAME_MINIMUM_SIZE 1
#define MAXIMUM_COMMAND_LENGTH 512
#define MAXIMUM_DATA_LENGTH 512000

typedef struct freeBlock  {
    int index;
    struct freeBlock* next;
    struct freeBlock* prev;
} freeBlock;

typedef struct file_Or_Directory_Node  {
    int isFile;
    char name[FILE_NAME_MAXIMUM_SIZE];
    struct file_Or_Directory_Node* parent;
    struct file_Or_Directory_Node* child;
    struct file_Or_Directory_Node* next;
    struct file_Or_Directory_Node* prev;
    int blockPointer[NUMBER_OF_BLOCKS];
    int sizeOfTheFile;
    int totalBlockCOUNT;
} file_Or_Directory_Node;

freeBlock* freeBlocksHead = NULL;
freeBlock* freeBlocksTail = NULL;

file_Or_Directory_Node* root = NULL;
file_Or_Directory_Node* cwd = NULL;

int usedBlockCount = 0;

char** diskSpace = NULL;

char* readInput(char * command);
int initializeRootDirectory();
void initializeSystem(char** diskSpaceParam);

void mkdir();
void changeDirectory();
void createFile();
void writeFile();
void readFile();
void DeleteFile();
void RemoveDirectory();
void ShowCurrentDirectory();
void ShowDiskImage();
void TerminateProgram();

char* trim(char *s)  {
    if (!s) return s;
    while(isspace((unsigned char)*s)) s++;
    if (*s == 0) return s;
    char *end = s + strlen(s) - 1;
    while(end > s && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return s;
}

file_Or_Directory_Node* find_child(file_Or_Directory_Node *parent, const char *name)  {
    if (!parent || !parent->child) return NULL;
    file_Or_Directory_Node *cur = parent->child;
    do  {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    } while(cur != parent->child);
    return NULL;
}

int allocate_block_from_head()  {
    if (!freeBlocksHead) return -1;
    freeBlock *n = freeBlocksHead;
    int idx = n->index;
    freeBlocksHead = n->next;
    if (freeBlocksHead) freeBlocksHead->prev = NULL;
    else freeBlocksTail = NULL;
    free(n);
    usedBlockCount++;
    return idx;
}

void free_block_to_tail(int index)  {
    freeBlock *n = (freeBlock*) malloc(sizeof(freeBlock));
    if (!n)  {
        fprintf(stderr, "Memory allocation failure while freeing block\n");
        return;
    }
    n->index = index;
    n->next = NULL;
    n->prev = freeBlocksTail;
    if (freeBlocksTail) freeBlocksTail->next = n;
    else freeBlocksHead = n;
    freeBlocksTail = n;
    usedBlockCount--;
}

int blocks_needed(int bytes)  {
    if (bytes <= 0) return 0;
    return (bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

void insert_child_node(file_Or_Directory_Node *parent, file_Or_Directory_Node *child)  {
    child->parent = parent;
    child->child = NULL;
    child->totalBlockCOUNT = 0;
    child->sizeOfTheFile = 0;
    if (!parent->child)  {
        parent->child = child;
        child->next = child->prev = child;
    } else  {
        file_Or_Directory_Node *tail = parent->child->prev;
        tail->next = child;
        child->prev = tail;
        child->next = parent->child;
        parent->child->prev = child;
    }
}

void remove_child_node(file_Or_Directory_Node *parent, file_Or_Directory_Node *child)  {
    if (!parent || !parent->child || !child) return;
    if (child->next == child)  {
        parent->child = NULL;
    } else  {
        if (parent->child == child) parent->child = child->next;
        child->prev->next = child->next;
        child->next->prev = child->prev;
    }
    child->next = child->prev = NULL;
    child->parent = NULL;
}

void free_file_node(file_Or_Directory_Node *fnode)  {
    if (!fnode) return;
    for (int i = 0; i < fnode->totalBlockCOUNT; ++i)  {
        int idx = fnode->blockPointer[i];
        if (idx >= 0) free_block_to_tail(idx);
    }
    fnode->totalBlockCOUNT = 0;
    fnode->sizeOfTheFile = 0;
}

void free_node_recursive(file_Or_Directory_Node *node)  {
    if (!node) return;
    if (!node->isFile)  {
        while (node->child)  {
            file_Or_Directory_Node *ch = node->child;
            remove_child_node(node, ch);
            free_node_recursive(ch);
        }
        free(node);
    } else  {
        free_file_node(node);
        free(node);
    }
}

int count_free_blocks()  {
    int cnt = 0;
    freeBlock *p = freeBlocksHead;
    while(p)  { cnt++; p = p->next; }
    return cnt;
}

char* readInput(char * command)  {
    if (!command) return NULL;
    if (fgets(command, MAXIMUM_DATA_LENGTH, stdin) == NULL)  {
        return NULL;
    }
    size_t L = strlen(command);
    if (L > 0 && command[L-1] == '\n') command[L-1] = '\0';
    return command;
}

int initializeRootDirectory()  {
    root = (file_Or_Directory_Node*) malloc(sizeof(file_Or_Directory_Node));
    if(!root) {
        printf("Error creating root \n");
        return 0;
    }
    strcpy(root->name,"/");
    root->isFile = 0;
    root->parent = NULL;
    root->child = NULL;
    root->next = root;
    root->prev = root;
    root->sizeOfTheFile = 0;
    root->totalBlockCOUNT = 0;
    cwd = root;
    return 1;
}

void initializeSystem(char** diskSpaceParam)  {
    diskSpace = diskSpaceParam;

    if(!initializeRootDirectory()) {
        return;
    }

    freeBlocksHead = freeBlocksTail = NULL;
    for(int i=0; i<NUMBER_OF_BLOCKS; i++) {
        freeBlock* newBlock  = (freeBlock*) malloc(sizeof(freeBlock));
        if(!newBlock) {
            printf("Failure in memory allocation\n");
            freeBlock* temp = freeBlocksHead;
            while(temp) {
                freeBlock* Next = temp->next;
                free(temp);
                temp = Next;
            }
            freeBlocksHead = freeBlocksTail = NULL;
            return ;
        }
        newBlock->index = i;
        newBlock->prev = freeBlocksTail;
        newBlock->next = NULL;
        if(freeBlocksTail) {
            freeBlocksTail->next = newBlock;
        }
        else {
            freeBlocksHead = newBlock;
        }
        freeBlocksTail = newBlock;
    }
}

void mkdir()  {
    char buf[MAXIMUM_COMMAND_LENGTH];
    printf("Enter directory name: ");
    if (!readInput(buf))  { printf("Input error\n"); return; }
    char *name = trim(buf);
    if (strlen(name) < FILE_NAME_MINIMUM_SIZE || strlen(name) >= FILE_NAME_MAXIMUM_SIZE)  {
        printf("Invalid name length.\n"); return;
    }
    if (find_child(cwd, name))  {
        printf("A file or directory with that name already exists.\n"); return;
    }
    file_Or_Directory_Node *dir = (file_Or_Directory_Node*) malloc(sizeof(file_Or_Directory_Node));
    if (!dir)  { printf("Memory allocation error\n"); return; }
    dir->isFile = 0;
    strncpy(dir->name, name, FILE_NAME_MAXIMUM_SIZE-1);
    dir->name[FILE_NAME_MAXIMUM_SIZE-1] = '\0';
    dir->parent = NULL;
    dir->child = NULL;
    dir->next = dir->prev = NULL;
    dir->sizeOfTheFile = 0;
    dir->totalBlockCOUNT = 0;
    insert_child_node(cwd, dir);
    printf("Directory '%s' created successfully.\n", name);
}

void changeDirectory()  {
    char buf[MAXIMUM_COMMAND_LENGTH];
    printf("Enter directory to change to: ");
    if (!readInput(buf))  { printf("Input error\n"); return; }
    char *arg = trim(buf);
    if (strcmp(arg, "/") == 0)  {
        cwd = root;
        printf("Moved to /\n");
        return;
    }
    if (strcmp(arg, "..") == 0)  {
        if (cwd->parent)  {
            cwd = cwd->parent;
            if (cwd == root) printf("Moved to /\n");
            else printf("Moved to %s\n", cwd->name);
        } else  {
            printf("No parent directory (already at root)\n");
        }
        return;
    }
    file_Or_Directory_Node *target = find_child(cwd, arg);
    if (!target)  { printf("Directory '%s' not found.\n", arg); return; }
    if (target->isFile)  { printf("'%s' is not a directory.\n", arg); return; }
    cwd = target;
    printf("Moved to %s\n", (cwd == root) ? "/" : cwd->name);
}

void createFile()  {
    char buf[MAXIMUM_COMMAND_LENGTH];
    printf("Enter file name: ");
    if (!readInput(buf))  { printf("Input error\n"); return; }
    char *name = trim(buf);
    if (strlen(name) < FILE_NAME_MINIMUM_SIZE || strlen(name) >= FILE_NAME_MAXIMUM_SIZE)  {
        printf("Invalid name length.\n"); return;
    }
    if (find_child(cwd, name))  {
        printf("A file or directory with that name already exists.\n"); return;
    }
    file_Or_Directory_Node *f = (file_Or_Directory_Node*) malloc(sizeof(file_Or_Directory_Node));
    if (!f)  { printf("Memory allocation error\n"); return; }
    f->isFile = 1;
    strncpy(f->name, name, FILE_NAME_MAXIMUM_SIZE-1);
    f->name[FILE_NAME_MAXIMUM_SIZE-1] = '\0';
    f->parent = NULL;
    f->child = NULL;
    f->next = f->prev = NULL;
    f->sizeOfTheFile = 0;
    f->totalBlockCOUNT = 0;
    for (int i=0;i<NUMBER_OF_BLOCKS;i++) f->blockPointer[i] = -1;
    insert_child_node(cwd, f);
    printf("File '%s' created successfully.\n", name);
}

void writeFile()  {
    char buf_name[MAXIMUM_COMMAND_LENGTH];
    char buf_content[MAXIMUM_DATA_LENGTH];
    printf("Enter filename: ");
    if (!readInput(buf_name))  { printf("Input error\n"); return; }
    char *name = trim(buf_name);
    file_Or_Directory_Node *f = find_child(cwd, name);
    if (!f)  { printf("File '%s' not found.\n", name); return; }
    if (!f->isFile)  { printf("'%s' is a directory.\n", name); return; }

    printf("Enter content (you may include spaces): ");
    if (!readInput(buf_content))  { printf("Input error\n"); return; }
    char *content = buf_content;

    int bytes = (int) strlen(content);
    int neededBlocks = blocks_needed(bytes);

    int freeAvailable = count_free_blocks() + f->totalBlockCOUNT;
    if (neededBlocks > freeAvailable)  {
        printf("Not enough free space. Required blocks: %d, available blocks: %d\n", neededBlocks, freeAvailable);
        return;
    }

    for (int i=0;i<f->totalBlockCOUNT;i++) {
        int b = f->blockPointer[i];
        if (b >= 0) free_block_to_tail(b);
        f->blockPointer[i] = -1;
    }
    f->totalBlockCOUNT = 0;
    f->sizeOfTheFile = 0;

    if (neededBlocks == 0)  {
        printf("Data written successfully (size=0 bytes).\n");
        return;
    }

    for (int i=0;i<neededBlocks;i++) {
        int idx = allocate_block_from_head();
        if (idx == -1)  {
            for (int j=0;j<f->totalBlockCOUNT;j++) { free_block_to_tail(f->blockPointer[j]); }
            f->totalBlockCOUNT = 0;
            f->sizeOfTheFile = 0;
            printf("Unexpected: ran out of blocks while writing.\n");
            return;
        }
        f->blockPointer[f->totalBlockCOUNT++] = idx;
    }

    int written = 0;
    for (int i=0;i<f->totalBlockCOUNT;i++) {
        int idx = f->blockPointer[i];
        int to_write = (bytes - written) > BLOCK_SIZE ? BLOCK_SIZE : (bytes - written);
        if (to_write > 0)  {
            memcpy(diskSpace[idx], content + written, (size_t)to_write);
            if (to_write < BLOCK_SIZE) memset(diskSpace[idx] + to_write, 0, BLOCK_SIZE - to_write);
        } else  {
            memset(diskSpace[idx], 0, BLOCK_SIZE);
        }
        written += to_write;
    }
    f->sizeOfTheFile = bytes;
    printf("Data written successfully (size=%d bytes).\n", bytes);
}

void readFile()  {
    char buf[MAXIMUM_COMMAND_LENGTH];
    printf("Enter filename to read: ");
    if (!readInput(buf))  { printf("Input error\n"); return; }
    char *name = trim(buf);
    file_Or_Directory_Node *f = find_child(cwd, name);
    if (!f)  { printf("File '%s' not found.\n", name); return; }
    if (!f->isFile)  { printf("'%s' is a directory.\n", name); return; }
    if (f->totalBlockCOUNT == 0 || f->sizeOfTheFile == 0)  {
        printf("(empty)\n");
        return;
    }
    int remaining = f->sizeOfTheFile;
    for (int i=0;i<f->totalBlockCOUNT;i++) {
        int idx = f->blockPointer[i];
        int to_read = remaining > BLOCK_SIZE ? BLOCK_SIZE : remaining;
        if (to_read > 0)  {
            fwrite(diskSpace[idx], 1, (size_t)to_read, stdout);
            remaining -= to_read;
        }
    }
    printf("\n");
}

void DeleteFile()  {
    char buf[MAXIMUM_COMMAND_LENGTH];
    printf("Enter filename to delete: ");
    if (!readInput(buf))  { printf("Input error\n"); return; }
    char *name = trim(buf);
    file_Or_Directory_Node *f = find_child(cwd, name);
    if (!f)  { printf("File '%s' not found.\n", name); return; }
    if (!f->isFile)  { printf("'%s' is a directory.\n", name); return; }

    remove_child_node(cwd, f);
    for (int i=0;i<f->totalBlockCOUNT;i++) {
        int idx = f->blockPointer[i];
        if (idx >= 0) free_block_to_tail(idx);
    }
    free(f);
    printf("File deleted successfully.\n");
}

void RemoveDirectory()  {
    char buf[MAXIMUM_COMMAND_LENGTH];
    printf("Enter directory name to remove: ");
    if (!readInput(buf))  { printf("Input error\n"); return; }
    char *name = trim(buf);
    file_Or_Directory_Node *d = find_child(cwd, name);
    if (!d)  { printf("Directory '%s' not found.\n", name); return; }
    if (d->isFile)  { printf("'%s' is not a directory.\n", name); return; }
    if (d->child)  { printf("Directory '%s' is not empty.\n", name); return; }
    remove_child_node(cwd, d);
    free(d);
    printf("Directory removed successfully.\n");
}

void ShowCurrentDirectory()  {
    if (!cwd->child)  {
        printf("(empty)\n");
    } else  {
        file_Or_Directory_Node *cur = cwd->child;
        do  {
            if (cur->isFile) printf("%s\n", cur->name);
            else printf("%s/\n", cur->name);
            cur = cur->next;
        } while(cur != cwd->child);
    }
}

void ShowDiskImage()  {
    int freeCount = count_free_blocks();
    printf("Total Blocks: %d\n", NUMBER_OF_BLOCKS);
    printf("Used Blocks: %d\n", usedBlockCount);
    printf("Free Blocks: %d\n", freeCount);
    double usage = 0.0;
    if (NUMBER_OF_BLOCKS > 0) usage = ((double)usedBlockCount / (double)NUMBER_OF_BLOCKS) * 100.0;
    printf("Disk Usage: %.2f%%\n", usage);
}

void TerminateProgram()  {
    if (root)  {
        while (root->child)  {
            file_Or_Directory_Node *c = root->child;
            remove_child_node(root, c);
            free_node_recursive(c);
        }
        free(root);
        root = NULL;
        cwd = NULL;
    }
    freeBlock *p = freeBlocksHead;
    while(p)  {
        freeBlock *n = p->next;
        free(p);
        p = n;
    }
    freeBlocksHead = freeBlocksTail = NULL;
    if (diskSpace)  {
        for (int i=0;i<NUMBER_OF_BLOCKS;i++)  {
            if (diskSpace[i]) free(diskSpace[i]);
        }
        free(diskSpace);
        diskSpace = NULL;
    }
    printf("Memory released. Exiting program...\n");
}

void print_pwd()  {
    if (cwd == root)  { printf("/\n"); return; }
    char path[4096] =  {0};
    file_Or_Directory_Node *cur = cwd;
    char segments[64][FILE_NAME_MAXIMUM_SIZE];
    int segc = 0;
    while (cur && cur != root)  {
        strncpy(segments[segc++], cur->name, FILE_NAME_MAXIMUM_SIZE-1);
        segments[segc-1][FILE_NAME_MAXIMUM_SIZE-1] = '\0';
        cur = cur->parent;
    }
    for (int i = segc-1; i >= 0; --i)  {
        strcat(path, "/");
        strcat(path, segments[i]);
    }
    if (strlen(path) == 0) strcpy(path, "/");
    printf("%s\n", path);
}

int main()  {
    printf("Virtual File System\n");
    diskSpace = (char**) malloc(NUMBER_OF_BLOCKS * sizeof(char*));
    if (!diskSpace)  {
        fprintf(stderr, "Failed to allocate diskSpace pointers\n");
        return 1;
    }
    for(int i=0;i<NUMBER_OF_BLOCKS;i++) {
        diskSpace[i] = (char*) malloc(BLOCK_SIZE * sizeof(char));
        if (!diskSpace[i])  {
            fprintf(stderr, "Failed allocating block %d\n", i);
            for (int j=0;j<i;j++) free(diskSpace[j]);
            free(diskSpace);
            return 1;
        }
        memset(diskSpace[i], 0, BLOCK_SIZE);
    }

    initializeSystem(diskSpace);

    char line[MAXIMUM_COMMAND_LENGTH];
    while(1) {
        if (cwd == root) printf("/ > ");
        else printf("%s > ", cwd->name);

        if (!readInput(line))  {
            printf("\n");
            TerminateProgram();
            break;
        }
        char *cmdline = trim(line);
        if (strlen(cmdline) == 0) continue;

        char *token = strtok(cmdline, " ");
        if (!token) continue;

        if (strcmp(token, "mkdir") == 0)  {
            mkdir();
        } else if (strcmp(token, "ls") == 0)  {
            ShowCurrentDirectory();
        } else if (strcmp(token, "create") == 0)  {
            createFile();
        } else if (strcmp(token, "write") == 0)  {
            writeFile();
        } else if (strcmp(token, "read") == 0)  {
            readFile();
        } else if (strcmp(token, "delete") == 0)  {
            DeleteFile();
        } else if (strcmp(token, "rmdir") == 0)  {
            RemoveDirectory();
        } else if (strcmp(token, "cd") == 0)  {
            changeDirectory();
        } else if (strcmp(token, "pwd") == 0)  {
            print_pwd();
        } else if (strcmp(token, "df") == 0)  {
            ShowDiskImage();
        } else if (strcmp(token, "exit") == 0)  {
            TerminateProgram();
            break;
        } else  {
            printf("Unknown command: %s\n", token);
            printf("Supported commands: mkdir, ls, create, write, read, delete, rmdir, cd, pwd, df, exit\n");
        }
    }

    return 0;
}