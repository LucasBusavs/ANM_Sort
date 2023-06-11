#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_INICIAL 10000
#define TAMANHO_INCREMENTO 10000

int main() {
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
        // Remover o caractere de nova linha (\n) do final da linha
        //buffer[strcspn(buffer, "\n")] = '\0';

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
            if (linhas == NULL) {
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

    // Fechar o arquivo
    fclose(arquivo);

    FILE *file = fopen("test.kml","w");
    if (!file) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }
    // Imprimir as linhas armazenadas no vetor
    for (int i = 0; i < num_linhas; i++) {
        fputs(linhas[i], file);
        printf("Linha %d: %s\n", i+1, linhas[i]);
    }
    fclose(file);

    // Liberar a memória alocada
    for (int i = 0; i < num_linhas; i++) {
        free(linhas[i]);
    }
    free(linhas);

    return 0;
}
