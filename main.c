#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Temperaturas fixas das bordas da placa */
#define TEMP_BORDA_CIMA 100.0
#define TEMP_BORDA_BAIXO 0.0
#define TEMP_BORDA_ESQUERDA 50.0
#define TEMP_BORDA_DIREITA 50.0

/* Maior temperatura sorteada para o interior */
#define TEMP_MAXIMA_INTERIOR 100

/* Quantidade de vizinhos usados na media (cima, baixo, esquerda, direita) */
#define NUM_VIZINHOS 4.0

/* Converte (linha, coluna) na posicao do vetor que guarda a matriz */
int indice(int linha, int coluna, int n)
{
    return linha * n + coluna;
}

/* Retorna o tempo atual em segundos */
double agora(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

/* Preenche a matriz: bordas fixas e interior aleatorio */
void gerar_matriz(double *placa, int n, unsigned int semente)
{
    srand(semente);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int pos = indice(i, j, n);

            if (i == 0)
            {
                placa[pos] = TEMP_BORDA_CIMA;
            }
            else if (i == n - 1)
            {
                placa[pos] = TEMP_BORDA_BAIXO;
            }
            else if (j == 0)
            {
                placa[pos] = TEMP_BORDA_ESQUERDA;
            }
            else if (j == n - 1)
            {
                placa[pos] = TEMP_BORDA_DIREITA;
            }
            else
            {
                placa[pos] = rand() % (TEMP_MAXIMA_INTERIOR + 1);
            }
        }
    }
}

/* Uma iteracao: cada celula interna vira a media dos 4 vizinhos */
void passo_simulacao(const double *atual, double *nova, int n)
{
    for (int i = 1; i < n - 1; i++)
    {
        for (int j = 1; j < n - 1; j++)
        {
            double cima = atual[indice(i - 1, j, n)];
            double baixo = atual[indice(i + 1, j, n)];
            double esquerda = atual[indice(i, j - 1, n)];
            double direita = atual[indice(i, j + 1, n)];

            nova[indice(i, j, n)] = (cima + baixo + esquerda + direita) / NUM_VIZINHOS;
        }
    }
}

/* Troca as matrizes: a nova vira a atual na proxima iteracao */
void trocar_matrizes(double **atual, double **nova)
{
    double *tmp = *atual;
    *atual = *nova;
    *nova = tmp;
}

/* Soma todos os valores (serve para conferir o resultado) */
double somar(const double *placa, int n)
{
    double soma = 0.0;
    for (int k = 0; k < n * n; k++)
    {
        soma += placa[k];
    }
    return soma;
}

int main(int argc, char *argv[])
{
    if (argc != 3 && argc != 4)
    {
        fprintf(stderr, "Uso: %s <N> <T> [semente]\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);         /* tamanho da matriz N x N */
    int iteracoes = atoi(argv[2]); /* quantas iteracoes rodar */

    unsigned int semente;
    if (argc == 4)
    {
        semente = (unsigned int)strtoul(argv[3], NULL, 10);
    }
    else
    {
        semente = (unsigned int)time(NULL);
    }

    if (n < 3 || iteracoes < 1)
    {
        fprintf(stderr, "Erro: use N >= 3 e T >= 1\n");
        return 1;
    }

    double *atual = malloc(n * n * sizeof(double));
    double *nova = malloc(n * n * sizeof(double));
    if (atual == NULL || nova == NULL)
    {
        fprintf(stderr, "Erro: memoria insuficiente\n");
        free(atual);
        free(nova);
        return 1;
    }

    gerar_matriz(atual, n, semente);
    gerar_matriz(nova, n, semente);

    double inicio = agora();
    for (int t = 0; t < iteracoes; t++)
    {
        passo_simulacao(atual, nova, n);
        trocar_matrizes(&atual, &nova);
    }
    double fim = agora();

    printf("N=%d  T=%d  semente=%u\n", n, iteracoes, semente);
    printf("Tempo: %.4f segundos\n", fim - inicio);
    printf("Soma final: %.6f\n", somar(atual, n));

    free(atual);
    free(nova);
    return 0;
}
