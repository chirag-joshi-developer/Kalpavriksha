#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PRODUCTNAMEMAXIMUMLENGTH 51
#define MINIMUMPRODUCTID 1
#define MAXIMUMPRODCTID 1000
#define MINIMUMPRODUCTPRICE 0
#define MAXIMUMPRODUCTPRICE 100000
#define MINIMUMPRODUCTQUANTITY 0
#define MAXIMUMPRODUCTQUANTITY 1000000

typedef struct {
    int productId;
    char productName[PRODUCTNAMEMAXIMUMLENGTH];
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

void readAndStoreInitialData(productInfo* inventory, int numberOfProductsInStore) {
    for (int i = 0; i < numberOfProductsInStore; i++) {
        productInfo* current = inventory + i;
        printf("\nEnter details for product %d\n", i + 1);

        printf("Enter Product ID (%d-%d): ", MINIMUMPRODUCTID, MAXIMUMPRODCTID);
        if (scanf("%d", &(current->productId)) != 1 ||
            current->productId < MINIMUMPRODUCTID ||
            current->productId > MAXIMUMPRODCTID) {
            printf("Invalid Product ID. Setting default ID = %d\n", MINIMUMPRODUCTID);
            current->productId = MINIMUMPRODUCTID;
            while (getchar() != '\n');
        }

        printf("Enter Product Name: ");
        scanf(" %[^\n]", current->productName);
        if (strlen(current->productName) == 0 || strlen(current->productName) >= PRODUCTNAMEMAXIMUMLENGTH) {
            printf("Invalid Product Name Length. Setting default name = 'Unknown'\n");
            strcpy(current->productName, "Unknown");
        }

        printf("Enter Product Price (%d-%d): ", MINIMUMPRODUCTPRICE, MAXIMUMPRODUCTPRICE);
        if (scanf("%ld", &(current->productPrice)) != 1 ||
            current->productPrice < MINIMUMPRODUCTPRICE ||
            current->productPrice > MAXIMUMPRODUCTPRICE) {
            printf("Invalid Price. Setting default price = %d\n", MINIMUMPRODUCTPRICE);
            current->productPrice = MINIMUMPRODUCTPRICE;
            while (getchar() != '\n');
        }

        printf("Enter Product Quantity (%d-%d): ", MINIMUMPRODUCTQUANTITY, MAXIMUMPRODUCTQUANTITY);
        if (scanf("%ld", &(current->productQuantity)) != 1 ||
            current->productQuantity < MINIMUMPRODUCTQUANTITY ||
            current->productQuantity > MAXIMUMPRODUCTQUANTITY) {
            printf("Invalid Quantity. Setting default quantity = %d\n", MINIMUMPRODUCTQUANTITY);
            current->productQuantity = MINIMUMPRODUCTQUANTITY;
            while (getchar() != '\n');
        }
    }
}

void addNewProduct(productInfo** inventory, int* numberOfProductsInStore) {
    *inventory = realloc(*inventory, (*numberOfProductsInStore + 1) * sizeof(productInfo));
    if (*inventory == NULL) {
        printf("Memory allocation failed. Returning to main menu.\n");
        return;
    }

    productInfo* newProduct = *inventory + *numberOfProductsInStore;

    printf("Enter Product ID (%d-%d): ", MINIMUMPRODUCTID, MAXIMUMPRODCTID);
    if (scanf("%d", &(newProduct->productId)) != 1 ||
        newProduct->productId < MINIMUMPRODUCTID ||
        newProduct->productId > MAXIMUMPRODCTID) {
        printf("Invalid Product ID. Setting default ID = %d\n", MINIMUMPRODUCTID);
        newProduct->productId = MINIMUMPRODUCTID;
        while (getchar() != '\n');
    }

    printf("Enter Product Name: ");
    scanf(" %[^\n]", newProduct->productName);
    if (strlen(newProduct->productName) == 0 || strlen(newProduct->productName) >= PRODUCTNAMEMAXIMUMLENGTH) {
        printf("Invalid Product Name Length. Setting default name = 'Unknown'\n");
        strcpy(newProduct->productName, "Unknown");
    }

    printf("Enter Product Price (%d-%d): ", MINIMUMPRODUCTPRICE, MAXIMUMPRODUCTPRICE);
    if (scanf("%ld", &(newProduct->productPrice)) != 1 ||
        newProduct->productPrice < MINIMUMPRODUCTPRICE ||
        newProduct->productPrice > MAXIMUMPRODUCTPRICE) {
        printf("Invalid Price. Setting default = %d\n", MINIMUMPRODUCTPRICE);
        newProduct->productPrice = MINIMUMPRODUCTPRICE;
        while (getchar() != '\n');
    }

    printf("Enter Product Quantity (%d-%d): ", MINIMUMPRODUCTQUANTITY, MAXIMUMPRODUCTQUANTITY);
    if (scanf("%ld", &(newProduct->productQuantity)) != 1 ||
        newProduct->productQuantity < MINIMUMPRODUCTQUANTITY ||
        newProduct->productQuantity > MAXIMUMPRODUCTQUANTITY) {
        printf("Invalid Quantity. Setting default = %d\n", MINIMUMPRODUCTQUANTITY);
        newProduct->productQuantity = MINIMUMPRODUCTQUANTITY;
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

    printf("Enter new quantity (%d-%d): ", MINIMUMPRODUCTQUANTITY, MAXIMUMPRODUCTQUANTITY);
    if (scanf("%ld", &(product->productQuantity)) != 1 ||
        product->productQuantity < MINIMUMPRODUCTQUANTITY ||
        product->productQuantity > MAXIMUMPRODUCTQUANTITY) {
        printf("Invalid quantity. Quantity unchanged.\n");
        while (getchar() != '\n');
    } else {
        printf("Quantity updated successfully.\n");
    }
}

void toLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
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
    char name[PRODUCTNAMEMAXIMUMLENGTH];
    printf("Enter part or full Product Name to search: ");
    scanf(" %[^\n]", name);

    char searchTerm[PRODUCTNAMEMAXIMUMLENGTH];
    strcpy(searchTerm, name);
    toLowerCase(searchTerm);

    int found = 0;
    for (int i = 0; i < numberOfProductsInStore; i++) {
        char productNameLower[PRODUCTNAMEMAXIMUMLENGTH];
        strcpy(productNameLower, inventory[i].productName);
        toLowerCase(productNameLower);

        if (strstr(productNameLower, searchTerm)) {
            displayProductData(&inventory[i]);
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
        if (inventory[i].productPrice >= lower && inventory[i].productPrice <= upper) {
            displayProductData(&inventory[i]);
            found = 1;
        }
    }

    if (!found) printf("No products found in this price range.\n");
}

void deleteProduct(productInfo* inventory, int* numberOfProductsInStore) {
    int id;
    printf("Enter Product ID to delete: ");
    scanf("%d", &id);

    int foundIndex = -1;
    for (int i = 0; i < *numberOfProductsInStore; i++) {
        if (inventory[i].productId == id) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        printf("Product not found.\n");
        return;
    }

    for (int i = foundIndex; i < *numberOfProductsInStore - 1; i++) {
        inventory[i] = inventory[i + 1];
    }

    (*numberOfProductsInStore)--;
    printf("Product deleted successfully.\n");
}

int main() {
    int numberOfProductsInStore = 0;
    printf("Enter the initial number of products: ");
    if (scanf("%d", &numberOfProductsInStore) != 1 ||
        numberOfProductsInStore <= 0 ||
        numberOfProductsInStore > 100) {
        printf("Invalid number entered. Defaulting to 1.\n");
        numberOfProductsInStore = 1;
        while (getchar() != '\n');
    }

    productInfo* inventory = (productInfo*) calloc(numberOfProductsInStore, sizeof(productInfo));
    readAndStoreInitialData(inventory, numberOfProductsInStore);

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
                deleteProduct(inventory, &numberOfProductsInStore);
                break;
            case 8:
                printf("Exiting program...\n");
                free(inventory);
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (operation != 8);

    return 0;
}
