/**
 * Criação e Gerenciamento de Threads
 *
 * Aluno: Raphael Ramos da Silva
 * Matricula: 20200081884
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Faz a leitura do arquivo pgm
 * @param path
 * @param width
 * @param height
 * @param max_value
 */
int** read_image(char* path, int* width, int* height, int* max_value) {
    FILE* file;
    char magic[3];
    int** pixels;

    // Abra o arquivo PGM para leitura
    file = fopen(path, "r");

    // Leia o cabeçalho PGM
    fscanf(file, "%s", magic);

    // Leia a largura, altura e valor máximo
    fscanf(file, "%d %d", width, height);
    fscanf(file, "%d", max_value);

    // Aloque memória para a matriz 2D de pixels
    pixels = (int**)malloc((*height) * sizeof(int*));
    for (int i = 0; i < *height; i++) {
        pixels[i] = (int*)malloc((*width) * sizeof(int));
    }

    // Leia os valores de pixel e preencha a matriz 2D
    for (int i = 0; i < *height; i++) {
        for (int j = 0; j < *width; j++) {
            fscanf(file, "%d", &pixels[i][j]);
        }
    }

    // Fechar o arquivo
    fclose(file);

    return pixels;
}

/**
 * Identifica contorno na imagem
 * @param m altura
 * @param n largura
 */
int** process_image(int** img, int m, int n) {
    int gx[m][n];
    int gy[m][n];

    int** g = (int**)malloc((m) * sizeof(int*));
    for (int i = 0; i < m; i++) {
        g[i] = (int*)malloc((n) * sizeof(int));
    }

    // Calculando as informações de bordas
    for (int i = 1; i < m - 2; i++) {
        for (int j = 1; j < n - 2; j++) {
            // Thread para calculo de Gx
            gx[i][j] = (img[i + 1][j - 1] + img[i + 1][j] + img[i + 1][j + 1]) -
                       (img[i - 1][j - 1] + img[i - 1][j] + img[i - 1][j + 1]);
            if (gx[i][j] < 0) gx[i][j] = 0;
            if (gx[i][j] > 255) gx[i][j] = 255;

            // Thread para calculo de Gy
            gy[i][j] = (img[i - 1][j + 1] + img[i][j + 1] + img[i + 1][j + 1]) -
                       (img[i - 1][j - 1] + img[i][j - 1] + img[i + 1][j - 1]);
            if (gy[i][j] < 0) gy[i][j] = 0;
            if (gy[i][j] > 255) gy[i][j] = 255;
        }
    }


    for (int i = 0; i < m - 1; i++) {
        for (int j = 0; j < n - 1; j++) {
            g[i][j] = gx[i][j] + gy[i][j];
            if (g[i][j] > 255) g[i][j] = 255;
        }
    }

    return g;
}

void export_image(char* path, int** img, int width, int height, int max_value) {
    FILE* file = fopen(path, "w");
    fprintf(file, "P2\n");
    fprintf(file, "# Contorno das moedas \n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "%d\n", max_value);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(file, "%d ", img[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

int main() {
    int** pixels;       // Matriz de pixels da imagem
    int** g;            // Matriz resultado do processamento
    int width, height;  // Largura e altura
    int max_value;      // Valor maximo dos pixels

    // Ler imagem
    pixels = read_image("coins.ascii.pgm", &width, &height, &max_value);

    // Processar imagem com threads
    g = process_image(pixels, height, width);

    // Exportar imagem
    export_image("coins-processed.ascii.pgm", g, width, height, max_value);

    // Liberar memoria ocupada pelas matrizes
    for (int i = 0; i < height; i++) {
        free(pixels[i]);
        free(g[i]);
    }

    free(pixels);
    free(g);

    return 0;
}
