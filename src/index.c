#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define BUFFER_SIZE 8192

/*
    @brief: cria structs
*/
typedef struct {
    uint32_t id_chegada;
    uint32_t prioridade_pacote;
    uint32_t tamanho_pacote;
    char buffer_dados[512][3];
}info_pacote;

typedef struct {
    uint32_t numero_pacotes;
    uint32_t max_bytes;
}info_roteador;

/*
    @brief: Função para abrir arquivos
*/
void open_files(int argc, char **argv, FILE **ptr_input, FILE **ptr_output){
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return;
    }
    
    *ptr_input = fopen(argv[1], "r");
    if (!(*ptr_input))
    {
        perror("Erro ao abrir arquivo de entrada\n");
        exit(1);
    }

    *ptr_output = fopen(argv[2], "w");
    if (!(*ptr_output))
    {
        perror("Erro ao abrir arquivo de saída\n");
        fclose(*ptr_input);
        exit(1);
    }
}

/*
    @brief: Função para carregar memória
*/
void carregar_memoria(info_pacote ***pacotes, char *linha, FILE **input, FILE **output, info_roteador *roteador){
    if(!fgets(linha, BUFFER_SIZE, *input))
    {
        perror("Erro ao ler a primeira linha do arquivo de entrada\n");
        fclose(*input);
        fclose(*output);
        exit(1);
    };

    if (sscanf(linha, "%u %u", &roteador->numero_pacotes, &roteador->max_bytes) != 2) 
    {
        perror("Erro ao ler numero de pacotes e quantidade de bytes\n");
        fclose(*input);
        fclose(*output);
        exit(1);
    };

    *pacotes = malloc(sizeof(info_pacote*) * roteador->numero_pacotes);
    uint32_t i = 0;

    while (i < roteador->numero_pacotes && fgets(linha, BUFFER_SIZE, *input) != NULL)
    {
        (*pacotes)[i] = (info_pacote *)malloc(sizeof(info_pacote));
        (*pacotes)[i]->prioridade_pacote = 0;
        (*pacotes)[i]->tamanho_pacote = 0;
        (*pacotes)[i]->id_chegada = i;
        char *ptr = linha;
        int offset = 0;
        
        // %n armazena o numero de caracteres lidos ate o momento
        sscanf(linha, "%u %u%n", &(*pacotes)[i]->prioridade_pacote, &(*pacotes)[i]->tamanho_pacote, &offset); 
        ptr += offset;

        for (uint32_t j = 0; j < (*pacotes)[i]->tamanho_pacote; j++)
        {
            if (sscanf(ptr, "%s%n", (*pacotes)[i]->buffer_dados[j], &offset) == 1) 
            {
                ptr += offset; 
            }
        }

        linha[strcspn(linha, "\n")] = '\0';
        i++;
    }
}

/*
    @brief: Trocar elementos
*/
void swap(info_pacote **a, info_pacote **b)
{
    info_pacote *aux = *a;
    *a = *b;
    *b = aux;
}

/*
    @brief: Diz ordem dos elementos
*/
int eh_maior(info_pacote *a, info_pacote *b) {
    if (a->prioridade_pacote >= b->prioridade_pacote) return 0;
    
    if (a->prioridade_pacote < b->prioridade_pacote) {
        return 1;
        // Poderiamos implementar dessa forma e tornar o heapsort estavel
        // if (a->id_chegada < b->id_chegada)
            // return 0
    }
    
    return -1;
}

/*
    @brief: Heapify
*/
void heapify(info_pacote **arr, uint32_t n, uint32_t i){
    uint32_t maior = i;
    uint32_t l = 2 * i + 1;
    uint32_t r = 2 * i + 2;

    if (l < n && eh_maior(arr[l], arr[maior]))   
        maior = l;

    if (r < n && eh_maior(arr[r], arr[maior]))
        maior = r;

    if (maior != i)
    {
        swap(&arr[i], &arr[maior]);
        heapify(arr, n, maior);
    }
};

/*
    @brief: construindo heap
*/
void heapSort(info_pacote **arr, uint32_t n){
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    for (int i = n - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);

        heapify(arr, i, 0);
    }
}

/*
    @brief: Função para processar pacotes
*/
void processar_pacotes(info_pacote **pacote, info_roteador *roteador, int *index, FILE *output){
    uint32_t acc = 0;
    uint32_t i = *index, index_anterior = *index;
    while (i < roteador->numero_pacotes)
    {
        if (acc + pacote[i]->tamanho_pacote <= roteador->max_bytes)
        {
            acc += pacote[i]->tamanho_pacote;
            i++;
            *index = i;
        }
        else if (acc == 0) 
        {
            i++; 
        }
        else {
            break;
        }
    }

    uint32_t processados = i - index_anterior;
    if (processados > 0)
    {
        heapSort(&pacote[index_anterior], processados);

        fprintf(output, "|");
        for(int k = 0; k < processados; k++)
        {
            info_pacote *p = pacote[index_anterior + k];

            for (int b = 0; b < p->tamanho_pacote; b++)
            {
                fprintf(output, "%s", p->buffer_dados[b]);
                if (b < p->tamanho_pacote - 1)
                    fprintf(output, ",");
            }
            fprintf(output, "|");
        }
        fprintf(output, "\n");
    }
    *index = i;
}

int main(int argc, char **argv){
    FILE *input, *output;
    char linha[BUFFER_SIZE] = "";
    info_roteador roteador;
    info_pacote **pacotes;
    int index = 0;
    
    // ---> Abrindo arquivos e carregando memoria <---
    open_files(argc, argv, &input, &output);
    carregar_memoria(&pacotes, linha, &input, &output, &roteador);

    // ---> Processando pacotes <---
    while (index < roteador.numero_pacotes){
        processar_pacotes(pacotes, &roteador, &index, output);
    }

    // ---> Liberando memoria <---
    for(uint32_t k = 0; k < roteador.numero_pacotes; k++) 
    {
        free(pacotes[k]);
    }
    free(pacotes);
    fclose(input);
    fclose(output);
    return 0;
}