#include <stdlib.h>

#include "check.h"
#include "posicao.h"
#include "estado.h"
#include "jogo.h"

#include "html.h"

void imprime_entidades (const entidades p, size_t max, char * img)
{
	size_t i = 0;

	check(p != NULL);
	check(img != NULL);

	for (i = 0; i < max; i++)
		IMAGE(p[i].pos.x, p[i].pos.y, ESCALA, img);
}

void imprime_inimigos (const estado_p e)
{
	check(e != NULL);
	imprime_entidades(e->inimigo, e->num_inimigos, IMG_INIMIGO);
}

void imprime_jogada (const jogada_p j)
{
	check(j != NULL);
	check(j->link != NULL);

	GAME_LINK(j->link);
	RECT_TRANSPARENTE(j->dest.y, j->dest.x, ESCALA);
	FECHA_A;
}

void imprime_jogadas (const estado_p e)
{
	uchar N = 0;
	uchar i = 0;
	jogada_p j = NULL;

	check(e != NULL);

	j = jogadas_possiveis(e),
	  check(j != NULL);

	/* imprimir o jogador */
	imprime_entidades(&e->jog, 1, IMG_JOGADOR);

	/* imprimir as jogadas */
	N = j->dest.x;
	for (i = 0; i < N; i++)
		imprime_jogada(j + i);
}

void imprime_obstaculos (const estado_p e)
{
	check(e != NULL);
	imprime_entidades(e->obstaculo, e->num_obstaculos, IMG_OBSTACULO);
}

char * random_color (void)
{
	/*
	 * cada cor tem 3 bytes,
	 * logo existem 2 ^ (3 * 8)
	 * cores diferentes
	 */
#define NUM_CORES	(1 << (3 * 8))
	unsigned int rgb = rand() % NUM_CORES;
#undef NUM_CORES

	/* "#rrggbb0" */
	static char ret[8] = "#";
	sprintf(ret+1, "%06x", rgb);
	ret[7] = '\0';

	return ret;
}

void imprime_tabuleiro (abcissa L, ordenada C)
{
	size_t l = 0;
	size_t c = 0;
	for (l = 0; l < L; l++) {
		for (c = 0; c < C; c++)
			IMPRIME_CASA(l, c);
		putchar('\n');
	}
}

void imprime_porta (const estado_p e)
{
	check(e != NULL);
	IMAGE(e->porta.x, e->porta.y, ESCALA, IMG_PORTA);
}

void imprime_jogo (const estado_p e)
{
	check(e != NULL);

	ABRE_SVG(SVG_WIDTH, SVG_HEIGHT); {
		COMMENT("tabuleiro");
		imprime_tabuleiro(TAM, TAM);

		COMMENT("porta");
		imprime_porta(e);

		COMMENT("obstaculos");
		imprime_obstaculos(e);

		COMMENT("inimigos");
		imprime_inimigos(e);

		COMMENT("jogadas");
		imprime_jogadas(e);
	} FECHA_SVG;
}
