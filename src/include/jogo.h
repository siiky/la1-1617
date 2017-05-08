#ifndef _JOGO_H
#define _JOGO_H

#include "posicao.h"
#include "estado.h"

#define quantas_jogadas(J) (*(((uchar *) (J)) - 1))

/*
 * 8 for action type
 * 4 for player position (x/y)
 * 4 for target position (x/y)
 * 4 for comma (,)
 * 1 for '\0'
 */
#define JOGADA_LINK_MAX_BUFFER (8 + 4 + 4 + 4 + 1)

enum accao {
	ACCAO_RESET,
	ACCAO_MOVE,
	ACCAO_INVALID,
};

typedef struct {
	posicao_s dest;
	char link[JOGADA_LINK_MAX_BUFFER];
} jogada_s, *jogada_p;

jogada_p jogadas_possiveis (const estado_p e);
estado_s ler_estado (char * args);
void escreve_estado (const estado_p e);

#endif /* _JOGO_H */