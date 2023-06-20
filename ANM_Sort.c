#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_INICIAL 10000
#define TAMANHO_INCREMENTO 10000

struct Index{
    int start;
    int end;
    char fase[38];
};

int compara(struct Index* a, struct Index* b){
    return strcmp(a->fase, b->fase);
}

int main() {
    int ultLinha = 0;
    int priLinha = 0;
    int flag = 0;
    int j = 4;
    int qntProcessos = 0;
    int tamanhoIndex = 1;
    int tamanhoQntd = 1;
    struct Index* listaIndex =  malloc(tamanhoIndex * sizeof(struct Index)); 
    int* qntd = malloc(tamanhoQntd * sizeof(int));

    // Abrir o arquivo para leitura
    FILE *arquivo = fopen("MG.kml", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Alocar o vetor inicial com tamanho inicial
    char **linhas = (char **) malloc(TAMANHO_INICIAL * sizeof(char *));
    if (linhas == NULL) {
        printf("Erro ao alocar memória.\n");
        fclose(arquivo);
        return 1;
    }

    int tamanho_vetor = TAMANHO_INICIAL;
    int num_linhas = 0;
    char buffer[1024]; // Tamanho máximo da linha

    // Ler o arquivo linha por linha
    while (fgets(buffer, sizeof(buffer), arquivo) != NULL) {
        // Alocar memória para a linha
        linhas[num_linhas] = (char *) malloc((strlen(buffer) + 1) * sizeof(char));
        if (linhas[num_linhas] == NULL) {
            printf("Erro ao alocar memória.\n");
            fclose(arquivo);

            // Liberar a memória alocada anteriormente
            for (int i = 0; i < num_linhas; i++) {
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
            tamanho_vetor += TAMANHO_INCREMENTO;
            linhas = (char **) realloc(linhas, tamanho_vetor * sizeof(char *));
            if (!linhas) {
                printf("Erro ao realocar memória.\n");
                fclose(arquivo);

                // Liberar a memória alocada anteriormente
                for (int i = 0; i < num_linhas; i++) {
                    free(linhas[i]);
                }
                free(linhas);

                return 1;
            }
        }
    }
    fclose(arquivo);

    for (int i = 0; i < num_linhas; i++) {
        //Encontra a linha anterior aos processos
        if(strcmp(linhas[i], "    <Placemark>\n") == 0){
            qntProcessos++;
            if(!flag){
                priLinha = i;
                flag = 1;
            }
            listaIndex[tamanhoIndex - 1].start = i;
        }
        else if(strcmp(linhas[i], "<td>Fase</td>\r\n") == 0){
            while(linhas[i+1][j] != '<'){
                listaIndex[tamanhoIndex - 1].fase[j-4] = linhas[i+1][j];
                j++;
            }
            listaIndex[tamanhoIndex - 1].fase[j-4] = '\0';
            j = 4;
            //printf("\n");
        }
        //Encontra ultima linha antes de fechar o arquivo
        else if(strcmp(linhas[i], "    </Placemark>\n") == 0){
            ultLinha = i;
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
    ultLinha ++;

    qsort(listaIndex, tamanhoIndex - 1, sizeof(struct Index), compara);

    qntd[tamanhoQntd - 1] = 1;

    for(int i=0;i<tamanhoIndex-1;i++){
        //printf("Start %d: %d\n",i+1,listaIndex[i].start);
        //printf("Fase %d: %s\n",i+1,listaIndex[i].fase);
        //printf("End %d: %d\n",i+1,listaIndex[i].end);
        //printf("\n");
        if(i>0){
            if(!strcmp(listaIndex[i].fase, listaIndex[i-1].fase)){
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

    FILE* newFile = fopen("MG.kml", "w");
    if (newFile == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    for(int i=0;i<priLinha;i++){
        fprintf(newFile,"%s",linhas[i]);
    }

    int aux2 = 0;
    int aux = 0;
    fprintf(newFile,"\t\t<Folder>\n\t\t\t<name>%s</name>\n",listaIndex[aux].fase);
    for(int i=0;i<tamanhoIndex-1;i++){
        if(qntd[aux] == aux2){
            aux2 = 0;
            aux++;
            fprintf(newFile,"</Folder>\n");
            fprintf(newFile,"\t\t<Folder>\n\t\t\t<name>%s</name>\n",listaIndex[i].fase);
        }
        for(int j=listaIndex[i].start;j<=listaIndex[i].end;j++){
            fprintf(newFile, "%s",linhas[j]);
        }
        aux2++;
    }
    fprintf(newFile,"</Folder>\n");

    
    for(int i=ultLinha;i<num_linhas;i++){
        fprintf(newFile,"%s",linhas[i]);
    }
    
    fclose(newFile);
    printf("TERMINOU");

    // Liberar a memória alocada
    for (int i = 0; i < num_linhas; i++) {
        free(linhas[i]);
    }
    free(linhas);
    free(listaIndex);
    free(qntd);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_INICIAL 10000
#define TAMANHO_INCREMENTO 10000

struct Index{
    int start;
    int end;
    char fase[38];
};

int compara(struct Index* a, struct Index* b){
    return strcmp(a->fase, b->fase);
}

int main() {
    int ultLinha = 0;
    int priLinha = 0;
    int flag = 0;
    int j = 4;
    int qntProcessos = 0;
    int tamanhoIndex = 1;
    int tamanhoQntd = 1;
    struct Index* listaIndex =  malloc(tamanhoIndex * sizeof(struct Index)); 
    int* qntd = malloc(tamanhoQntd * sizeof(int));

    // Abrir o arquivo para leitura
    FILE *arquivo = fopen("MG.kml", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Alocar o vetor inicial com tamanho inicial
    char **linhas = (char **) malloc(TAMANHO_INICIAL * sizeof(char *));
    if (linhas == NULL) {
        printf("Erro ao alocar memória.\n");
        fclose(arquivo);
        return 1;
    }

    int tamanho_vetor = TAMANHO_INICIAL;
    int num_linhas = 0;
    char buffer[1024]; // Tamanho máximo da linha

    // Ler o arquivo linha por linha
    while (fgets(buffer, sizeof(buffer), arquivo) != NULL) {
        // Alocar memória para a linha
        linhas[num_linhas] = (char *) malloc((strlen(buffer) + 1) * sizeof(char));
        if (linhas[num_linhas] == NULL) {
            printf("Erro ao alocar memória.\n");
            fclose(arquivo);

            // Liberar a memória alocada anteriormente
            for (int i = 0; i < num_linhas; i++) {
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
            tamanho_vetor += TAMANHO_INCREMENTO;
            linhas = (char **) realloc(linhas, tamanho_vetor * sizeof(char *));
            if (!linhas) {
                printf("Erro ao realocar memória.\n");
                fclose(arquivo);

                // Liberar a memória alocada anteriormente
                for (int i = 0; i < num_linhas; i++) {
                    free(linhas[i]);
                }
                free(linhas);

                return 1;
            }
        }
    }
    fclose(arquivo);

    for (int i = 0; i < num_linhas; i++) {
        //Encontra a linha anterior aos processos
        if(strcmp(linhas[i], "    <Placemark>\n") == 0){
            qntProcessos++;
            if(!flag){
                priLinha = i;
                flag = 1;
            }
            listaIndex[tamanhoIndex - 1].start = i;
        }
        else if(strcmp(linhas[i], "<td>Fase</td>\r\n") == 0){
            while(linhas[i+1][j] != '<'){
                listaIndex[tamanhoIndex - 1].fase[j-4] = linhas[i+1][j];
                j++;
            }
            listaIndex[tamanhoIndex - 1].fase[j-4] = '\0';
            j = 4;
            //printf("\n");
        }
        //Encontra ultima linha antes de fechar o arquivo
        else if(strcmp(linhas[i], "    </Placemark>\n") == 0){
            ultLinha = i;
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
    ultLinha ++;

    qsort(listaIndex, tamanhoIndex - 1, sizeof(struct Index), compara);

    qntd[tamanhoQntd - 1] = 1;

    for(int i=0;i<tamanhoIndex-1;i++){
        //printf("Start %d: %d\n",i+1,listaIndex[i].start);
        //printf("Fase %d: %s\n",i+1,listaIndex[i].fase);
        //printf("End %d: %d\n",i+1,listaIndex[i].end);
        //printf("\n");
        if(i>0){
            if(!strcmp(listaIndex[i].fase, listaIndex[i-1].fase)){
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

    FILE* newFile = fopen("MG.kml", "w");
    if (newFile == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    for(int i=0;i<priLinha;i++){
        fprintf(newFile,"%s",linhas[i]);
    }

    int aux2 = 0;
    int aux = 0;
    fprintf(newFile,"\t\t<Folder>\n\t\t\t<name>%s</name>\n",listaIndex[aux].fase);
    for(int i=0;i<tamanhoIndex-1;i++){
        if(qntd[aux] == aux2){
            aux2 = 0;
            aux++;
            fprintf(newFile,"</Folder>\n");
            fprintf(newFile,"\t\t<Folder>\n\t\t\t<name>%s</name>\n",listaIndex[i].fase);
        }
        for(int j=listaIndex[i].start;j<=listaIndex[i].end;j++){
            fprintf(newFile, "%s",linhas[j]);
        }
        aux2++;
    }
    fprintf(newFile,"</Folder>\n");

    
    for(int i=ultLinha;i<num_linhas;i++){
        fprintf(newFile,"%s",linhas[i]);
    }
    
    fclose(newFile);
    printf("TERMINOU");

    // Liberar a memória alocada
    for (int i = 0; i < num_linhas; i++) {
        free(linhas[i]);
    }
    free(linhas);
    free(listaIndex);
    free(qntd);

    return 0;
}
