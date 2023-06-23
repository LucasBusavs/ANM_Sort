/*
Algorith to sort a ANM database by its phases
C version to be more efficient
*/

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Constants
#define INITIAL_SIZE 10000
#define INCREMENT_SIZE 10000

//Block indexing structure
struct Index{
    int start;
    int end;
    char phaseType[38];  //Maximum size of phaseType type
};

/*
Description: Function to compare two phases types, in alphabet order
Parameters: 2 blocks 
*/
int compare(struct Index* a, struct Index* b){
    return strcmp(a->phaseType, b->phaseType);
}

int main() {
    int lastLine = 0, firstLine = 0;
    int flag = 0;
    int j = 4, i;
    int qntProcess = 0;
    int sizeIndex = 1, sizeQnt = 1;
    int linesSize = INITIAL_SIZE;
    int numLines = 0;
    char buffer[1024]; //Maximum length of the line 

    //********Alocating memory**********
    //Memory allocation for a matrix containing the file's lines, line by line
    char** lines = (char **) malloc(linesSize * sizeof(char *));
    if (!lines) {
        printf("Error allocating memory.\n");
        return 1;
    }
    
    struct Index* listIndex = (struct Index*) malloc(sizeIndex * sizeof(struct Index)); 
    if (!listIndex) {
        printf("Error allocating memory.\n");
        return 1;
    }
    
    int* qnt = (int*) malloc(sizeQnt * sizeof(int));
    if (!qnt) {
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
        lines[numLines] = (char *) malloc((strlen(buffer) + 1) * sizeof(char));
        if (!lines[numLines]) {
            printf("Error allocating memory.\n");
            fclose(f);
            //Free previously allocated memory
            for (i = 0; i < numLines; i++) {
                free(lines[i]);
            }
            free(lines);
            return 1;
        }

        //Coping line into the array of lines
        strcpy(lines[numLines], buffer);
        numLines++;

        //Check if lines needs to be reallocated
        if (numLines >= linesSize) {
            linesSize += INCREMENT_SIZE;
            lines = (char **) realloc(lines, linesSize * sizeof(char *));
            if (!lines) {
                printf("Error reallocating memory.\n");
                fclose(f);
                //Free previously allocated memory
                for (int i = 0; i < numLines; i++) {
                    free(lines[i]);
                }
                free(lines);
                return 1;
            }
        }
    }
    fclose(f);
    //Finished reading the file

    //Generate the listIndex of all mining processes
    for (i = 0; i < numLines; i++) {
        if(strcmp(lines[i], "    <Placemark>\n") == 0){
            qntProcess++;
            if(!flag){
                firstLine = i;
                flag = 1;
            }
            listIndex[sizeIndex - 1].start = i;
        }
        else if(strcmp(lines[i], "<td>Fase</td>\r\n") == 0){
            while(lines[i+1][j] != '<'){
                listIndex[sizeIndex - 1].phaseType[j-4] = lines[i+1][j];
                j++;
            }
            listIndex[sizeIndex - 1].phaseType[j-4] = '\0';
            j = 4;
        }
        else if(strcmp(lines[i], "    </Placemark>\n") == 0){
            lastLine = i + 1;
            listIndex[sizeIndex - 1].end = i;
            listIndex = realloc(listIndex, ++sizeIndex * sizeof(struct Index));
            if(!listIndex){
                printf("Error reallocating memory.\n");
                free(listIndex);
                return 1;
            }
        }
    }

    //Quick sorting lisIndex alphabetically, by phaseType
    qsort(listIndex, qntProcess, sizeof(struct Index), compare);

    //Counting the quantity of each phaseType in the file
    qnt[sizeQnt - 1] = 1;   //Define first position equal to 1
    for(i = 1; i < qntProcess; i++){
        if(!strcmp(listIndex[i].phaseType, listIndex[i-1].phaseType)){
            qnt[sizeQnt - 1]++;
        }
        else{
            //printf("%d\n",qnt[sizeQnt - 1]);
            qnt = realloc(qnt, ++sizeQnt * sizeof(int));
            if(!qnt){
                printf("Error reallocating memory.\n");
                free(qnt);
                return 1;
            }
            qnt[sizeQnt - 1] = 1;
        }
    }

    //Overwrinting the same file
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
    int aux = 0, aux2 = 0;
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
    printf("FINISHED");

    for (i = 0; i < numLines; i++) {
        free(lines[i]);
    }
    free(lines);
    free(listIndex);
    free(qnt);

    return 0;
}