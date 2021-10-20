// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

#define PREFIX "movies"
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

char * getLargestFile() {
    DIR* currDir = opendir(".");
    struct dirent *aDir;
    struct stat dirStat;
    int fileSize = 0;
    char *largestFile;
    int i = 0;
    char *saveptr, *ptr1, *ptr2, *tempStr;

    // Go through all the entries
    while((aDir = readdir(currDir)) != NULL){

        if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0){
            tempStr = calloc(strlen(aDir->d_name) +1, sizeof(char));
            strcpy(tempStr, aDir->d_name);
            ptr1 = strtok_r(tempStr, ".", &saveptr);
            ptr2 = strtok_r(NULL, ".", &saveptr);

            // Get meta-data for the current entry
            stat(aDir->d_name, &dirStat); 

            //Only grab files that end with extension "csv"
            if(ptr2 != NULL && strcmp(ptr2, "csv") == 0){ 
                if(dirStat.st_size > fileSize) {
                largestFile = calloc(strlen(aDir->d_name) + 1, sizeof(char));
                strcpy(largestFile, aDir->d_name);
                fileSize = dirStat.st_size;
                }
            } 
        }
    }

    // Close the directory
    closedir(currDir);
    return largestFile;
}

char * createDir() {
    char *nameOfDir;
    int randNum = random() % 100000;
    int len = sprintf(NULL, "gonzedua_movies_%i" ,randNum);
    nameOfDir = calloc(len + 1, sizeof(char));
    sprintf(nameOfDir, "gonzedua_movies_%i" ,randNum);

    int check = mkdir(nameOfDir, 0750);
    if(check == -1)
        printf("\nError in creating directory.");
    
    return nameOfDir;
}

void createNewFiles(char *dir, struct movie *list) {
    char * newFilePath;
    while(list != NULL) {
        printf("HELLO");
        // int len = sprintf(NULL, "%s/%d", dir, list->year);
        // newFilePath = calloc(len + 1, sizeof(char));
        // sprintf(newFilePath, "%s/%d", dir, list->year );
        // printf("%s", newFilePath);
        list = list->next;
    }
}

//Picks an action for the program to perform depending on user choice.
void promptToProcess() {
    char *fileToProcess;
    int option;
    do{
        printf("\n\nWhich file do you want to process?\n Enter 1 to pick the largest file\n Enter 2 to pick the smallest file\n Enter 3 to specfiy the name of a file\n");

        printf("\nEnter a choice from 1 to 3: ");
        scanf("%i", &option);

        switch(option) {
            case 1:
                fileToProcess = getLargestFile();
                printf("The largest file/directory in the current directory is %s\n", fileToProcess);
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

    struct movie *list = processFile(fileToProcess);
    printf("HELLO");
    char *newDir = createDir();
    printf("HELLO");
    createNewFiles(newDir, list);
    freeList(list);
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

/*
*   Process the file provided as an argument to the program to
*   create a linked list of movie structs and print out the list.
*   Compile the program as follows:
*       gcc --std=gnu99 -o movies main.c
*/

int main()
{
    srandom(time(NULL));
    int choice;
    do{
        choice = initialPrompt();
        if(choice == 1)
            promptToProcess();
        else if (choice == 2)
            printf("Goodbye!");
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