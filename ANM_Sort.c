#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_INICIAL 10000
#define TAMANHO_INCREMENTO 10000

int main() {
    int ultLinha = 0;
    int priLinha = 0;
    int flag = 0;
    int fases = 0;
    // Abrir o arquivo para leitura
    FILE *arquivo = fopen("PB.kml", "r");
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

    // Imprimir as linhas armazenadas no vetor
    for (int i = 0; i < num_linhas; i++) {
        //printf("Linha %d: %s\n", i+1, linhas[i]);
        if(strcmp(linhas[i], "    </Placemark>\n") == 0){
            ultLinha = i;
        }
        if(strcmp(linhas[i], "    <Placemark>\n") == 0 && !flag){
            priLinha = i;
            flag = 1;
        }
        if(strcmp(linhas[i], "<td>Fase</td>\r\n") == 0){
            printf("Linha %d: %s\n", i+2, linhas[i+1]);
        }
    }
    printf("%d\n", ultLinha);
    printf("%d\n", priLinha);

    // Liberar a memória alocada
    for (int i = 0; i < num_linhas; i++) {
        free(linhas[i]);
    }
    free(linhas);

    return 0;
}
