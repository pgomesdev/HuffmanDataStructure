#include "descompactar.h"
unsigned char is_bit_set_char(unsigned char c, int i)
{
    unsigned char mask = 1 << i;
    return mask & c;
}

unsigned char obter_lixo(FILE *arquivo)
{
    rewind(arquivo);
    unsigned char lixo;

    lixo = fgetc(arquivo);
    lixo = lixo >> 5;
    return lixo;
}

unsigned short obter_tamanho_arvore(FILE *arquivo)
{
    rewind(arquivo);
    unsigned short tamanho_arvore;
    unsigned short auxiliar[2];

    auxiliar[0] = fgetc(arquivo);
    auxiliar[1] = fgetc(arquivo);

    auxiliar[0] = auxiliar[0] << 11;
    auxiliar[0] = auxiliar[0] >> 3;

    tamanho_arvore = auxiliar[0] + auxiliar[1];

    return tamanho_arvore;
}

/*void obter_arvore(unsigned char *array_arvore, FILE *arquivo)
{
    rewind(arquivo);
    int i = 0;
    int tamanho = obter_tamanho_arvore(arquivo);
    for(i = 0; i < tamanho; i++)
    {
        array_arvore[i] = fgetc(arquivo);
    }
}*/

/* CONTAGEM DA QUANTIDADE DE BITS NO TEXTO DO ARQUIVO COMPACTADO */
unsigned long long int contar_tamanho_array_binarios_descompactar(FILE *arquivo, unsigned short tam_cabecalho)
{
    int i;
    unsigned long long int tam = 0;

    rewind(arquivo);
    for(i = 0; i<tam_cabecalho ; i++)
    {
        fgetc(arquivo);
    }

    /* LÊ TODO ARQUIVO, INCLUSIVE O QUE HÁ DE LIXO */
    while(fgetc(arquivo) != EOF)
    {
        tam++;
    }

    /* RETORNA A QUANTIDADE DE BITS DO TEXTO */
    return tam * 8;
}

void escrever_array_compactado(FILE *arquivo, unsigned char *array_binarios_descompactar, unsigned int tam_array_b_descompactar, unsigned short tam_cabecalho)
{
    unsigned char c;
    unsigned long long int i, tam = 0;
    rewind(arquivo);
    for(i = 0; i<tam_cabecalho ; i++)
    {
        fgetc(arquivo);
    }

    /* LÊ TODO ARQUIVO, INCLUSIVE O QUE HÁ DE LIXO */
    while(tam < tam_array_b_descompactar)
    {

        c = fgetc(arquivo);
        for(i = 0; i < TAMANHO_BYTE; i++)
        {
            if(is_bit_set_char(c, 7 - (tam % TAMANHO_BYTE)))
            {
                array_binarios_descompactar[tam] = 1;
            }
            else
            {
                array_binarios_descompactar[tam] = 0;
            }
            tam++;
        }
    }
}

void descompactar_texto(Node *cabeca, unsigned char *array_binarios_descompactar, FILE *novo_arquivo, int tamanho_texto)
{
    int i = 0;
    Node *auxiliar = cabeca;

    while(i < tamanho_texto)
    {
        while(auxiliar->letra == '*')
        {
            if(array_binarios_descompactar[i] == 0)
                auxiliar = auxiliar->filho_esquerda;
            else
                auxiliar = auxiliar->filho_direita;
            i++;
        }

        if(auxiliar->letra == '\\')
            fputc('*', novo_arquivo);
        else
            fputc(auxiliar->letra, novo_arquivo);
        auxiliar = cabeca;
    }
}

Node *criar_arvore_descompactacao(FILE *arquivo, Node *arvore_huffman, unsigned short tam_arvore)
{
    char c;

    Node *newnode = (Node*)malloc(sizeof(Node));

    if(newnode == NULL)
    {
        puts("Erro ao Alocar Memoria para criar_arvore_descompactacao!\n");
        exit(0);
    }

    newnode->letra = 0;
    newnode->num = 0;
    newnode->profundidade = 0;
    newnode->proximo_node = NULL;
    newnode->filho_esquerda = NULL;
    newnode->filho_direita = NULL;
    arvore_huffman = newnode;

    c = fgetc(arquivo);

    if(c == '*')
    {
        arvore_huffman->letra = '*';
        arvore_huffman->filho_esquerda = criar_arvore_descompactacao(arquivo, arvore_huffman->filho_esquerda, tam_arvore);
        arvore_huffman->filho_direita = criar_arvore_descompactacao(arquivo, arvore_huffman->filho_direita, tam_arvore);

        return arvore_huffman;
    }

    else if(c == '\\')
    {
        int i = ftell(arquivo);
        c = fgetc(arquivo);

        if(c == '*')
            arvore_huffman->letra = c;

        else
        {
            arvore_huffman->letra = '\\';
            fseek(arquivo, i, SEEK_SET);
        }

        return arvore_huffman;
    }

    else
    {
        arvore_huffman->letra = c;

        return arvore_huffman;
    }

    return arvore_huffman;
}

