// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* struct for movie information */
struct movie
{
    char *title;
    int year;
    char languages[5][21];
    double rating;
    struct movie *next;
};

/* Parse the current line which is space delimited and create a
*  movie struct with the data in this line
*/
struct movie *createMovie(char *currLine)
{
    struct movie *currMovie = malloc(sizeof(struct movie));

    // For use with strtok_r
    char *saveptr;

    // The first token is the title
    char *token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // The next token is the year
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = atoi(token);

    // The next token(s) is the languages

    //Tokenize all the languages into one string
    char *langPtr;
    char *langToken = strtok_r(NULL, "[]", &saveptr);
    char *langLine = langToken;
    langToken = strtok_r(langLine, ";", &langPtr);

    int index = 0;

    //Tokenize language string into movie struct
    while(langToken != NULL) {
        strcpy(currMovie->languages[index], langToken);
        langToken = strtok_r(NULL, ";", &langPtr);
        index++;
    };

    // The last token is the rating
    token = strtok_r(NULL, ",\n", &saveptr);
    currMovie->rating = strtod(token, NULL);

    // Set the next node to NULL in the newly created movie entry
    currMovie->next = NULL;

    return currMovie;
}

/*
* Return a linked list of movies by parsing data from
* each line of the specified file.
*/
struct movie *processFile(char *filePath)
{
    // Open the specified file for reading only
    FILE *movieFile = fopen(filePath, "r");

    char *currLine = NULL;
    char *stringBuffer = NULL;
    size_t len = 0;
    ssize_t nread;
    char *token;

    // The head of the linked list
    struct movie *head = NULL;
    // The tail of the linked list
    struct movie *tail = NULL;

    //remove first line from file
    getline(&stringBuffer, &len, movieFile);
    free(stringBuffer);

    int numOfMovies = 0;

    // Read the file line by line
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
    {
        // Get a new movie node corresponding to the current line
        struct movie *newNode = createMovie(currLine);

        // Is this the first node in the linked list?
        if (head == NULL)
        {
            // This is the first node in the linked link
            // Set the head and the tail to this node
            head = newNode;
            tail = newNode;
        }
        else
        {
            // This is not the first node.
            // Add this node to the list and advance the tail
            tail->next = newNode;
            tail = newNode;
        }
        numOfMovies++;
    }

    printf("processed %s and parsed data for %d movies\n\n", filePath, numOfMovies);
    free(currLine);
    fclose(movieFile);
    return head;
}

// Prompt the user for what kind of information they want to see
int initialPrompt() {
    int answer;

    printf("\n\n1. Select a file to process\n");
        
    printf("\n2. Exit from the program\n");

    printf("\nEnter a choice 1 or 2: ");
    scanf("%i", &answer);

    return answer;
}


//Picks an action for the program to perform depending on user choice.
void promptToProcess() {

    int option;
    do{
        printf("\n\nWhich file do you want to process?\n Enter 1 to pick the largest file\n Enter 2 to pick the smallest file\n Enter 3 to specfiy the name of a file\n");

        printf("\nEnter a choice from 1 to 3: ");
        scanf("%i", &option);

        switch(option) {
            case 1:
                printf("\n\nProcess largest file");
                break;
            case 2:
                printf("\n\nProcess smallest file");
                break;
            case 3:
                printf("\n\nProcess user specified file");
                break;

            default:
                printf("ERROR: Please choose an integer within the range of [1, 3]\n\n");
        }

    }while(option < 1 || option > 3);

}

//Free all memory allocated for the movie linked list
void freeList (struct movie *head) {
    struct movie *temp;

    while(head != NULL) {
        temp = head;
        head = head->next;
        free(temp->title);
        free(temp);
    }
}

/*
*   Process the file provided as an argument to the program to
*   create a linked list of movie structs and print out the list.
*   Compile the program as follows:
*       gcc --std=gnu99 -o movies main.c
*/

int main()
{
    int choice;
    do{
        choice = initialPrompt();
        if(choice == 1)
            promptToProcess();
        else
            printf("ERROR: Please choose an integer within the range of [1, 2]\n\n");

    }while(choice != 2);

    return EXIT_SUCCESS;
}



    // if (argc < 2)
    // {
    //     printf("You must provide the name of the file to process\n");
    //     printf("Example usage: ./movies movies_sample_1.csv\n");
    //     return EXIT_FAILURE;
    // }
    // struct movie *list = processFile(argv[1]);
    //freeList(list);