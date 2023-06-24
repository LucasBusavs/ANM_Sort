/*
Algorith to sort a ANM database by its phases
C version to be more efficient
*/

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Constants
#define INITIAL_SIZE 10000
#define INCREMENT_SIZE 10000

//Block indexing structure
struct Index{
    int start;
    int end;
    char phaseType[38];  //Maximum size of phaseType type
};

int readLines(int* numLines, char*** lines){
    int i;
    int linesSize = INITIAL_SIZE;
    char buffer[1024]; //Maximum length of the line

    *lines = (char **) malloc(linesSize * sizeof(char *));
    if (!(*lines)) {
        printf("Error allocating memory.\n");
        return 1;
    }
    
    //Opening file for reading
    FILE *f = fopen("PB.kml", "r");
    if (!f) {
        printf("Error opening file.\n");
        return 1;
    }

    //Reading file line by line
    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        //Allocate memory for lines
        (*lines)[(*numLines)] = (char *) malloc((strlen(buffer) + 1) * sizeof(char));
        if (!(*lines)[(*numLines)]) {
            printf("Error allocating memory.\n");
            fclose(f);
            //Free previously allocated memory
            for (i = 0; i < *numLines; i++) {
                free(lines[i]);
            }
            free(lines);
            return 1;
        }

        //Coping line into the array of lines
        strcpy((*lines)[(*numLines)], buffer);
        (*numLines)++;

        //Check if lines needs to be reallocated
        if (*numLines >= linesSize) {
            linesSize += INCREMENT_SIZE;
            *lines = (char **) realloc(*lines, linesSize * sizeof(char *));
            if (!(*lines)) {
                printf("Error reallocating memory.\n");
                fclose(f);
                //Free previously allocated memory
                for (i = 0; i < (*numLines); i++) {
                    free(lines[i]);
                }
                free(lines);
                return 1;
            }
        }
    }
    fclose(f);
    return 0;
    //Finished reading the file
}

int indexing(struct Index** listIndex, int numLines, int* qntProcess, char** lines, int* firstLine, int* lastLine){
    int j = 4;
    int flag = 0;
    int sizeIndex = 1;

    *listIndex = (struct Index*) malloc(sizeIndex * sizeof(struct Index)); 
    if (!(*listIndex)) {
        printf("Error allocating memory.\n");
        return 1;
    }

    //Generate the listIndex of all mining processes
    for (int i = 0; i < numLines; i++) {
        if(strcmp(lines[i], "    <Placemark>\n") == 0){
            (*qntProcess)++;
            if(!flag){
                *firstLine = i;
                flag = 1;
            }
            (*listIndex)[sizeIndex - 1].start = i;
        }
        else if(strcmp(lines[i], "<td>Fase</td>\r\n") == 0){
            while(lines[i+1][j] != '<'){
                (*listIndex)[sizeIndex - 1].phaseType[j-4] = lines[i+1][j];
                j++;
            }
            (*listIndex)[sizeIndex - 1].phaseType[j-4] = '\0';
            j = 4;
        }
        else if(strcmp(lines[i], "    </Placemark>\n") == 0){
            *lastLine = i + 1;
            (*listIndex)[sizeIndex - 1].end = i;
            *listIndex = realloc(*listIndex, ++sizeIndex * sizeof(struct Index));
            if(!(*listIndex)){
                printf("Error reallocating memory.\n");
                free(listIndex);
                return 1;
            }
        }
    }
    return 0;
}

/*
Description: Function to compare two phases types, in alphabet order
Parameters: 2 blocks 
*/
int compare(struct Index* a, struct Index* b){
    return strcmp(a->phaseType, b->phaseType);
}

int phaseFrequency(int** qnt, struct Index* listIndex, int qntProcess){
    int sizeQnt = 1;
    *qnt = (int*)malloc(sizeQnt * sizeof(int));
    if (!(*qnt)) {
        printf("Error allocating memory.\n");
        return 1;
    }

    (*qnt)[sizeQnt - 1] = 1;   //Define first position equal to 1
    for (int i = 1; i < qntProcess; i++) {
        if (!strcmp(listIndex[i].phaseType, listIndex[i - 1].phaseType)) {
            (*qnt)[sizeQnt - 1]++;
        }
        else {
            printf("%s\t%d\n", listIndex[i - 1].phaseType, (*qnt)[sizeQnt - 1]);
            sizeQnt++;
            *qnt = (int*)realloc(*qnt, sizeQnt * sizeof(int));
            if (!(*qnt)) {
                printf("Error reallocating memory.\n");
                return 1;
            }
            (*qnt)[sizeQnt - 1] = 1;
        }
    }
    return 0;
}

int write(int firstLine, int lastLine, int qntProcess, int numLines, struct Index* listIndex, int* qnt, char** lines){
    int i;
    int aux = 0, aux2 = 0;

    FILE* newFile = fopen("PB.kml", "w");
    if (!newFile) {
        printf("Error opening file.\n");
        return 1;
    }

    //Writing the begining of the file, like the old one
    for(i = 0; i < firstLine; i++){
        fprintf(newFile,"%s",lines[i]);
    }

    //Writing the middle of the file, creating a folder for every phaseType
    fprintf(newFile,"\t\t<Folder>\n\t\t\t<name>%s</name>\n",listIndex[aux].phaseType);
    for(i = 0; i < qntProcess; i++){
        if(qnt[aux] == aux2){
            aux2 = 0;
            aux++;
            fprintf(newFile,"</Folder>\n");
            fprintf(newFile,"\t\t<Folder>\n\t\t\t<name>%s</name>\n",listIndex[i].phaseType);
        }
        for(int j = listIndex[i].start; j <= listIndex[i].end; j++){
            fprintf(newFile, "%s",lines[j]);
        }
        aux2++;
    }
    fprintf(newFile,"</Folder>\n");

    //Writing the end of the file, like the old one
    for(i = lastLine; i < numLines; i++){
        fprintf(newFile,"%s",lines[i]);
    }
    fclose(newFile);
    return 0;
}

int main(){
    clock_t start = clock();
    int lastLine = 0, firstLine = 0;
    int qntProcess = 0;
    int numLines = 0;
    int* qnt;
    char** lines;
    struct Index* listIndex;

    if(readLines(&numLines, &lines)){
        printf("Error reading lines\n");
        return 1;
    }
    
    if(indexing(&listIndex, numLines, &qntProcess, lines, &firstLine, &lastLine)){
        printf("Error indexing the file\n");
        return 1;
    }

    //Quick sorting lisIndex alphabetically, by phaseType
    qsort(listIndex, qntProcess, sizeof(struct Index), compare);

    if(phaseFrequency(&qnt, listIndex, qntProcess)){
        printf("Error calculating frequency of phases\n");
        return 1;
    }

    if(write(firstLine, lastLine, qntProcess, numLines, listIndex, qnt, lines)){
        printf("Error writing file\n");
        return 1;
    }

    printf("\nFINISHED");

    for (int i = 0; i < numLines; i++) {
        free(lines[i]);
    }
    free(lines);
    free(listIndex);
    free(qnt);

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n%.4f seconds elapsed", elapsed);
    
    return 0;
}