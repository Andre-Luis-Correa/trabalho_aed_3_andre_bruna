//
// Created by andre on 29/04/2024.
//

#include "arvore_utils.h"
#include "../cabecalho/cabecalho_indices.h"
#include <stdio.h>
#include <stdlib.h>

//verifica se um n�(lido) � nulo(arvore vazia)
//pre-condi�ao: n� lido
//pos-condi�ao: 0 se nao for vazia, !=0 se vazia
int vazia(ARVOREB * r) {
    return (r == NULL);
}

//verifica se o no e folha
//pr�-condi�ao: no nao nulo
//pos-conid�ao: 1 = sim, 0 = nao
int eh_folha(ARVOREB * r) {
    return r->filho[0] == -1;
}

//testa se � raiz
//pre-condi��o: r nao pode ser NULL
//pos-condi��o: 1 se for raiz, 0 se nao for
//int eh_raiz(ARQUIVOS files, ARVOREB * r) {
//    if (r != NULL) {
//        int pos_pai = buscar_pai(files.file_indices, int pos_raiz, int codigo, int * pos_filho_remocao);
//
//        if ( buscar_pai(ARQUIVOS files, int pos_raiz, int codigo, int * pos_filho_remocao) == -1) {
//            return 1;
//        }
//    }
//    return 0;
//}

//verifica se deu underflow
//pre-condi�ao: arvore nao nula
//pos-conidi��o: 1 se deu, 0 se nao deu, underflow
//int underflow(ARVOREB * r) {
//    if (r != NULL) {
//        if (r->num_chaves < MIN && !eh_raiz(r) )
//            return 1;
//        return 0;
//    }
//    return 0;
//}

int buscar_pos_chave(ARVOREB * r, int codigo){
    int pos_codigo;

    // Encontra a pos dentro do n�
    for(pos_codigo = 0; pos_codigo < r->num_chaves; pos_codigo++){
        if( r->chave[pos_codigo] == codigo)
            return pos_codigo;
    }

    return -1;
}

// Fun��o para verificar se j� existe um determinado c�digo na �rvore e retorna a posi��o do n� onde ele est� presente
int buscar_no(FILE * file_indices, int codigo){
    CABECALHO_INDICES * cab = le_cabecalho_indices(file_indices);
    int pos_raiz = cab->pos_raiz;
    free(cab);

    if(pos_raiz == -1) return -1;

    return buscar_no_aux(file_indices, codigo, pos_raiz);
}

int buscar_no_aux(FILE * file_indices, int codigo, int pos){
    if(pos == -1) return -1;

    ARVOREB * r = ler_no(file_indices, pos);
    int i;

    for(i = 0; i < r->num_chaves; i++){
        if(r->chave[i] == codigo ){
            free(r);
            return pos;
        } else if (r->chave[i] > codigo ){
            int pos_atual = r->filho[i];
            free(r);
            return buscar_no_aux(file_indices, codigo, pos_atual);
        }
    }

    int pos_atual = r->filho[i];
    free(r);

    return buscar_no_aux(file_indices, codigo, pos_atual);
}

int buscar_pai(FILE * file_indices, int codigo){
    CABECALHO_INDICES * cab_indices = le_cabecalho_indices(file_indices);
    int pos_raiz = cab_indices->pos_raiz;
    free(cab_indices);

    ARVOREB * r = ler_no(file_indices, pos_raiz);

    if (eh_folha(r) || buscar_pos_chave(r, codigo) != -1) { // A propria raiz � o pai, pois nao tem pai acima
        free(r);
        return -1; // Retorna -1 indicando que n�o foi encontrado
    }

    free(r);
    return buscar_pai_aux(file_indices, pos_raiz, codigo);
}

int buscar_pai_aux(FILE * file_indices, int pos_raiz, int codigo){
    ARVOREB * r = ler_no(file_indices, pos_raiz);

    int i;
    for (i = 0; i < r->num_chaves; i++) {
        // Se o c�digo for menor que a chave atual, desce para o filho � esquerda
        if (codigo < r->chave[i]) {
                ARVOREB * filho = ler_no(file_indices, r->filho[i]);
                int pos_codigo = buscar_pos_chave(filho, codigo);
                free(filho);
                // Se a chave for encontrada no filho, retorna o n� atual como pai
                if (pos_codigo != -1) {
                    return pos_raiz;
                }
                // Se n�o, continua a busca descendente
                return buscar_pai_aux(file_indices, r->filho[i], codigo);
            }
        }

    // Se o c�digo for maior que todas as chaves, desce para o �ltimo filho
    ARVOREB * filho = ler_no(file_indices, r->filho[i]);
    int pos_codigo = buscar_pos_chave(filho, codigo);
    free(filho);

    // Se a chave for encontrada no �ltimo filho, retorna o n� atual como pai
    if (pos_codigo != -1) {
        return pos_raiz;
    }

    // Se n�o, continua a busca descendente
    return buscar_pai_aux(file_indices, r->filho[i], codigo);
}

//acha a posi�ao da chave divisa entre irmao esq e irmao dir.
//pre-condi��o: os 2 irm�os devem ser filhos do memsmo pai.
//pos-condi��o: devolve o indice da chave que � divisa no pai entre os n�s, ou -1 caso de erro.
int buscar_pos_chave_separadora(FILE* file_indices, ARVOREB * esq, ARVOREB * dir) {//irmao esq e irm�o dir
    int pos_pai, pos_esq, pos_dir;

    pos_pai = buscar_pai(file_indices, esq->chave[0]);
    pos_esq = buscar_no(file_indices, esq->chave[0]);
    pos_dir = buscar_no(file_indices, dir->chave[0]);
    ARVOREB * pai = ler_no(file_indices, pos_pai);

    int pos = -1;
    if (!vazia(pai)) {
        if (pai->filho[0] == pos_esq) { //se o filho esq for igual ao filho[0] no pai, a posicao da chave separadora � 0;
            free(pai);
            return 0;
        } else if (pai->filho[pai->num_chaves] == pos_dir) {
            pos = pai->num_chaves - 1;
            free(pai);
            return pos;
        }
        int i = 0;
        for (i = 0; i < pai->num_chaves; i++)
        {
            if (pos_esq == pai->filho[i] && pos_dir == pai->filho[i + 1])
            {
                free(pai);
                return i;
            }
        }
        return -1;
    }
    return -1;
}