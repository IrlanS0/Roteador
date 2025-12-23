#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#define BUFFER_SIZE 2048
static FILE *log;

void init_log(void){
    log = fopen("log.txt", "w");
    if (!log) {
        perror("Erro ao abrir o arquivo de log");
        exit(1);
    }
}

typedef struct {
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
    if (!*(ptr_input))
    {
        perror("Erro ao abrir arquivo de entrada\n");
        exit(1);
    }

    *ptr_output = fopen(argv[2], "w");
    if (!*(ptr_output))
    {
        perror("Erro ao abrir arquivo de saída\n");
        fclose(*ptr_input);
        exit(1);
    }
    init_log();
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
    uint16_t i = 0;

    fprintf(log, "---> CARREGANDO MEMORIA COM PACOTES <---\n");
    while (fgets(linha, BUFFER_SIZE, *input) != NULL)
    {
        (*pacotes)[i] = (info_pacote *)malloc(sizeof(info_pacote));
        fprintf(log, "linha lida:%s", linha);
        char *ptr = linha;
        int offset = 0;

        // %n armazena o numero de caracteres lidos ate o momento
        sscanf(linha, "%u %u%n", &(*pacotes)[i]->prioridade_pacote, &(*pacotes)[i]->tamanho_pacote, &offset); 
        
        ptr += offset;
        fprintf(log, "Lendo Pacote %d: Prio=%u, Tam=%u\n", 
                i, (*pacotes)[i]->prioridade_pacote, (*pacotes)[i]->tamanho_pacote);

        for (uint32_t j = 0; j < (*pacotes)[i]->tamanho_pacote; j++)
        {
            if (sscanf(ptr, "%s%n", (*pacotes)[i]->buffer_dados[j], &offset) == 1) 
            {
                ptr += offset; 
                fprintf(log, "Byte lido: %s\n", (*pacotes)[i]->buffer_dados[j]);
            }
        }
        fprintf(log, "\n");

        linha[strcspn(linha, "\n")] = '\0';
        i++;
    }
    fprintf(log, "---> PACOTES CARREGADOS <---\n\n");
}

int main(int argc, char **argv){
    FILE *input, *output;
    char linha[BUFFER_SIZE];
    info_roteador roteador;
    info_pacote **pacotes;

    // Abrindo arquivos e carregando memoria
    open_files(argc, argv, &input, &output);
    carregar_memoria(&pacotes, linha, &input, &output, &roteador);

    // Liberando memoria
    for(uint8_t k = 0; k < roteador.numero_pacotes; k++) 
    {
        free(pacotes[k]);
    }
    free(pacotes);
    fclose(input);
    fclose(output);
    return 0;
}