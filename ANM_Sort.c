#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TAMANHO_LINHA 65536

void mostra(){
    int qnt = 0;
    char c[TAMANHO_LINHA];
    FILE *f = fopen("PB.kml", "r");
    if(!f){
        printf("Error");
        exit(1);
    }
    while(fgets(c, TAMANHO_LINHA, f) != NULL){
        if(strcmp(c, "<td>Fase</td>") == 0){
            qnt++;
        }
    }
    fclose(f);
    printf("%d\n", qnt);
}

int main(){
    clock_t start = clock();
    mostra();
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n%.4f seconds elapsed", elapsed);
    return 0;
}