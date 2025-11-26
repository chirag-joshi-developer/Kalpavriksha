#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXIMUM_VALUE_LENGTH 50
#define MINIMUM_CACHE_SIZE 1
#define MAXIMUM_CACHE_SIZE 1000

typedef struct QNode{
    int key;
    char value[MAXIMUM_VALUE_LENGTH];
    struct QNode* prev;
    struct QNode* next;
}QNode;

typedef struct Queue{
    QNode* head;
    QNode* tail;
    int totalCount;
}Queue;

typedef struct HashNode{
    int key;
    QNode* targeNode;
    struct HashNode* next;
}HashNode;

typedef struct HashMap{
    HashNode** buckets;
    int capacity;
}HashMap;

typedef struct LRUCache{
    int capacity;
    Queue* Queue;
    HashMap* map;
}LRUCache;

static LRUCache* lruCache = NULL;

static Queue* createQueue(){
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    if(queue == NULL){
        return NULL;
    }

    queue->head = NULL;
    queue->tail = NULL;
    queue->totalCount = 0;
    return queue;
}

static QNode* createQNode(int key, const char* value){
    QNode* Node = (QNode*)malloc(sizeof(QNode));
    if(Node == NULL){
        return NULL;
    }
    Node->key = key;
    strncpy(Node->value,value,MAXIMUM_VALUE_LENGTH-1);
    Node->value[MAXIMUM_VALUE_LENGTH-1] = '\0';
    Node->prev = NULL;
    Node->next = NULL;
    return Node;
}

static void enqueueFront(Queue* queue ,QNode* Node){
    if(queue == NULL || Node == NULL){
        return ;
    }
    Node->next = queue->head;
    Node->prev = NULL;

    if(queue->head != NULL){
        queue->head->prev = Node;
    }
    queue->head = Node;
    if(queue->tail == NULL){
        queue->tail = Node;
    }
    queue->totalCount++;
}

static void detachNode(Queue* queue, QNode* Node) {
    if (queue == NULL || Node == NULL) return;

    if (Node->prev != NULL) {
        Node->prev->next = Node->next;
    } else {
        queue->head = Node->next;
    }

    if (Node->next != NULL) {
        Node->next->prev = Node->prev;
    } else {
        queue->tail = Node->prev;
    }
    
    Node->next = NULL;
    Node->prev = NULL;
    queue->totalCount--;
}

static QNode* popTail(Queue* queue){
    if(queue == NULL || queue->tail == NULL){
        return NULL;
    }
    QNode* lruNode = queue->tail;
    detachNode(queue, lruNode);
    return lruNode;
}

static int getHashIndex(int key, int capacity){
    int index = key % capacity;
    return (index<0) ? (index+capacity) : index;
}

static HashMap* createMap(int capacity) {
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if (map == NULL){
        return NULL;
    }
    map->capacity = capacity;
    map->buckets = (HashNode**)calloc(capacity, sizeof(HashNode*));
    
    if (map->buckets == NULL) {
        free(map);
        return NULL;
    }
    return map;
}

static void mapPut(HashMap* map, int key, QNode* targetNode){
    if(map == NULL || targetNode == NULL){
        return ;
    }
    int index = getHashIndex(key,map->capacity);
    HashNode* hNode = (HashNode*)malloc(sizeof(HashNode));
    if (hNode == NULL){
        return;
    }
    hNode->key = key;
    hNode->targeNode = targetNode;
    hNode->next = map->buckets[index];
    map->buckets[index] = hNode;
}

static QNode* getNodeFromMap(HashMap* map,int key){
    if(map == NULL){
        return NULL;
    }
    int index = getHashIndex(key,map->capacity);
    HashNode* current = map->buckets[index];
    while(current != NULL){
        if(current->key == key){
            return current->targeNode;
        }
        current = current->next;
    }
    return NULL;
}

static void removeNodeFromMap(HashMap* map, int key){
    if(map ==  NULL){
        return;
    }
    int index = getHashIndex(key,map->capacity);
    HashNode* current = map->buckets[index];
    HashNode* prev = NULL;
    while(current != NULL){
        if(current->key == key){
            if(prev == NULL){
                map->buckets[index] = current->next;
            }
            else{
                prev->next = current->next;
            }
            free(current);
            return ;
        }
        prev = current;
        current = current->next;
    }
}

static void freeCache(){
    if(lruCache == NULL){
        return;
    }
    if(lruCache->Queue != NULL){
        QNode* curr = lruCache->Queue->head;
        while(curr != NULL){
            QNode* temp = curr;
            curr = curr->next;
            free(temp);
        }
        free(lruCache->Queue);
    }
    if(lruCache->map != NULL){
        if(lruCache->map->buckets != NULL){
            for(int i=0;i<lruCache->map->capacity;i++){
                HashNode* hcurr = lruCache->map->buckets[i];
                while(hcurr != NULL){
                    HashNode* htemp = hcurr;
                    hcurr = hcurr->next;
                    free(htemp);
                }
            }
            free(lruCache->map->buckets);
        }
        free(lruCache->map);
    }

    free(lruCache);
    lruCache = NULL;
}

static void createCache(int capacity){
    if(capacity < MINIMUM_CACHE_SIZE || capacity > MAXIMUM_CACHE_SIZE){
        printf("capacity should be in between %d to %d \n",MINIMUM_CACHE_SIZE,MAXIMUM_CACHE_SIZE);
        return ;
    }
    if(lruCache != NULL){
        freeCache();
    }
    lruCache = (LRUCache*)malloc(sizeof(LRUCache));
    if(lruCache == NULL){
        fprintf(stderr,"Memory allocation failed \n");
        return;
    }
    lruCache->capacity = capacity;
    lruCache->Queue = createQueue();
    lruCache->map = createMap(capacity);
    if(lruCache->Queue == NULL || lruCache->map == NULL){
        fprintf(stderr,"Cache Initialization failed \n");
        freeCache();
        return;
    }
    printf("Cache created with capacity %d\n",capacity);
}

char* get(int key){
    if(lruCache == NULL){
        printf("Cache not initialized \n");
        return NULL;
    }

    QNode* node = getNodeFromMap(lruCache->map,key);
    if(node != NULL){
        detachNode(lruCache->Queue,node);
        enqueueFront(lruCache->Queue,node);
        return node->value;
    }
    return NULL;
}

void put(int key,const char* value){
    if(lruCache == NULL){
        printf("Cache not initialized \n");
        return;
    }

    QNode *existingNode = getNodeFromMap(lruCache->map,key);

    if(existingNode != NULL){
        strncpy(existingNode->value, value, MAXIMUM_VALUE_LENGTH - 1);
        existingNode->value[MAXIMUM_VALUE_LENGTH - 1] = '\0';

        detachNode(lruCache->Queue, existingNode);
        enqueueFront(lruCache->Queue, existingNode);
        printf("Key %d updated.\n", key);
        return;
    }

    if(lruCache->Queue->totalCount >= lruCache->capacity){
        QNode* lruNode = popTail(lruCache->Queue);
        if(lruNode != NULL){
            printf("Evicting key: %d\n",lruNode->key);
            removeNodeFromMap(lruCache->map,lruNode->key);
            free(lruNode);
        }
    }
    QNode* newNode = createQNode(key, value);
    if(newNode == NULL){
        printf("Memory allocation failed \n");
        return;
    }

    enqueueFront(lruCache->Queue,newNode);
    mapPut(lruCache->map,key,newNode);
    printf("Key %d inserted \n",key);
}

static void insertPut(){
    if(lruCache == NULL){
        printf("Cache not initialized \n");
        return;
    }
    int key = 0;
    int ch;
    printf("\nEnter the key: ");
    while(scanf("%d",&key) != 1){
        printf("\nKey should be numeric \n");
        while((ch = getchar()) != '\n' && ch != EOF);
        printf("\nEnter the key: ");
    }
    while ((ch = getchar()) != '\n' && ch != EOF);

    char value[MAXIMUM_VALUE_LENGTH];
    printf("Enter the value: ");

    if (fgets(value, MAXIMUM_VALUE_LENGTH, stdin) == NULL) {
        printf("Error reading value\n");
        return;
    }

    value[strcspn(value, "\n")] = '\0';
    put(key,value);
}

static void insertGet(){
    if(lruCache == NULL){
        printf("Cache not initialized \n");
        return;
    }
    int key = 0;
    int ch;
    printf("\nEnter the key: ");
    while (scanf("%d", &key) != 1) {
        printf("\nKey should be numeric\n");
        while ((ch = getchar()) != '\n' && ch != EOF);

        printf("Enter the key: ");
    }
    while ((ch = getchar()) != '\n' && ch != EOF);

    char* res = get(key);
    printf("\n%s\n", (res ? res : "NULL"));
}

int main(){
    int capacity = 0;
    int ch;
    printf("Enter Cache size");
    while(scanf("%d",&capacity)!=1){
        printf("Capacity should be numeric value\n");
        while ((ch = getchar()) != '\n' && ch != EOF);
        printf("Enter Cache size");
    }

    while ((ch = getchar()) != '\n' && ch != EOF);
    
    if(capacity < MINIMUM_CACHE_SIZE || capacity > MAXIMUM_CACHE_SIZE){
        printf("Capacity should be in between %d to %d \n",MINIMUM_CACHE_SIZE,MAXIMUM_CACHE_SIZE);
        return 0;
    }
    createCache(capacity);
    while(1){
        int choice = 0;

        printf("1. for put\n");
        printf("2. for get\n");
        printf("3. for exit\n");

        printf("Enter choice:");

        while (scanf("%d", &choice) != 1) {
            printf("\nChoice should be numeric value\n");
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("\nEnter choice: ");
        }
        while ((ch = getchar()) != '\n' && ch != EOF);
        if (choice < 1 || choice > 3) {
            printf("\nChoice should be in range of 1 to 3\n");
            continue;
        }
        switch (choice) {
            case 1:
                insertPut();
                break;

            case 2:
                insertGet();
                break;

            case 3:
                freeCache();
                printf("\nexit\n");
                return 0;

            default:
                printf("\nEnter the valid choice\n");
        }
    }
    return 0;
}