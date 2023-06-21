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
    int qntProcessos = 0;
    int tamanhoIndex = 1, tamanhoQntd = 1;
    struct Index* listaIndex =  malloc(tamanhoIndex * sizeof(struct Index)); 
    int* qntd = malloc(tamanhoQntd * sizeof(int));

    //Opening file for reading
    FILE *f = fopen("PB.kml", "r");
    if (!f) {
        printf("Error opening file.\n");
        return 1;
    }

    // Alocar o vetor inicial com tamanho inicial
    char **linhas = (char **) malloc(INITIAL_SIZE * sizeof(char *));
    if (!linhas) {
        printf("Erro ao alocar memória.\n");
        fclose(f);
        return 1;
    }

    int tamanho_vetor = INITIAL_SIZE;
    int num_linhas = 0;
    char buffer[1024]; // Tamanho máximo da linha

    //Reading file line by line
    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        // Alocar memória para a linha
        linhas[num_linhas] = (char *) malloc((strlen(buffer) + 1) * sizeof(char));
        if (!linhas[num_linhas]) {
            printf("Erro ao alocar memória.\n");
            fclose(f);

            // Liberar a memória alocada anteriormente
            for (i = 0; i < num_linhas; i++) {
                free(linhas[i]);
            }
            free(linhas);

            return 1;
        }

        // Copiar a linha para o vetor
        strcpy(linhas[num_linhas], buffer);
        num_linhas++;

        // Verificar se o vetor precisa ser realocado para acomodar mais linhas
        if (num_linhas >= tamanho_vetor) {
            tamanho_vetor += INCREMENT_SIZE;
            linhas = (char **) realloc(linhas, tamanho_vetor * sizeof(char *));
            if (!linhas) {
                printf("Erro ao realocar memória.\n");
                fclose(f);

                // Liberar a memória alocada anteriormente
                for (int i = 0; i < num_linhas; i++) {
                    free(linhas[i]);
                }
                free(linhas);

                return 1;
            }
        }
    }
    fclose(f);

    for (i = 0; i < num_linhas; i++) {
        //Encontra a linha anterior aos processos
        if(strcmp(linhas[i], "    <Placemark>\n") == 0){
            qntProcessos++;
            if(!flag){
                firstLine = i;
                flag = 1;
            }
            listaIndex[tamanhoIndex - 1].start = i;
        }
        else if(strcmp(linhas[i], "<td>Fase</td>\r\n") == 0){
            while(linhas[i+1][j] != '<'){
                listaIndex[tamanhoIndex - 1].phaseType[j-4] = linhas[i+1][j];
                j++;
            }
            listaIndex[tamanhoIndex - 1].phaseType[j-4] = '\0';
            j = 4;
        }
        else if(strcmp(linhas[i], "    </Placemark>\n") == 0){
            lastLine = i;
            listaIndex[tamanhoIndex - 1].end = i;
            tamanhoIndex++;
            int* novaLista = realloc(listaIndex, tamanhoIndex * sizeof(struct Index));
            if(!novaLista){
                printf("Erro ao realocar memória!\n");
                free(listaIndex);
                return 1;
            }
            listaIndex = novaLista;
        }
    }
    lastLine ++;

    qsort(listaIndex, tamanhoIndex - 1, sizeof(struct Index), compare);

    qntd[tamanhoQntd - 1] = 1;

    for(i = 0; i < tamanhoIndex-1; i++){
        if(i>0){
            if(!strcmp(listaIndex[i].phaseType, listaIndex[i-1].phaseType)){
                qntd[tamanhoQntd - 1]++;
            }
            else{
                tamanhoQntd++;
                int* novaQntd = realloc(qntd, tamanhoQntd * sizeof(int));
                if(!novaQntd){
                    printf("Erro ao realocar memória!\n");
                    free(qntd);
                    return 1;
                }
                qntd = novaQntd;
                qntd[tamanhoQntd - 1] = 1;
            }
        }
    }

    /*
    for(int i=0;i<tamanhoQntd;i++){
        printf("%d ", qntd[i]);
    }
    */
    //printf("\n%d\n", qntProcessos);
    //printf("%d\n", tamanhoIndex);

    FILE* newFile = fopen("PB.kml", "w");
    if (!newFile) {
        printf("Error opening file.\n");
        return 1;
    }

    for(i = 0; i < firstLine; i++){
        fprintf(newFile,"%s",linhas[i]);
    }

    int aux2 = 0;
    int aux = 0;
    fprintf(newFile,"\t\t<Folder>\n\t\t\t<name>%s</name>\n",listaIndex[aux].phaseType);
    for(i = 0; i < tamanhoIndex-1; i++){
        if(qntd[aux] == aux2){
            aux2 = 0;
            aux++;
            fprintf(newFile,"</Folder>\n");
            fprintf(newFile,"\t\t<Folder>\n\t\t\t<name>%s</name>\n",listaIndex[i].phaseType);
        }
        for(int j = listaIndex[i].start; j <= listaIndex[i].end; j++){
            fprintf(newFile, "%s",linhas[j]);
        }
        aux2++;
    }
    fprintf(newFile,"</Folder>\n");
    
    for(i = lastLine; i < num_linhas; i++){
        fprintf(newFile,"%s",linhas[i]);
    }
    
    fclose(newFile);
    printf("TERMINOU");

    // Liberar a memória alocada
    for (i = 0; i < num_linhas; i++) {
        free(linhas[i]);
    }
    free(linhas);
    free(listaIndex);
    free(qntd);

    return 0;
}