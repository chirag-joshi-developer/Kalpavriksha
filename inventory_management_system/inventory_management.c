#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PRODUCT_NAME_MAXIMUM_LENGTH 51
#define MINIMUM_PRODUCT_ID 1
#define MAXIMUM_PRODUCT_ID 10000
#define MINIMUM_PRODUCT_PRICE 0
#define MAXIMUM_PRODUCT_PRICE 100000
#define MINIMUM_PRODUCT_QUANTITY 0
#define MAXIMUM_PRODUCT_QUANTITY 1000000

typedef struct {
    int productId;
    char productName[PRODUCT_NAME_MAXIMUM_LENGTH];
    long productPrice;
    long productQuantity;
} productInfo;

void displayProductData(productInfo* product) {
    printf("Product ID: %d\n", product->productId);
    printf("Product Name: %s\n", product->productName);
    printf("Product Price: %ld\n", product->productPrice);
    printf("Product Quantity: %ld\n\n", product->productQuantity);
}

productInfo* findProductFromInventoryByID(productInfo* inventory, int productID, int numberOfProductsInStore) {
    for (int i = 0; i < numberOfProductsInStore; i++) {
        if ((inventory + i)->productId == productID) {
            return inventory + i;
        }
    }
    return NULL;
}

void toLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

int findProductFromInventoryByName(productInfo* inventory, char* name, int totalProducts) {
    char tempName1[PRODUCT_NAME_MAXIMUM_LENGTH];
    char tempName2[PRODUCT_NAME_MAXIMUM_LENGTH];

    strcpy(tempName1, name);
    toLowerCase(tempName1);

    for (int i = 0; i < totalProducts; i++) {
        strcpy(tempName2, inventory[i].productName);
        toLowerCase(tempName2);

        if (strcmp(tempName1, tempName2) == 0) {
            return 1; 
        }
    }
    return 0; 
}


void readAndStoreInitialData(productInfo* inventory, int numberOfProductsInStore,int startingPoint) {
    int currentProductCount = startingPoint;
    for (int i = startingPoint; i < numberOfProductsInStore; i++) {
        productInfo* current = inventory + i;
        printf("\nEnter details for product %d\n", i + 1);

        printf("Enter Product ID (%d-%d): ", MINIMUM_PRODUCT_ID, MAXIMUM_PRODUCT_ID);
        int temporaryProductId;
        char temporaryProductName [51];

        while (scanf("%d", &temporaryProductId) != 1 ||
            temporaryProductId < MINIMUM_PRODUCT_ID ||
            temporaryProductId > MAXIMUM_PRODUCT_ID) {
            printf("Invalid Product ID\n");
            printf("Enter Product ID (%d-%d): ", MINIMUM_PRODUCT_ID, MAXIMUM_PRODUCT_ID);
            while (getchar() != '\n');
        }
        currentProductCount++;
        if(findProductFromInventoryByID(inventory, temporaryProductId, currentProductCount)){
            printf("Product of this id already exists\n");
            printf("Enter product details again \n");
            i--;
            continue;
        }
        else{
            current->productId = temporaryProductId;
        }

        printf("Enter Product Name: ");
        while (1) {
            while (getchar() != '\n');
            if (fgets(temporaryProductName, PRODUCT_NAME_MAXIMUM_LENGTH, stdin) == NULL) {
                printf("Input error. Try again:\n");
                continue;
            }
            temporaryProductName[strcspn(temporaryProductName, "\n")] = '\0';
            break;
        }
        if (findProductFromInventoryByName(inventory, temporaryProductName, currentProductCount)) {
            printf("Product with this name already exists.\n");
            printf("Enter product details again.\n");
            i--;
            continue;
        } else {
            strcpy(current->productName, temporaryProductName);
        }
        printf("Enter Product Price (%d-%d): ", MINIMUM_PRODUCT_PRICE, MAXIMUM_PRODUCT_PRICE);
        while(scanf("%ld", &(current->productPrice)) != 1 ||
            current->productPrice < MINIMUM_PRODUCT_PRICE ||
            current->productPrice > MAXIMUM_PRODUCT_PRICE) {
            printf("Invalid Price\n");
            printf("Enter Product Price (%d-%d): ", MINIMUM_PRODUCT_PRICE, MAXIMUM_PRODUCT_PRICE);
            while (getchar() != '\n');
        }

        printf("Enter Product Quantity (%d-%d): ", MINIMUM_PRODUCT_QUANTITY, MAXIMUM_PRODUCT_QUANTITY);
        while(scanf("%ld", &(current->productQuantity)) != 1 ||
            current->productQuantity < MINIMUM_PRODUCT_QUANTITY ||
            current->productQuantity > MAXIMUM_PRODUCT_QUANTITY) {
            printf("Invalid Quantity.\n");
            printf("Enter Product Quantity (%d-%d): ", MINIMUM_PRODUCT_QUANTITY, MAXIMUM_PRODUCT_QUANTITY);
            while (getchar() != '\n');
        }
    }
}

void addNewProduct(productInfo** inventory, int* numberOfProductsInStore) {
    productInfo * temp = realloc(*inventory, (*numberOfProductsInStore + 1) * sizeof(productInfo)); 
    if (temp == NULL) {
        printf("Memory allocation failed. Returning to main menu.\n");
        return;
    }
    else{
        *inventory = temp;
    }

    int temporaryProductId;
    char *temporaryProductName = (char*)malloc(PRODUCT_NAME_MAXIMUM_LENGTH*sizeof(char));

    productInfo* newProduct = *inventory + *numberOfProductsInStore;

    printf("Enter Product ID (%d-%d): ", MINIMUM_PRODUCT_ID, MAXIMUM_PRODUCT_ID);
    while(scanf("%d", &temporaryProductId) != 1 ||
        temporaryProductId < MINIMUM_PRODUCT_ID ||
        temporaryProductId > MAXIMUM_PRODUCT_ID) {
        printf("Invalid Product ID.");
        printf("Enter Product ID (%d-%d): ", MINIMUM_PRODUCT_ID, MAXIMUM_PRODUCT_ID);
        while (getchar() != '\n');
    }

    if(findProductFromInventoryByID(*inventory, temporaryProductId, *numberOfProductsInStore)){
        printf("Product of this id already exists\n");
        printf("Enter product details again \n");
        addNewProduct(inventory,numberOfProductsInStore);
        return;
    }
    else{
        newProduct->productId = temporaryProductId;
    }

    printf("Enter Product Name: ");
    while (1) {
        while (getchar() != '\n');
        if (fgets(temporaryProductName, PRODUCT_NAME_MAXIMUM_LENGTH, stdin) == NULL) {
            printf("Input error. Try again:\n");
            continue;
        }
        temporaryProductName[strcspn(temporaryProductName, "\n")] = '\0';
        break;
    }
    
    if (findProductFromInventoryByName(*inventory, temporaryProductName, *numberOfProductsInStore)) {
        printf("Product with this name already exists.\n");
        printf("Enter product details again.\n");
        addNewProduct(inventory, numberOfProductsInStore);
        free(temporaryProductName);
        return;
    } else {
        strcpy(newProduct->productName, temporaryProductName);
    }

    printf("Enter Product Price (%d-%d): ", MINIMUM_PRODUCT_PRICE, MAXIMUM_PRODUCT_PRICE);
    while(scanf("%ld", &(newProduct->productPrice)) != 1 ||
        newProduct->productPrice < MINIMUM_PRODUCT_PRICE ||
        newProduct->productPrice > MAXIMUM_PRODUCT_PRICE) {
        printf("Invalid Price.\n"); 
        printf("Enter Product Price (%d-%d): ", MINIMUM_PRODUCT_PRICE, MAXIMUM_PRODUCT_PRICE);
        while (getchar() != '\n');
    }

    printf("Enter Product Quantity (%d-%d): ", MINIMUM_PRODUCT_QUANTITY, MAXIMUM_PRODUCT_QUANTITY);
    while(scanf("%ld", &(newProduct->productQuantity)) != 1 ||
        newProduct->productQuantity < MINIMUM_PRODUCT_QUANTITY ||
        newProduct->productQuantity > MAXIMUM_PRODUCT_QUANTITY) {
        printf("Invalid Quantity.");
        printf("Enter Product Quantity (%d-%d): ", MINIMUM_PRODUCT_QUANTITY, MAXIMUM_PRODUCT_QUANTITY);
        while (getchar() != '\n');
    }

    (*numberOfProductsInStore)++;
    printf("New product added successfully!\n\n");
}

void showEntireInventory(productInfo* inventory, int numberOfProductsInStore) {
    if (numberOfProductsInStore == 0) {
        printf("Inventory is empty!\n");
        return;
    }
    for (int i = 0; i < numberOfProductsInStore; i++) {
        displayProductData(inventory + i);
    }
}

void updateProductQuantity(productInfo* inventory, int numberOfProductsInStore) {
    int id;
    printf("Enter Product ID to update quantity: ");
    scanf("%d", &id);

    productInfo* product = findProductFromInventoryByID(inventory, id, numberOfProductsInStore);
    if (product == NULL) {
        printf("Product not found.\n");
        return;
    }

    printf("Enter new quantity (%d-%d): ", MINIMUM_PRODUCT_QUANTITY, MAXIMUM_PRODUCT_QUANTITY);
    if (scanf("%ld", &(product->productQuantity)) != 1 ||
        product->productQuantity < MINIMUM_PRODUCT_QUANTITY ||
        product->productQuantity > MAXIMUM_PRODUCT_QUANTITY) {
        printf("Invalid quantity. Quantity unchanged.\n");
        while (getchar() != '\n');
    } else {
        printf("Quantity updated successfully.\n");
    }
}

void searchProductByID(productInfo* inventory, int numberOfProductsInStore) {
    int id;
    printf("Enter Product ID to search: ");
    scanf("%d", &id);

    productInfo* product = findProductFromInventoryByID(inventory, id, numberOfProductsInStore);
    if (product == NULL) {
        printf("Product not found.\n");
    } else {
        displayProductData(product);
    }
}

void searchProductByName(productInfo* inventory, int numberOfProductsInStore) {
    char *name = (char*) malloc(PRODUCT_NAME_MAXIMUM_LENGTH*sizeof(char));
    printf("Enter part or full Product Name to search: ");
    scanf(" %[^\n]", name);

    char searchTerm[PRODUCT_NAME_MAXIMUM_LENGTH];
    strcpy(searchTerm, name);
    toLowerCase(searchTerm);

    int found = 0;
    for (int i = 0; i < numberOfProductsInStore; i++) {
        char productNameLower[PRODUCT_NAME_MAXIMUM_LENGTH];
        strcpy(productNameLower, (inventory+i)->productName);
        toLowerCase(productNameLower);

        if (strstr(productNameLower, searchTerm)) {
            displayProductData((inventory+i));
            found = 1;
        }
    }

    if (!found) printf("No matching products found.\n");
}

void searchProductByPriceRange(productInfo* inventory, int numberOfProductsInStore) {
    long lower, upper;
    printf("Enter lower bound of price: ");
    scanf("%ld", &lower);
    printf("Enter upper bound of price: ");
    scanf("%ld", &upper);

    int found = 0;
    for (int i = 0; i < numberOfProductsInStore; i++) {
        if ((inventory+i)->productPrice >= lower && (inventory+i)->productPrice <= upper) {
            displayProductData((inventory+i));
            found = 1;
        }
    }

    if (!found) printf("No products found in this price range.\n");
}

void deleteProduct(productInfo** inventory, int* numberOfProductsInStore) {
    int id;
    printf("Enter Product ID to delete: ");
    scanf("%d", &id);

    int foundIndex = -1;
    for (int i = 0; i < *numberOfProductsInStore; i++) {
        if (((*inventory)+i)->productId == id) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        printf("Product not found.\n");
        return;
    }

    for (int i = foundIndex; i < *numberOfProductsInStore - 1; i++) {
        *((*inventory)+i) = *(*inventory+(i + 1));
    }

    (*numberOfProductsInStore)--;
    if(*numberOfProductsInStore == 0){
        free(*inventory);
        *inventory = NULL;
        printf("Last product deleted successfully memory is freed.\n");
        return;
    }
    
    productInfo* temp = (productInfo*)realloc(*inventory,(*numberOfProductsInStore)*sizeof(productInfo));
    
    if(temp != NULL ){
        *inventory = temp;
    }
    else{
        printf("Product deleted but memory reallocation failed ");
        return;
    }
    
    printf("Product deleted successfully.\n");
}

int main() {
    int numberOfProductsInStore = 0;
    printf("Enter the initial number of products: ");
    while(scanf("%d", &numberOfProductsInStore) != 1 ||
        numberOfProductsInStore <= 0 ||
        numberOfProductsInStore > 100) {
        printf("Invalid initial number entered it must be between 1 and 100.\n");
        while (getchar() != '\n');
        printf("Enter the initial number of products: ");
    }

    productInfo* inventory = (productInfo*) calloc(numberOfProductsInStore, sizeof(productInfo));
    readAndStoreInitialData(inventory, numberOfProductsInStore,0);

    int operation = 0;
    do {
        printf("\nSelect an operation:\n");
        printf("1. Add New Product\n");
        printf("2. View All Products\n");
        printf("3. Update Quantity\n");
        printf("4. Search by ID\n");
        printf("5. Search by Name\n");
        printf("6. Search by Price Range\n");
        printf("7. Delete Product\n");
        printf("8. Exit\n");
        scanf("%d", &operation);

        switch (operation) {
            case 1:
                addNewProduct(&inventory, &numberOfProductsInStore);
                break;
            case 2:
                showEntireInventory(inventory, numberOfProductsInStore);
                break;
            case 3:
                updateProductQuantity(inventory, numberOfProductsInStore);
                break;
            case 4:
                searchProductByID(inventory, numberOfProductsInStore);
                break;
            case 5:
                searchProductByName(inventory, numberOfProductsInStore);
                break;
            case 6:
                searchProductByPriceRange(inventory, numberOfProductsInStore);
                break;
            case 7:
                deleteProduct(&inventory, &numberOfProductsInStore);
                break;
            case 8:
                printf("Exiting program\n");
                free(inventory);
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (operation != 8);

    return 0;
}