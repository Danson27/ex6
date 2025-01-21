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
// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

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

        switch (starterPokemonChoice) {
            case 1:
                // Bulbasaur
                starterPokemon = &pokedex[0];
                break;
            case 2:
                // Charmander
                starterPokemon = &pokedex[3];
                break;
            case 3:
                // Squirtle
                starterPokemon = &pokedex[6];
                break;
            default: {
                printf("Invalid choice.\n");
                clearBuffer();
                return;
            }
        }

        createOwner(newOwner, starterPokemon);
    }

}

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

// --------------------------------------------------------------
// Display Menu
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

void updateCircularListAfterSort(OwnerNode **sortedOwners, int numOwners) {
    if (numOwners == 0) {
        ownerHead = NULL;
        return;
    }

    for (int i = 0; i < numOwners; i++) {
        sortedOwners[i]->next = sortedOwners[(i + 1) % numOwners];
        sortedOwners[i]->prev = sortedOwners[(i - 1 + numOwners) % numOwners];
    }

    // Update the head of the circular linked list
    ownerHead = sortedOwners[0];
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
void printOwnersCircular() {

    char input;
    printf("Enter direction (F or B): ");
    scanf(" %c", &input);
    clearBuffer();

    int printAmount = readIntSafe("How many prints? ");
    OwnerNode* currentOwner = ownerHead;

    for (int i = 0; i < printAmount; i++) {
            printf("[%d] %s\n", i+1, currentOwner->ownerName);
        if ( input == 'F' || input == 'f') {
            currentOwner = currentOwner->next;
        }
        else if ( input == 'B' || input == 'b') {
            currentOwner = currentOwner->prev;
        }
    }
}

// Comparison function for an array of pointers to OwnerNode
int compareOwnersByName(const void *a, const void *b) {
    const OwnerNode *one = *(const OwnerNode**)a;
    const OwnerNode *two = *(const OwnerNode**)b;
    return strcmp(one->ownerName, two->ownerName);
}


void mergePokedexMenu(OwnerNode* ownerOne, OwnerNode* ownerTwo){
    printf("Merging %s and %s...\n", ownerOne->ownerName, ownerTwo->ownerName);
    addPokemonForMerge(ownerTwo->pokedexRoot, ownerOne);
    printf("Merge completed. \n");

    freePokemonTree(ownerTwo->pokedexRoot);
    printf("Owner '%s' has been removed after merging.\n", ownerTwo->ownerName);
    removeOwner(ownerTwo);
}

void addPokemonForMerge(PokemonNode *rootTwo, OwnerNode *ownerOne) {
    if (rootTwo == NULL) {
        return;
    }
    addPokemonForMerge(rootTwo->left, ownerOne);
    if (!isPokemon(ownerOne->pokedexRoot, ownerOne, rootTwo->data->id)) {
        ownerOne->pokedexRoot = addPokemon(ownerOne->pokedexRoot, ownerOne, rootTwo->data->id, 0);
    }
    addPokemonForMerge(rootTwo->right, ownerOne);
    if (!isPokemon(ownerOne->pokedexRoot, ownerOne, rootTwo->data->id)) {
        ownerOne->pokedexRoot = addPokemon(ownerOne->pokedexRoot, ownerOne, rootTwo->data->id, 0);
    }
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

void deletePokedex() {
    int ownerCount = countOwners();
    if (ownerCount == 0) {
        printf("No existing Pokedexes to delete.\n");
        return;
    }
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
void removeOwner(OwnerNode *toDelete) {
    if (toDelete == NULL || ownerHead == NULL) {
        return;
    }

    if (toDelete == ownerHead && toDelete->next == toDelete) {
        //Single Owner
        freeOwnerNode(toDelete);
        ownerHead = NULL;
        return;
    }

    if (toDelete->prev) {
        toDelete->prev->next = toDelete->next;
    }
    if (toDelete->next) {
        toDelete->next->prev = toDelete->prev;
    }
    if (toDelete == ownerHead) {
        ownerHead = toDelete->next;
    }

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


void freeOwnerNode(OwnerNode *toDelete) {
    if (toDelete != NULL) {
        free(toDelete->ownerName);
        toDelete->ownerName=NULL;
        free(toDelete);
    }
}

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




PokemonNode *createPokemonNode(const PokemonData *data) {
    PokemonNode *pokemonNodeP = malloc(sizeof(PokemonNode));
    if (!pokemonNodeP) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    pokemonNodeP->data = data;
    pokemonNodeP-> left = NULL;
    pokemonNodeP->right = NULL;

    return pokemonNodeP;
}
PokemonNode* addPokemon(PokemonNode* root, OwnerNode* owner, int id, int* added) {
    if (added) {
        *added = 0;
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
            *added = 1; // Mark as added
        }
        return root;
    }
    // Check for duplicates
    if (id == root->data->id) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", id);
        return root;
    }
    // Traverse the tree
    if (id < root->data->id) {
        root->left = addPokemon(root->left, owner, id, added);
    } else {
        root->right = addPokemon(root->right, owner, id, added);
    }

    return root;
}

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

    Queue *queue = createQueue(); // Create a queue
    enqueue(queue, root);         // Enqueue the root node

    while (!isQueueEmpty(queue)) {
        PokemonNode *current = dequeue(queue); // Dequeue the front node
        visit(current);                        // Visit the current node

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

PokemonNode* findMinimum(PokemonNode* root) {
    while (root && root->left)
        root = root->left;
    return root;
}


void pokemonFight(OwnerNode *owner, int IDOne, int IDTwo) {
    if (isPokemon(owner->pokedexRoot, owner, IDOne)== 0 ||
        isPokemon(owner->pokedexRoot, owner, IDTwo)==0) {
        printf("One or both Pokemon IDs not found");
        return;
    }
    double scoreOne = ((pokedex[IDOne-1].attack)*1.5)+ (pokedex[IDOne-1].hp*1.2);
    double scoreTwo = (pokedex[IDTwo-1].attack*1.5)+ (pokedex[IDTwo-1].hp*1.2);

    printf("Pokemon 1: %s (Score = %.2f)\n",pokedex[IDOne-1].name, scoreOne);
    printf("Pokemon 2: %s (Score = %.2f)\n",pokedex[IDTwo-1].name, scoreTwo);

    if (scoreOne > scoreTwo) {
        printf("%s wins!\n",pokedex[IDOne-1].name);
    }
    else if (scoreOne < scoreTwo) {
        printf("%s wins!\n",pokedex[IDTwo-1].name);
    }
    else {
        printf("It's a tie!\n");
    }
}


int isPokemon(PokemonNode* root, OwnerNode* owner, int id) {
    // If the tree is empty or we've reached a leaf node, the Pokemon doesn't exist
    if (root == NULL) {
        return 0;
    }
    // Check if the current node matches the ID
    if (id == root->data->id) {
        return 1;
    }
    // Traverse the tree
    if (id < root->data->id) {
        return isPokemon(root->left, owner, id);
    }
    if (id > root->data->id) {
        return isPokemon(root->right, owner, id);
    }
    // Fallback, although logically unreachable
    return 0;
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
            int wasAdded = 0;
            currentOwner->pokedexRoot = addPokemon(currentOwner->pokedexRoot, currentOwner, id, &wasAdded);
            if (wasAdded)
                printf("Pokemon %s (ID %d) added. \n ", pokedex[id-1].name, id);
            break;
            case 2:
            displayPokedexOptions(currentOwner);
            break;
        case 3:
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
            if (currentOwner->pokedexRoot == NULL) {
                printf("Pokedex is empty.\n");
                break;
            }
            int IDOne = readIntSafe("Enter ID of the first Pokemon: ");
            int IDTwo = readIntSafe("Enter ID of the second Pokemon: ");
            pokemonFight(currentOwner, IDOne, IDTwo);
            break;
        case 5:
            if (currentOwner->pokedexRoot == NULL) {
                printf("Cannot evolve. Pokedex empty.\n");
                break;
            }
            int idToEvolve = readIntSafe("Enter ID to evolve: ");
            if (!isPokemon(currentOwner->pokedexRoot, currentOwner, idToEvolve)) {
                printf("No Pokemon with ID %d found.\n", idToEvolve);
                break;
            }
            if (!pokedex[idToEvolve-1].CAN_EVOLVE) {
                printf("%s (ID %d) cannot evolve.\n",pokedex[idToEvolve-1].name, idToEvolve);
                break;
            }
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

            int ownerCount = countOwners();
            printf("\nExisting Pokedexes:\n");
            OwnerNode* currentOwner = ownerHead;

            for (int i = 0; i < ownerCount; i++) {
                printf("%d. %s\n", i+1, currentOwner->ownerName);
                currentOwner = currentOwner->next;
            }
            OwnerNode* selectedOwner = NULL;
            int choiceOfPokedex = readIntSafe("Choose a Pokedex by number: ");

            if (choiceOfPokedex <= 0) {
                selectedOwner = getOwnerAtIndex(0);
            }
            else if (choiceOfPokedex >= ownerCount) {
                selectedOwner = getOwnerAtIndex(ownerCount);
            }
            else {
                selectedOwner = getOwnerAtIndex(choiceOfPokedex-1);
            }

            printf("\nEntering %s's Pokedex...\n", selectedOwner->ownerName);
            enterPokedexMenu(selectedOwner);

            break;
        case 3:
            deletePokedex();
            break;
        case 4:
            OwnerNode *ownerOne = NULL;
            OwnerNode *ownerTwo = NULL;
            int owners = countOwners();
            if (owners < 2) {
                printf("Not enough owners to merge.\n");
                break;
            }
            printf("\n=== Merge Pokedexes ===\n");
            printf("Enter name of first owner: ");
            char* ownerNameOne = NULL;
            ownerNameOne = getDynamicInput();
            if (ownerNameOne == NULL) {
                exit(1);
            }
            printf("Enter name of second owner: ");
            char* ownerNameTwo = NULL;
            ownerNameTwo = getDynamicInput();
            if (ownerNameTwo == NULL) {
                exit(1);
            }
            int i = 0;
            OwnerNode* current = ownerHead;

            if (current != NULL) {
                do {
                    if (strcmp(ownerNameOne, current->ownerName) == 0) {
                        ownerOne = current;
                    }
                    if (strcmp(ownerNameTwo, current->ownerName) == 0) {
                        ownerTwo = current;
                    }
                    current = current->next;
                    i++;
                } while (current != ownerHead);
            }
            mergePokedexMenu(ownerOne, ownerTwo);
            free(ownerNameOne);
            free(ownerNameTwo);
            break;
        case 5:
            int numOwners = countOwners();
            if (numOwners < 2) {
                printf("0 or 1 owners only => no need to sort.\n");
                break;
            }
            OwnerNode **ownerArray = malloc(sizeof(OwnerNode*) * numOwners);
            if (ownerArray == NULL) {
                exit(1);
            }
            OwnerNode *theCurrent = ownerHead;
            for (int i = 0; i < numOwners; i++) {
                ownerArray[i] = theCurrent;
                theCurrent = theCurrent->next;
            }
            qsort(ownerArray, numOwners, sizeof(OwnerNode*), compareOwnersByName);
            updateCircularListAfterSort(ownerArray, numOwners);

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
    setbuf(stdout,0);
    mainMenu();
    // freeAllOwners();
    return 0;
}
