// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

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

    printf("Now processing the chosen file %s\n\n", filePath);
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


int filterFile(char *fileName) {
    char *saveptr, *ptr, *tempStr;
    int bool = 0;
    if(strncmp(PREFIX, fileName, strlen(PREFIX)) == 0){
        tempStr = calloc(strlen(fileName) +1, sizeof(char));
        strcpy(tempStr, fileName);
        ptr = strtok_r(tempStr, ".", &saveptr);
        ptr = strtok_r(NULL, ".", &saveptr);

        //if file meets requirements return true
        if(ptr!= NULL && strcmp(ptr, "csv") == 0) 
            bool = 1;

	free(tempStr);
    }

    return bool;
}

char * copyFile(char *prefFile, char *dName) {
    if(prefFile != NULL)
        free(prefFile);
    prefFile = calloc(strlen(dName) + 1, sizeof(char));
    strcpy(prefFile, dName);

    return prefFile;
}




char * getLargestFile(char fileChoice) {
    DIR* currDir = opendir(".");
    struct dirent *aDir;
    struct stat dirStat;
    int fileSize = 0;
    char *prefFile = NULL;

    // Go through all the entries
    while((aDir = readdir(currDir)) != NULL){

        if(filterFile(aDir->d_name) == 0)
            continue;
            
        //get meta-data of file
        stat(aDir->d_name, &dirStat);

        if(fileChoice == 'L' && dirStat.st_size > fileSize){
            prefFile =  copyFile(prefFile, aDir->d_name);
            fileSize = dirStat.st_size;
        }
        else if(fileChoice == 'S' && dirStat.st_size < fileSize) {
            prefFile =  copyFile(prefFile, aDir->d_name);
            fileSize = dirStat.st_size;
        }
    }
printf("\n%s\n", prefFile);
    // Close the directory
    closedir(currDir);
    return prefFile;
}

char * createDir() {
    char *nameOfDir;
    int randNum = random() % 100000;
    int len = strlen("gonzedua.movies.") + 5;
    nameOfDir = calloc(len + 1, sizeof(char));
    sprintf(nameOfDir, "gonzedua_movies_%i" ,randNum);

    int check = mkdir(nameOfDir, 0750);
    if(check == -1)
        printf("\nError in creating directory.");
    
    return nameOfDir;
}

void populateFile(char *file, char *title) {
	int fd;

    fd = open(file, O_RDWR | O_CREAT | O_APPEND, 0640);
	if (fd == -1)
		printf("open() failed on \"%s\"\n", file);

    write(fd, title, strlen(title));
    write(fd, "\r\n", 2);
    close(fd);
}

void createNewFiles(char *dir, struct movie *list) {
    char * newFilePath;
    while(list != NULL) {
        int len = strlen(dir) + 9;
        newFilePath = calloc(len + 1, sizeof(char));
        sprintf(newFilePath, "%s/%d.txt", dir, list->year );
        populateFile(newFilePath, list->title);
        free(newFilePath);
       
       	list = list->next;
    }

}

//Picks an action for the program to perform depending on user choice.
void promptToProcess() {
    char *fileToProcess;
    char fileOption;
    int option;
    do{
        printf("\n\nWhich file do you want to process?\n Enter 1 to pick the largest file\n Enter 2 to pick the smallest file\n Enter 3 to specfiy the name of a file\n");

        printf("\nEnter a choice from 1 to 3: ");
        scanf("%i", &option);

        switch(option) {
            case 1:
                fileOption = 'L';
                fileToProcess = getLargestFile(fileOption);
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
    char *newDir = createDir();
    createNewFiles(newDir, list);

    //free any remaining memory that was allocated
    freeList(list);
    free(fileToProcess);
    free(newDir);
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
*       gcc --std=gnu99 -o movies_by_year main.c
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


// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <dirent.h>
// #include <sys/stat.h>
// #include <time.h>
// #include <unistd.h>
// #include <fcntl.h>

// #define PREFIX "movies"
// /* struct for movie information */
// struct movie
// {
//     char *title;
//     int year;
//     char languages[5][21];
//     double rating;
//     struct movie *next;
// };

// /* Parse the current line which is space delimited and create a
// *  movie struct with the data in this line
// */
// struct movie *createMovie(char *currLine)
// {
//     struct movie *currMovie = malloc(sizeof(struct movie));

//     // For use with strtok_r
//     char *saveptr;

//     // The first token is the title
//     char *token = strtok_r(currLine, ",", &saveptr);
//     currMovie->title = calloc(strlen(token) + 1, sizeof(char));
//     strcpy(currMovie->title, token);

//     // The next token is the year
//     token = strtok_r(NULL, ",", &saveptr);
//     currMovie->year = atoi(token);

//     // The next token(s) is the languages

//     //Tokenize all the languages into one string
//     char *langPtr;
//     char *langToken = strtok_r(NULL, "[]", &saveptr);
//     char *langLine = langToken;
//     langToken = strtok_r(langLine, ";", &langPtr);

//     int index = 0;

//     //Tokenize language string into movie struct
//     while(langToken != NULL) {
//         strcpy(currMovie->languages[index], langToken);
//         langToken = strtok_r(NULL, ";", &langPtr);
//         index++;
//     };

//     // The last token is the rating
//     token = strtok_r(NULL, ",\n", &saveptr);
//     currMovie->rating = strtod(token, NULL);

//     // Set the next node to NULL in the newly created movie entry
//     currMovie->next = NULL;

//     return currMovie;
// }

// /*
// * Return a linked list of movies by parsing data from
// * each line of the specified file.
// */
// struct movie *processFile(char *filePath)
// {
//     // Open the specified file for reading only
//     FILE *movieFile = fopen(filePath, "r");

//     char *currLine = NULL;
//     char *stringBuffer = NULL;
//     size_t len = 0;
//     ssize_t nread;
//     char *token;

//     // The head of the linked list
//     struct movie *head = NULL;
//     // The tail of the linked list
//     struct movie *tail = NULL;

//     //remove first line from file
//     getline(&stringBuffer, &len, movieFile);
//     free(stringBuffer);

//     int numOfMovies = 0;

//     // Read the file line by line
//     while ((nread = getline(&currLine, &len, movieFile)) != -1)
//     {
//         // Get a new movie node corresponding to the current line
//         struct movie *newNode = createMovie(currLine);

//         // Is this the first node in the linked list?
//         if (head == NULL)
//         {
//             // This is the first node in the linked link
//             // Set the head and the tail to this node
//             head = newNode;
//             tail = newNode;
//         }
//         else
//         {
//             // This is not the first node.
//             // Add this node to the list and advance the tail
//             tail->next = newNode;
//             tail = newNode;
//         }
//         numOfMovies++;
//     }

//     printf("Now processing the chosen file %s\n\n", filePath);
//     free(currLine);
//     fclose(movieFile);
//     return head;
// }

// //Free all memory allocated for the movie linked list
// void freeList (struct movie *head) {
//     struct movie *temp;

//     while(head != NULL) {
//         temp = head;
//         head = head->next;
//         free(temp->title);
//         free(temp);
//     }
// }

// char * getLargestFile() {
//     DIR* currDir = opendir(".");
//     struct dirent *aDir;
//     struct stat dirStat;
//     int fileSize = 0;
//     char *largestFile = NULL;
//     char *saveptr, *ptr, *tempStr;

//     // Go through all the entries
//     while((aDir = readdir(currDir)) != NULL){

//         if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0){
//             tempStr = calloc(strlen(aDir->d_name) +1, sizeof(char));
//             strcpy(tempStr, aDir->d_name);
//             ptr = strtok_r(tempStr, ".", &saveptr);
//             ptr = strtok_r(NULL, ".", &saveptr);

//             // Get meta-data for the current entry
//             stat(aDir->d_name, &dirStat);
        
//         //Only grab files that end with extension "csv"
//         if((ptr != NULL && !strcmp(ptr, "csv")) && dirStat.st_size > fileSize){ 
//             //free memory allocated for previous largest file
// 	        if(largestFile != NULL)
// 		        free(largestFile);
//             largestFile = calloc(strlen(aDir->d_name) + 1, sizeof(char));
//             strcpy(largestFile, aDir->d_name);
//             fileSize = dirStat.st_size;
//         } 
// 	    free(tempStr);
//         }
//     }

//     // Close the directory
//     closedir(currDir);
//     return largestFile;
// }

// char * createDir() {
//     char *nameOfDir;
//     int randNum = random() % 100000;
//     int len = strlen("gonzedua.movies.") + 5;
//     nameOfDir = calloc(len + 1, sizeof(char));
//     sprintf(nameOfDir, "gonzedua_movies_%i" ,randNum);

//     int check = mkdir(nameOfDir, 0750);
//     if(check == -1)
//         printf("\nError in creating directory.");
    
//     return nameOfDir;
// }

// void populateFile(char *file, char *title) {
// 	int fd;

//     fd = open(file, O_RDWR | O_CREAT | O_APPEND, 0640);
// 	if (fd == -1)
// 		printf("open() failed on \"%s\"\n", file);

//     write(fd, title, strlen(title));
//     write(fd, "\r\n", 2);
//     close(fd);
// }

// void createNewFiles(char *dir, struct movie *list) {
//     char * newFilePath;
//     while(list != NULL) {
//         int len = strlen(dir) + 9;
//         newFilePath = calloc(len + 1, sizeof(char));
//         sprintf(newFilePath, "%s/%d.txt", dir, list->year );
//         populateFile(newFilePath, list->title);
//         free(newFilePath);
       
//        	list = list->next;
//     }

// }

// //Picks an action for the program to perform depending on user choice.
// void promptToProcess() {
//     char *fileToProcess;
//     int option;
//     do{
//         printf("\n\nWhich file do you want to process?\n Enter 1 to pick the largest file\n Enter 2 to pick the smallest file\n Enter 3 to specfiy the name of a file\n");

//         printf("\nEnter a choice from 1 to 3: ");
//         scanf("%i", &option);

//         switch(option) {
//             case 1:
//                 fileToProcess = getLargestFile();
//                 break;
//             case 2:
//                 printf("\n\nProcess smallest file");
//                 break;
//             case 3:
//                 printf("\n\nProcess user specified file");
//                 break;

//             default:
//                 printf("ERROR: Please choose an integer within the range of [1, 3]\n\n");
//         }

//     }while(option < 1 || option > 3);

//     struct movie *list = processFile(fileToProcess);
//     char *newDir = createDir();
//     createNewFiles(newDir, list);
//     //free any remaining memory that was allocated
//     freeList(list);
//     free(fileToProcess);
//     free(newDir);
// }

// // Prompt the user for what kind of information they want to see
// int initialPrompt() {
//     int answer;

//     printf("\n\n1. Select a file to process\n");
        
//     printf("\n2. Exit from the program\n");

//     printf("\nEnter a choice 1 or 2: ");
//     scanf("%i", &answer);

//     return answer;
// }

// /*
// *   Process the file provided as an argument to the program to
// *   create a linked list of movie structs and print out the list.
// *   Compile the program as follows:
// *       gcc --std=gnu99 -o movies_by_year main.c
// */

// int main()
// {
//     srandom(time(NULL));
//     int choice;
//     do{
//         choice = initialPrompt();
//         if(choice == 1)
//             promptToProcess();
//         else if (choice == 2)
//             printf("Goodbye!");
//         else
//             printf("ERROR: Please choose an integer within the range of [1, 2]\n\n");

//     }while(choice != 2);

//     return EXIT_SUCCESS;
// }
