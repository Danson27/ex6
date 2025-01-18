#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128
# define MAX_OWNER_NAME_LEN 20

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
void openPokedexMenu(OwnerNode ***allOwners, int *currentAmountOfOwners){
    OwnerNode **temp = realloc (*allOwners, sizeof(OwnerNode) * (*currentAmountOfOwners+1));
    if (temp == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    *allOwners = temp;

    OwnerNode *newOwner = malloc(sizeof(OwnerNode));
    if (newOwner == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    printf("Your name: ");
    char ownerName[21];
    scanf("%20s", ownerName);
    newOwner->ownerName = malloc(strlen(ownerName) + 1);
    if (newOwner->ownerName == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    if (*currentAmountOfOwners >= 1) {
        for (int i = 0; i < *currentAmountOfOwners; i++) {
            if (strcmp((*allOwners)[i]->ownerName, ownerName) == 0 ) {
                printf("Owner '%s' already exists. Not creating a new Pokedex.\n", (*allOwners)[i]->ownerName);
                clearBuffer();
                return;
            }
        }
    }
    strcpy(newOwner->ownerName, ownerName);
    newOwner->pokedexRoot = NULL;
    linkOwnerInCircularList(allOwners, newOwner, currentAmountOfOwners);

    int starterPokemonChoice;
    printf("Choose starter\n");
    printf("1. Bulbasaur\n"
                 "2. Charmander\n"
                 "3. Squirtle\n");
    printf("Your choice: ");
    scanf("%d", &starterPokemonChoice);

    switch (starterPokemonChoice) {
        case 1: {
            PokemonNode *pokemonNodeP = createPokemonNode(&pokedex[0]);
            newOwner->pokedexRoot = pokemonNodeP;
            printf("New Pokedex created for %s with starter %s.", newOwner->ownerName, pokedex[0].name);
            break;
        }
        case 2: {
            PokemonNode *pokemonNodeP = createPokemonNode(&pokedex[3]);
            newOwner->pokedexRoot = pokemonNodeP;
            printf("New Pokedex created for %s with starter %s.", newOwner->ownerName, pokedex[3].name);
            break;
        }
        case 3: {
            PokemonNode *pokemonNodeP = createPokemonNode(&pokedex[6]);
            newOwner->pokedexRoot = pokemonNodeP;
            printf("New Pokedex created for %s with starter %s.", newOwner->ownerName, pokedex[6].name);
            break;
        }
        default: {
            printf("Invalid choice.\n");
            clearBuffer();
            return;
        }
    }
    clearBuffer();
}


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
        printPokemonNode(owner->pokedexRoot);
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
        initNodeArray(na, cap);
        collectAll(owner->pokedexRoot, na);
        displayAlphabetical(na);
        free(na->nodes);
        break;
    default:
        printf("Invalid choice.\n");
    }
}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu(OwnerNode*** allOwners, const int *currentAmountOfOwners)
{
    int choiceOfPokedex;
    OwnerNode* currentOwner = NULL;
    printf("\nExisting Pokedexes:\n");
    for (int i = 0; i <= *currentAmountOfOwners-1 ; i++) {
        printf("%d. %s\n", i+1, (*allOwners)[i]->ownerName);
    }
    printf("Choose a Pokedex by number: ");
    scanf("%d", &choiceOfPokedex);

    if (choiceOfPokedex <= 0) {
        currentOwner = (*allOwners)[0];
    }
    else if (choiceOfPokedex >= *currentAmountOfOwners) {
        currentOwner = (*allOwners)[*currentAmountOfOwners - 1];
    }
    else {
        currentOwner = (*allOwners)[choiceOfPokedex-1];
    }
    clearBuffer();
  printf("\nEntering %s's Pokedex...\n", currentOwner->ownerName);

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
            int choice = readIntSafe("Enter Pokemon ID to release:");
            currentOwner->pokedexRoot = freePokemon(currentOwner->pokedexRoot, choice);
            break;
        case 4:
            if (currentOwner->pokedexRoot == NULL) {
                printf("Pokedex is empty.\n");
                return;
            }
            int IDOne = readIntSafe("Enter ID of the first Pokemon: ");
            int IDTwo = readIntSafe("Enter ID of the second Pokemon: ");
            pokemonFight(currentOwner, IDOne, IDTwo);
            break;
        case 5:
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
            break;
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
    int amountOfOwners = 0;
    OwnerNode **allOwners = malloc(sizeof(OwnerNode));
    if (!allOwners) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    ownerHead = *allOwners;
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
           openPokedexMenu(&allOwners, &amountOfOwners);
            break;
        case 2:
           enterExistingPokedexMenu(&allOwners, &amountOfOwners);
            break;
        case 3:
          //  deletePokedex();
            break;
        case 4:
          //  mergePokedexMenu();
            break;
        case 5:
          //  sortOwners();
            break;
        case 6:
           // printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main()
{
    mainMenu();
   // freeAllOwners();
    return 0;
}
// Function to read a dynamically allocated string input from the user
char* readInput() {
    char buffer[MAX_OWNER_NAME_LEN];
    if (fgets(buffer, MAX_OWNER_NAME_LEN, stdin) == NULL) {
        printf("Error reading input.\n");
        exit(1);
    }
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
    char *input = malloc(strlen(buffer) + 1);
    if (!input) {
        printf("Memory allocation error\n");
        exit(1);
    }
    strcpy(input, buffer);
    return input;
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

void linkOwnerInCircularList(OwnerNode ***allOwners, OwnerNode *newOwner, int *currentAmountOfOwners) {
    OwnerNode **temp = realloc(*allOwners, (*currentAmountOfOwners + 1) * sizeof(OwnerNode *));
    if (temp == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    *allOwners = temp;

    if (*currentAmountOfOwners == 0) {
        newOwner->next = newOwner;
        newOwner->prev = newOwner;
    }
    else {
        (*allOwners)[*currentAmountOfOwners - 1]->next = newOwner;
        (newOwner)->prev = (*allOwners)[*currentAmountOfOwners - 1];
        newOwner->next = (*allOwners)[0];
        (*allOwners)[0]->prev = newOwner;

    }
    (*allOwners)[*currentAmountOfOwners] = newOwner;
    (*currentAmountOfOwners)++;

    PokemonNode *newPokemon = malloc(sizeof(PokemonNode));
    if (newPokemon == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }

    newOwner->pokedexRoot = newPokemon;
    newPokemon->left = NULL;
    newPokemon->right = NULL;
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
    //printf("Pokemon %s (ID %d) added. \n ", pokedex[id-1].name, id);

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
void BFSGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        return;
    }
    if (root->left != NULL) {
        visit(root->left);
    }
    if (root->right != NULL) {
        visit(root->right);
    }
    BFSGeneric(root->left, visit);
    BFSGeneric(root->right, visit);
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

int compareByNameNode(const void *a, const void *b) {
    PokemonNode *nodeA = *(PokemonNode **)a;
    PokemonNode *nodeB = *(PokemonNode **)b;

    // Compare names alphabetically
    return strcmp(nodeA->data->name, nodeB->data->name);
}
void displayAlphabetical(NodeArray *na){
    qsort(na->nodes, na->size, sizeof(PokemonNode*), compareByNameNode);
    for (int i = 0; i < na->size; i++) {
        printPokemonNode(na->nodes[i]);
    }
}

PokemonNode* freePokemon(PokemonNode* root, int choice) {
    if (root == NULL) {
        return root;
    }
    //go left
    if (choice < root->data->id) {
        root->left = freePokemon(root->left, choice);
    }
    //go right
    if (choice > root->data->id) {
        root->right = freePokemon(root->right, choice);
    }

    // found the node to delete
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
            root->right = freePokemon(root->right, min->data->id);

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
        owner->pokedexRoot = freePokemon(owner->pokedexRoot, idToEvolve);
    }
    //if the evolved Pokemon is not in the Pokedex, we add the evolved Pokemon to the Pokedex
    //we then release the unevolved form
    else {
        // Release the unevolved form
        owner->pokedexRoot = freePokemon(owner->pokedexRoot, idToEvolve);
        printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n",
            pokedex[idToEvolve-1].name, idToEvolve, pokedex[idToEvolve].name, idToEvolve+1);
        // Add the evolved Pokémon
        owner->pokedexRoot = addPokemon (owner->pokedexRoot, owner, idToEvolve+1, 0);
    }
}
