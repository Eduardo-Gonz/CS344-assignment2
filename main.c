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

//Shows movies released in a specific year
void showMovieByYear(struct movie *list) {

    //Ask the user for the year
    int year;
    printf("\nEnter the year for which you want to see movies: ");
    scanf("%i", &year);

    int foundYear = 0;

    while(list != NULL) {

        if(list->year == year) {
            printf("\n%s", list->title);
            foundYear++;
        }

        list = list->next;
    }

    if(foundYear == 0)
        printf("\nno data about movies released in %i\n", year);

}

//Print the highest rated movie for each year present in the file.
void printHighestRated(struct movie *arr[121]){
    for(int i = 0; i < 121; i++) {
        if(arr[i] != NULL){
            printf("\n%d, %.1f, %s", arr[i]->year, 
                                   arr[i]->rating,
                                   arr[i]->title);
        }
    }
}

//Find the highest rated movie for each year present in the file and store information of that movie in an array.
void findHighestRated(struct movie *list) {
    struct movie *movieArr[121] = {NULL};

    while(list != NULL) {

        int currYear = list->year - 1900;
        if(movieArr[currYear] == NULL)
            movieArr[currYear] = list;
        else{
            if(movieArr[currYear]->rating < list->rating)
                movieArr[currYear] = list;
        }
        list = list->next;
    }

    printHighestRated(movieArr);
}

//Checks if a movie has been released in a user-specified language
int matchLang(char movieLang[5][21], char userLang[21]) {
    for(int i = 0; i < 5; i++) {
        if(strcmp(movieLang[i], userLang) == 0 )
            return 1;
    }
    return 0;
}

//Find and print movies that are released in a user-specified language
void findMovieByLang(struct movie *list) {
    char userLang[21];
    printf("\nEnter the language for which you want to see movies: ");
    scanf("%s", userLang);

    int foundMatch = 0;
    while(list != NULL) {
        if(matchLang(list->languages, userLang) == 1){
            printf("\n%i, %s", list->year, list->title);
            foundMatch++;
        }
        list = list->next;
    }
    if(foundMatch == 0)
        printf("\nno data about movies released in %s", userLang);
}

// Prompt the user for what kind of information they want to see
int promptUser() {
    int answer;

    printf("\n\n1. Show movies released in the specified year\n2. Show highest rated movie for each year\n3. Show the title and year of release of all movies in a specified language\n4. Exit from the program\n");
        
    printf("\nEnter a choice from 1 to 4: ");
    scanf("%i", &answer);

    return answer;
}

//Picks an action for the program to perform depending on user choice.
void doChoice(int choice, struct movie *list) {
    switch(choice) {
        case 1:
            showMovieByYear(list);
            break;
        case 2:
            findHighestRated(list);
            break;
        case 3:
            findMovieByLang(list);
            break;

        default:
            printf("See you next time!");
    }

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

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movies_sample_1.csv\n");
        return EXIT_FAILURE;
    }
    struct movie *list = processFile(argv[1]);

    int choice;
    do{
        choice = promptUser();
        if(0 < choice && choice < 5)
            doChoice(choice, list);
        else
            printf("ERROR: Please choose an integer within the range of [1, 4]\n\n");

    }while(choice != 4);

    freeList(list);
    return EXIT_SUCCESS;
}