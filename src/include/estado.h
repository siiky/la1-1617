#ifndef _ESTADO_H
#define _ESTADO_H

#include "posicao.h"
#include "entidades.h"


#define MIN_INIMIGOS	(TAM)
#define MIN_OBSTACULOS	(MIN_INIMIGOS << 1)

#define MAX_OBSTACULOS	((TAM * TAM) >> 1)
#define MAX_INIMIGOS	(MAX_OBSTACULOS >> 1)

typedef struct {
	uchar nivel;
	uchar num_inimigos;
	uchar num_obstaculos;
	entidade jog;
	posicao_s porta;
	entidade inimigo[MAX_INIMIGOS];
	entidade obstaculo[MAX_OBSTACULOS];
} estado_s, *estado_p;

#define MAX_BUFFER	((sizeof(estado_s) << 1) + 1)

char * estado2str (const estado_p e);
estado_s str2estado (char * args);
estado_s ler_estado (char * args);

bool fim_de_ronda (const estado_p e);
estado_s init_estado (uchar nivel);
estado_s move_jogador (estado_s e, posicao_s p);
estado_s ataca(const estado_p e, const entidades i, uchar I);

#endif /* _ESTADO_H */
