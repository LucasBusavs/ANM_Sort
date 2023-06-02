#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define TAMANHO_BLOCO 65536

void mostra() {
    char c[TAMANHO_BLOCO];
    FILE *f = fopen("PB.kml", "r");
    if (!f) {
        printf("Error");
        exit(1);
    }
    
    size_t bytesRead;
    char buffer[TAMANHO_BLOCO * 10];  // Buffer para armazenar múltiplas linhas
    size_t bufferIndex = 0;
    
    while ((bytesRead = fread(c, 1, sizeof(c), f)) > 0) {
        if (bufferIndex + bytesRead >= sizeof(buffer)) {
            // O buffer está cheio, imprime seu conteúdo
            fwrite(buffer, 1, bufferIndex, stdout);
            bufferIndex = 0;
        }
        
        // Copia os dados para o buffer
        memcpy(buffer + bufferIndex, c, bytesRead);
        bufferIndex += bytesRead;
    }
    
    // Imprime qualquer conteúdo restante no buffer
    fwrite(buffer, 1, bufferIndex, stdout);

    fclose(f);
}

int main() {
    setlocale(LC_ALL, "Portuguese_Brasil");
    clock_t start = clock();
    mostra();
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n%.4f seconds elapsed", elapsed);
    return 0;
}