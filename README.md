<div align="center">

# 📡 Poxim Tech: QoS Packet Scheduler

**Sistema de gerenciamento de tráfego de rede com priorização de pacotes para garantia de QoS (Quality of Service).**

</div>

<p align="center">
  <img src="https://img.shields.io/badge/Linguagem-C-blue.svg" alt="Linguagem C">
  <img src="https://img.shields.io/badge/Padrão-C99-blue.svg" alt="Padrão C99">
  <img src="https://img.shields.io/badge/Algoritmo-Stable%20Sort-orange.svg" alt="Algoritmo de Ordenação">
  <img src="https://img.shields.io/badge/Status-Finalizado-brightgreen.svg" alt="Status Finalizado">
</p>

> Este projeto foi desenvolvido para a disciplina de **Projeto e Análise de Algoritmos (PAA)** da **UFS**. Ele simula o firmware de um roteador da empresa **Poxim Tech**, responsável por decidir a ordem de processamento de pacotes TCP/IP baseando-se em níveis de prioridade estritos.

<br>

## 📜 Sumário

* [O Problema](#-o-problema)
* [A Solução Técnica](#-a-solução-técnica)
* [Stack Tecnológica](#-stack-tecnológica)
* [Como Compilar e Rodar](#-como-compilar-e-rodar)
* [Entrada e Saída](#-formato-de-io)

<br>

## 🎯 O Problema

A **Poxim Tech** está implementando um sistema de QoS. Em redes TCP/IP, pacotes chegam constantemente, mas o roteador possui uma **capacidade de processamento limitada** (em bytes) por ciclo.

Quando a capacidade é atingida, o roteador não pode simplesmente processar na ordem de chegada (FIFO), pois isso poderia atrasar tráfego crítico (ex: voz ou vídeo) em favor de tráfego menos importante (ex: download de arquivos em segundo plano).

**Regras de Negócio:**
1.  **Prioridade:** Varia de `0` (mínima) a `63` (máxima).
2.  **Tamanho:** Pacotes têm tamanho variável (até 512 bytes).
3.  **Processamento:** Quando o buffer enche, os pacotes devem ser ordenados pela prioridade. Pacotes com maior prioridade devem ser processados primeiro.

<br>

## 💡 A Solução Técnica

Para garantir que o tráfego crítico tenha preferência, o sistema implementa uma estratégia de ordenação robusta.

### 1. Estrutura do Pacote
Cada pacote é armazenado em uma `struct` contendo:
* `id` (ordem original de chegada - para garantir estabilidade).
* `prioridade` (0-63).
* `tamanho` (bytes).
* `conteudo` (payload em hex/bytes).

### 2. Algoritmo de Priorização (Ordenação)
Utilizamos um algoritmo de ordenação **Instável**(HeapSort), com complexidade de tempo eficiente.

A lógica de comparação segue a regra:
1.  **Maior Prioridade:** Pacotes com nível `63` vêm antes de pacotes nível `0`.
2.  **Estabilidade (Critério de Desempate):** Se dois pacotes têm a mesma prioridade, processa-se o que chegou primeiro (FIFO), garantindo a integridade da sequência de dados.

<br>

## 🛠️ Stack Tecnológica

* **Linguagem:** `C` (ANSI/C99)
* **Compilação:** `GCC`
* **Automação:** `Makefile` (Compatível com Windows e Linux)

<br>

## 🚀 Como Compilar e Rodar

### Comandos do Makefile

No terminal, execute:

| Comando | Descrição |
| :--- | :--- |
| `make` | Compila o projeto e gera o executável na pasta `build/`. |
| `make test` | Roda os casos de teste da pasta `tests/`. |
| `make clean` | Remove executáveis e arquivos temporários. |

<br>

## 📄 Formato de I/O

O programa lê da entrada padrão (`stdin`) seguindo a especificação da Poxim Tech:

### Entrada
1.  **Linha 1:** `N` (número de pacotes) e `C` (capacidade máxima de bytes do roteador).
2.  **Linhas Seguintes:** Cada linha descreve um pacote no formato:
    * `Prioridade` `Tamanho` `Byte1` `Byte2` ... `ByteM`

**Exemplo:**
```text
6 8
0 3 01 02 03
2 4 06 07 08 09
63 2 0F 10
...
