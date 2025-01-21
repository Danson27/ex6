/******************
Name: Dan Sonnenblick
ID: 345287882
Assignment: ex6
*******************/


#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128
#define BULBASAUR 0
#define CHARMANDER 3
#define SQUIRTLE 6
#define OFF 0
#define ON 1
#define NOT_FOUND 0
#define FOUND 1
#define ATTACK_WEIGHT 1.5
#define HP_WEIGHT 1.2
#define MIN_FOR_MERGE 2
#define MIN_FOR_SORT 2


// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

///////////////////////////////////////////
/// Functions for displaying Pokedex
///////////////////////////////////////////

void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        return;
    }
    visit(root);
    preOrderGeneric(root->left, visit);
    preOrderGeneric(root->right, visit);

}
void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        return;
    }
    inOrderGeneric(root->left, visit);
    visit(root);
    inOrderGeneric(root->right, visit);
}
void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit){
    if (root == NULL) {
        return;
    }
    postOrderGeneric(root->left, visit);
    postOrderGeneric(root->right, visit);
    visit(root);
}

// Queue functions
Queue *createQueue() {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    return queue;
}
// Function to add a node to the queue (enqueue)
void enqueue(Queue *queue, PokemonNode *node) {
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    newNode->node = node;
    newNode->next = NULL;

    if (queue->rear == NULL) { // If the queue is empty
        queue->front = queue->rear = newNode;
        return;
    }
    queue->rear->next = newNode;
    queue->rear = newNode;
}
// Function to remove a node from the queue (dequeue)
PokemonNode *dequeue(Queue *queue) {
    if (queue->front == NULL) {
        return NULL; // Queue is empty
    }

    QueueNode *temp = queue->front;
    PokemonNode *node = temp->node;
    queue->front = queue->front->next;

    if (queue->front == NULL) { // If the queue becomes empty
        queue->rear = NULL;
    }

    free(temp);
    return node;
}

// Function to check if the queue is empty
int isQueueEmpty(Queue *queue) {
    return queue->front == NULL;
}
// Function to free the queue
void freeQueue(Queue *queue) {
    while (!isQueueEmpty(queue)) {
        dequeue(queue);
    }
    free(queue);
}
void BFSGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        return;
    }

    // Create a queue
    Queue *queue = createQueue();

    // Enqueue the root node
    enqueue(queue, root);

    while (!isQueueEmpty(queue)) {
        // Dequeue the front node
        PokemonNode *current = dequeue(queue);
        // Visit the current node
        visit(current);

        // Enqueue the left and right children
        if (current->left != NULL) {
            enqueue(queue, current->left);
        }
        if (current->right != NULL) {
            enqueue(queue, current->right);
        }
    }

    freeQueue(queue); // Free the queue after BFS
}


// functions for alphabetical print

int sizeOfBFSTree(PokemonNode *root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + sizeOfBFSTree(root->left) + sizeOfBFSTree(root->right);
}

void initNodeArray(NodeArray *na, int cap) {
    na->nodes = (PokemonNode**)malloc(sizeof(PokemonNode*)*cap);
    if (na == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    na->size = 0;
    na->capacity = cap;
}

void collectAll(PokemonNode *root, NodeArray *na) {
    if (root == NULL) {
        return;
    }
    collectAll(root->left, na);
    addNode(na, root);
    collectAll(root->right, na);
}
void addNode(NodeArray *na, PokemonNode *node) {
    if (na->size == na->capacity) {
        na->capacity *= 2;
        na->nodes = (PokemonNode **)realloc(na->nodes, na->capacity * sizeof(PokemonNode *));
        if (na->nodes == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }
    }
    na->nodes[na->size] = node;
    na->size++;
}

int comparePokemonByNameNode(const void *a, const void *b) {
    PokemonNode *nodeA = *(PokemonNode **)a;
    PokemonNode *nodeB = *(PokemonNode **)b;

    // Compare names alphabetically
    return strcmp(nodeA->data->name, nodeB->data->name);
}
void displayAlphabetical(NodeArray *na){
    qsort(na->nodes, na->size, sizeof(PokemonNode*), comparePokemonByNameNode);
    for (int i = 0; i < na->size; i++) {
        printPokemonNode(na->nodes[i]);
    }
}

PokemonNode* findMinimum(PokemonNode* root) {
    while (root && root->left)
        // Traverse to the leftmost node
        root = root->left;
    // Return the minimum node
    return root;
}

// --------------------------------------------------------------
// Display Menu for how to display the Pokedex
// --------------------------------------------------------------
void displayPokedexOptions (OwnerNode *owner)
{
    if (!owner->pokedexRoot)
    {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice)
    {
        case 1:
            BFSGeneric(owner->pokedexRoot, printPokemonNode);
        break;
        case 2:
            preOrderGeneric(owner->pokedexRoot, printPokemonNode);
        break;
        case 3:
            inOrderGeneric(owner->pokedexRoot, printPokemonNode);
        break;
        case 4:
            postOrderGeneric(owner->pokedexRoot, printPokemonNode);
        break;
        case 5:
            int cap = sizeOfBFSTree(owner->pokedexRoot);
        NodeArray *na = malloc (cap * sizeof(NodeArray));
        if (na == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }
        initNodeArray(na, cap);
        collectAll(owner->pokedexRoot, na);
        displayAlphabetical(na);
        free(na->nodes);
        free(na);
        break;
        default:
            printf("Invalid choice.\n");
    }
}
// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------

void enterPokedexMenu(OwnerNode *currentOwner){

    int subChoice;
    do
    {
     printf("\n-- %s's Pokedex Menu --\n", currentOwner->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice)
        {
        case 1:
            int id = readIntSafe("Enter ID to add: ");
            // Declare a flag to print if the Pokémon was successfully added
            //set the flag to off
            int wasAdded = OFF;
            // Add the Pokémon to the owner's Pokedex
            // - currentOwner->pokedexRoot: Root of the owner's Pokedex tree
            // - currentOwner: Pointer to the current owner
            // - id: ID of the Pokémon to add
            // - &wasAdded: Pointer to the flag that indicates success or failure
            currentOwner->pokedexRoot = addPokemon(currentOwner->pokedexRoot, currentOwner, id, &wasAdded);
            if (wasAdded)
                printf("Pokemon %s (ID %d) added. \n ", pokedex[id-1].name, id);
            break;
            case 2:
            //print pokedex
            displayPokedexOptions(currentOwner);
            break;
        case 3:
            //delete pokemon
            if (currentOwner->pokedexRoot == NULL) {
                printf("No Pokemon to release.\n");
                break;
            }
            int choice = readIntSafe("Enter Pokemon ID to release: ");
            if (!isPokemon(currentOwner->pokedexRoot, currentOwner, choice)) {
                printf("No Pokemon with ID %d found.\n", choice);
                break;
            }
            currentOwner->pokedexRoot = freePokemonNode(currentOwner->pokedexRoot, choice);
            break;
        case 4:
            //Pokemon fight
            if (currentOwner->pokedexRoot == NULL) {
                printf("Pokedex is empty.\n");
                break;
            }
            int IDOne = readIntSafe("Enter ID of the first Pokemon: ");
            int IDTwo = readIntSafe("Enter ID of the second Pokemon: ");
            pokemonFight(currentOwner, IDOne, IDTwo);
            break;
        case 5:
            //evolve pokemon

            // Check if the Pokedex is empty
            if (currentOwner->pokedexRoot == NULL) {
                printf("Cannot evolve. Pokedex empty.\n");
                break;
            }
            int idToEvolve = readIntSafe("Enter ID to evolve: ");

            // Check if the Pokémon with the given ID exists in the owner's Pokedex
            if (!isPokemon(currentOwner->pokedexRoot, currentOwner, idToEvolve)) {
                printf("No Pokemon with ID %d found.\n", idToEvolve);
                break;
            }
            // Check if the Pokémon is capable of evolving
            if (!pokedex[idToEvolve-1].CAN_EVOLVE) {
                printf("%s (ID %d) cannot evolve.\n",pokedex[idToEvolve-1].name, idToEvolve);
                break;
            }
            // Evolve the Pokémon
            evolvePokemon(currentOwner, idToEvolve);
            break;
        case 6:
            printf("Back to Main Menu.\n");
            return;
        default:
            printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice)
        {
        case 1:
            openPokedexMenu();
            break;
        case 2:
            if (!ownerHead) {
                printf("No existing Pokedexes.\n");
                break;
            }
            // Get the count of existing Pokedex owners
            int ownerCount = countOwners();
            printf("\nExisting Pokedexes:\n");

            OwnerNode* currentOwner = ownerHead;

            for (int i = 0; i < ownerCount; i++) {
                printf("%d. %s\n", i+1, currentOwner->ownerName);
                currentOwner = currentOwner->next;
            }
            OwnerNode* selectedOwner = NULL;

            int choiceOfPokedex = readIntSafe("Choose a Pokedex by number: ");

            // Determine which owner was selected based on the user's choice
            if (choiceOfPokedex <= 0) {
                // If the user enters a number <= 0, default to the first owner in the list
                selectedOwner = getOwnerAtIndex(0);
            }
            else if (choiceOfPokedex >= ownerCount) {
                // If the user enters a number greater than or equal to the total owners,
                // default to the last owner in the list
                selectedOwner = getOwnerAtIndex(ownerCount);
            }
            else {
                // Otherwise, select the owner corresponding to the (choice - 1) index
                selectedOwner = getOwnerAtIndex(choiceOfPokedex-1);
            }

            printf("\nEntering %s's Pokedex...\n", selectedOwner->ownerName);
            // Enter the menu for the selected Pokedex
            enterPokedexMenu(selectedOwner);

            break;
        case 3:
            deletePokedex();
            break;
        case 4:
            OwnerNode *ownerOne = NULL;
            OwnerNode *ownerTwo = NULL;
            int owners = countOwners();

            // Check if there are at least two owners to perform the merge
            if (owners < MIN_FOR_MERGE) {
                printf("Not enough owners to merge.\n");
                break;
            }
            printf("\n=== Merge Pokedexes ===\n");

            // Get the name of the first owner
            printf("Enter name of first owner: ");
            char* ownerNameOne = NULL;
            ownerNameOne = getDynamicInput();
            if (ownerNameOne == NULL) {
                exit(1);
            }

            // Get the name of the second owner
            printf("Enter name of second owner: ");
            char* ownerNameTwo = NULL;
            ownerNameTwo = getDynamicInput();
            if (ownerNameTwo == NULL) {
                exit(1);
            }
            // Initialize a pointer to traverse the circular linked list of owners
            int i = OFF;
            OwnerNode* current = ownerHead;

            // Traverse the circular list to find the owners by their names
            if (current != NULL) {
                do {
                    // Check if the current owner's name matches the first input
                    if (strcmp(ownerNameOne, current->ownerName) == 0) {
                        ownerOne = current; // Assign the pointer to the first owner
                    }
                    // Check if the current owner's name matches the second input
                    if (strcmp(ownerNameTwo, current->ownerName) == 0) {
                        ownerTwo = current; // Assign the pointer to the second owner
                    }
                    current = current->next; // Move to the next owner in the list
                    i++;
                } while (current != ownerHead); // Continue until the list loops back
            }
            // Call the function to merge the Pokedexes of the two owners
            mergePokedexMenu(ownerOne, ownerTwo);

            // Free the dynamically allocated memory for the owner names
            free(ownerNameOne);
            free(ownerNameTwo);
            break;
        case 5:
            int numOwners = countOwners();

            // Check if there are enough owners to sort
            if (numOwners < MIN_FOR_SORT) {
                printf("0 or 1 owners only => no need to sort.\n");
                break;
            }
            OwnerNode **ownerArray = malloc(sizeof(OwnerNode*) * numOwners);
            if (ownerArray == NULL) {
                exit(1);
            }

            // Populate the array with pointers to the owners in the circular list
            OwnerNode *theCurrent = ownerHead;
            for (int i = 0; i < numOwners; i++) {
                ownerArray[i] = theCurrent;
                theCurrent = theCurrent->next;
            }

            // Sort the array of owner pointers using qsort and a comparison function
            qsort(ownerArray, numOwners, sizeof(OwnerNode*), compareOwnersByName);

            // Update the circular linked list based on the sorted array
            updateCircularListAfterSort(ownerArray, numOwners);

            // Free the dynamically allocated array
            free(ownerArray);

            printf("Owners sorted by name.\n");
            break;
        case 6:
            int amountOfOwner = countOwners();
            if (amountOfOwner == 0) {
                printf("No owners.\n");
                break;
            }
            printOwnersCircular();
            break;
        case 7:
            //Cleanup All Owners at Program End
              freeAllOwners();
            printf("Goodbye!\n");
            return;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}
int main()
{
    mainMenu();
    return 0;
}

void openPokedexMenu(){
    OwnerNode *newOwner = malloc(sizeof(OwnerNode));
    if (newOwner == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    printf("Your name: ");
    char* ownerName = NULL;
    ownerName = getDynamicInput();
    newOwner->ownerName = ownerName;
    newOwner->pokedexRoot = NULL;

    // Check if an owner with the same name already exists
    if (findOwnerByName(ownerName)) {
        printf("Owner '%s' already exists. Not creating a new Pokedex.\n", newOwner->ownerName);
        freeOwnerNode(newOwner);
        newOwner = NULL;

    } else {
        //clearBuffer();
        printf("Choose Starter:\n");
        printf("1. Bulbasaur\n"
                     "2. Charmander\n"
                     "3. Squirtle\n");
        int starterPokemonChoice = readIntSafe("Your choice: ");
        const PokemonData *starterPokemon = NULL;

        // Determine the chosen starter Pokemon
        switch (starterPokemonChoice) {
            case 1:
                // Assign Bulbasaur as the starter
                starterPokemon = &pokedex[BULBASAUR];

            break;
            case 2:
                // Assign Charmander as the starter
                starterPokemon = &pokedex[CHARMANDER];
            break;
            case 3:
                // Assign Squirtle as the starter
                starterPokemon = &pokedex[SQUIRTLE];
            break;
            default: {
                printf("Invalid choice.\n");
                clearBuffer();
                return;
            }
        }
        // Create a new owner with the selected starter Pokemon
        createOwner(newOwner, starterPokemon);
    }

}
// Function to create a new owner and initialize their Pokedex with a starter Pokemon
void createOwner(OwnerNode *newOwner, const PokemonData *starterPokemon) {
    // Initialize the owner's Pokedex with the starter Pokémon
    newOwner->pokedexRoot = createPokemonNode(starterPokemon);
    printf("New Pokedex created for %s with starter %s.\n",
           newOwner->ownerName, starterPokemon->name);

    // Add the new owner to the circular linked list
    linkOwnerInCircularList(newOwner);
}

// Link a new owner in the circular linked list
void linkOwnerInCircularList(OwnerNode *newOwner) {
    if (!ownerHead) {
        // First owner in the list
        ownerHead = newOwner;
        newOwner->next = newOwner;
        newOwner->prev = newOwner;
    }
    else {
        // Add to the end of the list
        ownerHead->prev->next = newOwner;
        newOwner->next = ownerHead;
        newOwner->prev = ownerHead->prev;
        ownerHead->prev = newOwner;
    }
}

// Function to create a new PokemonNode and initialize its values
PokemonNode *createPokemonNode(const PokemonData *data) {
    PokemonNode *pokemonNodeP = malloc(sizeof(PokemonNode));
    if (!pokemonNodeP) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    // Initialize the node with the provided Pokemon data
    pokemonNodeP->data = data;
    pokemonNodeP-> left = NULL;
    pokemonNodeP->right = NULL;

    return pokemonNodeP;
}
// Function to delete an owner's Pokedex
void deletePokedex() {
    // Get the total number of owners
    int ownerCount = countOwners();
    if (ownerCount == 0) {
        printf("No existing Pokedexes to delete.\n");
        return;
    }

    // Display the list of owners
    printf("\n=== Delete a Pokedex ===\n");
    OwnerNode* currentOwner = ownerHead;
    for (int i = 0; i < ownerCount; i++) {
        printf("%d. %s\n", i + 1, currentOwner->ownerName);
        currentOwner = currentOwner->next;
    }
    int choiceOfPokedex = readIntSafe("Choose a Pokedex to delete by number: ");

    OwnerNode* toDelete = NULL;
    if (choiceOfPokedex <= 0) {
        toDelete = getOwnerAtIndex(0);
    } else if (choiceOfPokedex >= ownerCount) {
        toDelete = getOwnerAtIndex(ownerCount);
    } else {
        toDelete = getOwnerAtIndex(choiceOfPokedex-1);
    }

    printf("Deleting %s's entire Pokedex...\n", toDelete->ownerName);

    // Free resources and remove owner
    freePokemonTree(toDelete->pokedexRoot);
    removeOwner(toDelete);

    printf("Pokedex deleted.\n");
}

// Function to remove an owner from the circular doubly linked list
void removeOwner(OwnerNode *toDelete) {
    if (toDelete == NULL || ownerHead == NULL) {
        return;
    }

    //Single Owner
    if (toDelete == ownerHead && toDelete->next == toDelete) {
        freeOwnerNode(toDelete);
        ownerHead = NULL;
        return;
    }

    // Update the previous and next pointers to bypass the node to delete
    if (toDelete->prev) {
        toDelete->prev->next = toDelete->next;
    }
    if (toDelete->next) {
        toDelete->next->prev = toDelete->prev;
    }

    // If the node to delete is the head, update the head pointer
    if (toDelete == ownerHead) {
        ownerHead = toDelete->next;
    }
    // Free the memory allocated for the owner node
    freeOwnerNode(toDelete);
}

// Get the owner at a specific index in the circular list
OwnerNode *getOwnerAtIndex(int index) {
    if (!ownerHead) return NULL;

    OwnerNode *current = ownerHead;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    return current;
}

// Function to free memory allocated for an owner node
void freeOwnerNode(OwnerNode *toDelete) {
    if (toDelete != NULL) {
        // Free the memory allocated for the owner's name
        free(toDelete->ownerName);
        toDelete->ownerName=NULL;
        free(toDelete);
    }
}

// Function to free all nodes in a binary search tree of Pokémon
void freePokemonTree(PokemonNode *root) {
    if (root == NULL) {
        return;
    }
    // Recursively free the left and right subtrees
    freePokemonTree(root->left);
    freePokemonTree(root->right);

    // Free the current node itself
    free (root);
}


// Function to merge two owners' Pokedexes and remove the second owner
void mergePokedexMenu(OwnerNode* ownerOne, OwnerNode* ownerTwo){
    printf("Merging %s and %s...\n", ownerOne->ownerName, ownerTwo->ownerName);

    // Add all Pokémon from ownerTwo's Pokedex to ownerOne's Pokedex
    addPokemonForMerge(ownerTwo->pokedexRoot, ownerOne);
    printf("Merge completed. \n");

    // Free the Pokedex tree of ownerTwo after merging
    freePokemonTree(ownerTwo->pokedexRoot);
    printf("Owner '%s' has been removed after merging.\n", ownerTwo->ownerName);

    // Remove ownerTwo from the circular doubly linked list
    removeOwner(ownerTwo);
}

// Recursive function to add Pokémon from one Pokedex to another
void addPokemonForMerge(PokemonNode *rootTwo, OwnerNode *ownerOne) {
    if (rootTwo == NULL) {
        return;
    }
    // Recursively process the left subtree
    addPokemonForMerge(rootTwo->left, ownerOne);

    // Check if the Pokémon from ownerTwo's Pokedex exists in ownerOne's Pokedex
    if (!isPokemon(ownerOne->pokedexRoot, ownerOne, rootTwo->data->id)) {
        // Add the Pokémon to ownerOne's Pokedex if it doesn't already exist
        ownerOne->pokedexRoot = addPokemon(ownerOne->pokedexRoot, ownerOne, rootTwo->data->id, 0);
    }

    // Recursively process the right subtree
    addPokemonForMerge(rootTwo->right, ownerOne);
    if (!isPokemon(ownerOne->pokedexRoot, ownerOne, rootTwo->data->id)) {
        ownerOne->pokedexRoot = addPokemon(ownerOne->pokedexRoot, ownerOne, rootTwo->data->id, 0);
    }
}

// Comparison function for an array of pointers to OwnerNode
int compareOwnersByName(const void *a, const void *b) {
    const OwnerNode *one = *(const OwnerNode**)a;
    const OwnerNode *two = *(const OwnerNode**)b;
    return strcmp(one->ownerName, two->ownerName);
}

// Function to update the circular doubly linked list based on the sorted array of owners
void updateCircularListAfterSort(OwnerNode **sortedOwners, int numOwners) {
    // Handle edge case: If there are no owners, set the head to NULL
    if (numOwners == 0) {
        ownerHead = NULL;
        return;
    }

    // Update the `next` and `prev` pointers for each owner in the sorted array
    for (int i = 0; i < numOwners; i++) {
        // Set the `next` pointer to the next owner in the array (wrap around using modulo)
        sortedOwners[i]->next = sortedOwners[(i + 1) % numOwners];
        // Set the `prev` pointer to the previous owner in the array (wrap around using modulo)
        sortedOwners[i]->prev = sortedOwners[(i - 1 + numOwners) % numOwners];
    }

    // Update the head of the circular linked list
    ownerHead = sortedOwners[0];
}

// Function to print owner names in a circular doubly linked list
void printOwnersCircular() {

    char input;
    printf("Enter direction (F or B): ");
    scanf(" %c", &input);
    clearBuffer();

    int printAmount = readIntSafe("How many prints? ");

    // Start from the head of the circular list
    OwnerNode* currentOwner = ownerHead;

    // Loop to print the specified number of owners
    for (int i = 0; i < printAmount; i++) {
        printf("[%d] %s\n", i+1, currentOwner->ownerName);
        if ( input == 'F' || input == 'f') {
            // Forward direction
            currentOwner = currentOwner->next;
        }
        else if ( input == 'B' || input == 'b') {
            // Backward direction
            currentOwner = currentOwner->prev;
        }
    }
}

void freeAllOwners() {
    if (ownerHead == NULL) {
        return;
    }
    OwnerNode* currentOwner = ownerHead;

    do {
        OwnerNode* nextOwner = currentOwner->next;

        //free the pokedex tree
        if (currentOwner->pokedexRoot != NULL) {
            freePokemonTree(currentOwner->pokedexRoot);
        }

        // Free the owner's name
        if (currentOwner->ownerName != NULL) {
            free(currentOwner->ownerName);
            currentOwner->ownerName = NULL;
        }
        //free the current owner node
        free(currentOwner);

        currentOwner = nextOwner;
    } while (currentOwner != ownerHead);

    ownerHead = NULL;
}

// --------------------------------------------------------------
// Indiviual Pokedex functions:
// 1. Add Pokemon
// 2. Display Pokemon
// 3. Delete Pokemon
// 4. Fight Pokemon
// 5. Evolve Pokemon
// --------------------------------------------------------------

// Function to add a Pokemon to the Pokedex (binary search tree)
PokemonNode* addPokemon(PokemonNode* root, OwnerNode* owner, int id, int* added) {
    // Reset the added flag to indicate no addition yet
    if (added) {
        *added = OFF;
    }
    // Validate ID
    if (id < 0 || id > POKEDEX_SIZE) {
        printf("Invalid ID.\n");
        return root;
    }

    // Base case: if the tree is empty, create a new node
    if (root == NULL) {
        root = createPokemonNode(&pokedex[id - 1]);
        if (added) {
            // Mark as added
            *added = ON;
        }
        return root;
    }

    // Check for duplicates
    if (id == root->data->id) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", id);
        return root;
    }

    // Traverse the tree based on the ID
    if (id < root->data->id) {
        // If the ID is smaller, recurse into the left subtree
        root->left = addPokemon(root->left, owner, id, added);
    } else {
        // If the ID is larger, recurse into the right subtree
        root->right = addPokemon(root->right, owner, id, added);
    }

    return root;
}

PokemonNode* freePokemonNode(PokemonNode* root, int choice) {
    if (root == NULL) {
        return root;
    }
    //go left
    if (choice < root->data->id) {
        root->left = freePokemonNode(root->left, choice);
    }
    //go right
    if (choice > root->data->id) {
        root->right = freePokemonNode(root->right, choice);
    }

    // found the node to delete woohoo
    if (choice == root->data->id) {
        printf("Removing Pokemon %s (ID %d).\n", root->data->name, root->data->id);

        //Option 1: No children
        if (root->left == NULL&& root->right == NULL) {
            free(root);
            root = NULL;
        }
        // Option 2: one child (right)
        else if (root->left == NULL) {
            PokemonNode *tmp = root;
            root = root->right;
            free(tmp);
        } //Option 2: one child (left)
        else if (root->right == NULL) {
            PokemonNode *tmp = root;
            root = root->left;
            free(tmp);
        }
        //Option 3: two children
        //here, I move the right tree up by making the spot for
        //deletion to be the right child, and then deleting the right child
        else {
            PokemonNode *min = findMinimum(root->right);
            root->data = min->data;
            //made it a problem of case 2
            root->right = freePokemonNode(root->right, min->data->id);
        }
    }
    return root;
}

// Function to simulate a fight between two Pokémon belonging to an owner
void pokemonFight(OwnerNode *owner, int IDOne, int IDTwo) {
    // Check if both Pokémon IDs exist in the owner's Pokedex
    if (isPokemon(owner->pokedexRoot, owner, IDOne)== 0 ||
        isPokemon(owner->pokedexRoot, owner, IDTwo)==0) {
        printf("One or both Pokemon IDs not found");
        return;
        }

    // Formula: Attack weighted at 1.5 + HP weighted at 1.2

    // Calculate the score for the first Pokémon
    double scoreOne = ((pokedex[IDOne-1].attack)*ATTACK_WEIGHT)+ (pokedex[IDOne-1].hp*HP_WEIGHT);
    // Calculate the score for the second Pokémon
    double scoreTwo = (pokedex[IDTwo-1].attack*ATTACK_WEIGHT)+ (pokedex[IDTwo-1].hp*HP_WEIGHT);

    printf("Pokemon 1: %s (Score = %.2f)\n",pokedex[IDOne-1].name, scoreOne);
    printf("Pokemon 2: %s (Score = %.2f)\n",pokedex[IDTwo-1].name, scoreTwo);

    // Determine the winner based on their scores
    if (scoreOne > scoreTwo) {
        // Pokémon 1 wins
        printf("%s wins!\n",pokedex[IDOne-1].name);
    }
    else if (scoreOne < scoreTwo) {
        // Pokémon 2 wins
        printf("%s wins!\n",pokedex[IDTwo-1].name);
    }
    else {
        // Both Pokémon have equal scores
        printf("It's a tie!\n");
    }
}

// Function to check if a Pokémon with a given ID exists in the Pokedex
int isPokemon(PokemonNode* root, OwnerNode* owner, int id) {
    // If the tree is empty or we've reached a leaf node, the Pokemon doesn't exist
    if (root == NULL) {

        return NOT_FOUND;
    }
    // Check if the current node matches the ID
    if (id == root->data->id) {
        // Pokémon found

        return FOUND;
    }
    // Traverse the tree
    if (id < root->data->id) {
        return isPokemon(root->left, owner, id);
    }
    if (id > root->data->id) {
        return isPokemon(root->right, owner, id);
    }
    // Fallback, although logically unreachable
    return NOT_FOUND;
}

//lets evolve these Pokemon!
void evolvePokemon(OwnerNode *owner, int idToEvolve) {
    //if the evolved Pokemon is already in the Pokedex, we release the unevolved form
    if (isPokemon(owner->pokedexRoot, owner, idToEvolve+1)){
        printf("Evolution ID %d (%s) already in the Pokedex. Releasing %s (ID %d).\n",
            idToEvolve+1, pokedex[idToEvolve].name, pokedex[idToEvolve-1].name, idToEvolve );
        owner->pokedexRoot = freePokemonNode(owner->pokedexRoot, idToEvolve);
    }
    //if the evolved Pokemon is not in the Pokedex, we add the evolved Pokemon to the Pokedex
    //we then release the unevolved form
    else {
        // Release the unevolved form
        owner->pokedexRoot = freePokemonNode(owner->pokedexRoot, idToEvolve);
        printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n",
            pokedex[idToEvolve-1].name, idToEvolve, pokedex[idToEvolve].name, idToEvolve+1);
        // Add the evolved Pokémon
        owner->pokedexRoot = addPokemon (owner->pokedexRoot, owner, idToEvolve+1, 0);
    }
}
//////////////////////////////////////////
///helper functions
//////////////////////////////////////////

void trimWhitespace(char *str)
{
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src)
{
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}
int countOwners() {
    if (ownerHead == NULL) {
        return 0;
    }
    int count = 0;
    OwnerNode *current = ownerHead;
     do{
        count++;
        current = current->next;
    }while (current != ownerHead);
    return count;
}



// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}


// Find an owner by name
int findOwnerByName(const char *name) {
    if (!ownerHead)
        return 0;

    OwnerNode *current = ownerHead;
    do {
        if (strcmp(current->ownerName, name) == 0) {
            return 1;
        }
        current = current->next;
    } while (current != ownerHead);

    return 0;
}

char* duplicateString(const char *src) {
    if (!src) return NULL;
    char *copy = malloc(strlen(src) + 1);
    if (!copy) return NULL;
    strcpy(copy, src);
    return copy;
}

void clearBuffer() {
    scanf("%*[^\n]");
    scanf("%*c");
}
