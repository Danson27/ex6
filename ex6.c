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
void displayMenu(OwnerNode *owner)
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
        //displayBFS(owner->pokedexRoot);
        break;
    case 2:
       // preOrderTraversal(owner->pokedexRoot);
        break;
    case 3:
       // inOrderTraversal(owner->pokedexRoot);
        break;
    case 4:
       // postOrderTraversal(owner->pokedexRoot);
        break;
    case 5:
       // displayAlphabetical(owner->pokedexRoot);
        break;
    default:
        printf("Invalid choice.\n");
    }
}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    // list owners
    printf("\nExisting Pokedexes:\n");
    // you need to implement a few things here :)

  //  printf("\nEntering %s's Pokedex...\n", cur->ownerName);

    int subChoice;
    do
    {
   //     printf("\n-- %s's Pokedex Menu --\n", cur->ownerName);
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
            //addPokemon(cur);
            break;
        case 2:
           // displayMenu(cur);
            break;
        case 3:
            //freePokemon(cur);
            break;
        case 4:
          //  pokemonFight(cur);
            break;
        case 5:
           // evolvePokemon(cur);
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
           // enterExistingPokedexMenu();
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


