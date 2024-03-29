/** @file */
#include "check.h"

#include <string.h>

#include "posicao.h"
#include "estado.h"

#include "jogo.h"

/*
 * Tipos de movimento:
 * [X] Rei do Xadrez
 * [X] Cavalo do Xadrez
 * [ ] Peao do Xadrez
 * [ ] Torre do Xadrez
 * [ ] Bispo do Xadrez
 * [ ] Rainha do Xadrez
 * [ ] Damas
 */

/**
 * @brief Verifica se uma jogada e válida.
 * @param e Estado actual do jogo.
 * @param p Posicao a testar.
 * @returns Verdadeiro se for valida, falso caso contrario.
 */
bool jogada_valida (const estado_p e, const posicao_p p)
{
	assert(e != NULL);
	assert(p != NULL);
	return posicao_valida(*p)
	    && !(pos_inimigos(e->obstaculo, *p, e->num_obstaculos));
}

/**
 * @brief Calcula posicoes possiveis para o tipo de movimento rei de xadrez.
 * @param dst Array de destino.
 * @param o Posicao de origem.
 * @returns Numero de posicoes escritas.
 */
uchar pospos_xadrez_rei (posicao_p dst, const posicao_p o)
{
	/*
	 *    1 0 1
	 * 1 |X|X|X|
	 *   -------
	 * 0 |X|J|X|
	 *   -------
	 * 1 |X|X|X|
	 */
	assert(dst != NULL);
	assert(o != NULL);

	uchar ret = 0;
	for (abcissa x = o->x; x < o->x + 3; x++) {
		for (ordenada y = o->y; y < o->y + 3; y++) {
			posicao_s p = posicao_new(x - 1, y - 1);
			if (!posicao_igual(*o, p))
				dst[ret++] = p;
		}
	}

	return ret;
}

/**
 * @def F(X, Y)
 * @brief Calcula uma nova posicao e guarda-a no array de destino.
 * @param X Abcissa da nova posicao.
 * @param Y Ordenada da nova posicao.
 * @returns A nova posicao.
 */

/**
 * @brief Calcula posicoes possiveis para o tipo de movimento cavalo de xadrez.
 * @param dst Array de destino.
 * @param o Posicao de origem.
 * @returns Numero de posicoes escritas.
 */
uchar pospos_xadrez_cavalo (posicao_p dst, const posicao_p o)
{
	/*
	 *    2 1 0 1 2
	 * 2 | |X| |X| |
	 *   -----------
	 * 1 |X| | | |X|
	 *   -----------
	 * 0 | | |J| | |
	 *   -----------
	 * 1 |X| | | |X|
	 *   -----------
	 * 2 | |X| |X| |
	 */
	assert(dst != NULL);
	assert(o != NULL);

	uchar ret = 0;

#define F(X, Y) dst[ret++] = posicao_new((X), (Y))
	F(o->x - 2, o->y - 1);
	F(o->x - 2, o->y + 1);
	F(o->x - 1, o->y - 2);
	F(o->x - 1, o->y + 2);
	F(o->x + 1, o->y - 2);
	F(o->x + 1, o->y + 2);
	F(o->x + 2, o->y - 1);
	F(o->x + 2, o->y + 1);
	return ret;

#undef F
}

/**
 * @brief Tipo de funcoes que calculam as posicoes possiveis para um tipo de movimento.
 */
typedef uchar (* pospos_handler) (posicao_p dst, const posicao_p o);

/**
 * @brief Devolve um array de apontadores para funcoes que calculam posicoes possiveis.
 * @returns Array de apontadores de funcoes.
 */
const pospos_handler * pospos_handlers (void)
{
	static const pospos_handler ret[MOV_TYPE_QUANTOS] = {
		[MOV_TYPE_XADREZ_REI]    = pospos_xadrez_rei,
		[MOV_TYPE_XADREZ_CAVALO] = pospos_xadrez_cavalo,
	};
	return ret;
}

/**
 * @brief Filtra um conjunto de posicoes dado uma fucao.
 * @param e O estado actual.
 * @param p O array de posicoes.
 * @param num O numero de posicoes.
 * @param f O filtro.
 * @returns Numero de posicoes que satisfizeram o filtro.
 */
uchar pospos_filter (const estado_p e, posicao_p p, size_t num, bool (* f) (const estado_p, const posicao_p))
{
	assert(e != NULL);
	assert(p != NULL);
	assert(num > 0);
	assert(f != NULL);

	uchar ret = 0;

	for (size_t r = 0; r < num; r++) {
		if (f(e, p + r)) {
			p[ret] = p[r];
			ret++;
		}
	}

	return ret;
}

/**
 * @def SIZE
 * @brief Tamanho maximo, em bytes, das posicoes possiveis.
 */

/**
 * @brief Calcula um conjunto de posicoes possiveis.
 * @param e O estado actual.
 * @param o A posicao de origem.
 * @returns As posicoes possiveis.
 */
posicao_p posicoes_possiveis (const estado_p e, posicao_s o)
{
	assert(e != NULL);
	assert(e->mov_type < MOV_TYPE_QUANTOS);
	assert(posicao_valida(o));

#define SIZE (1 + (sizeof(posicao_s) * NJOGADAS))
	static uchar arr[SIZE] = "";
	memset(arr, 0, SIZE);

	const pospos_handler * handlers = pospos_handlers();
	assert(handlers != NULL);

	posicao_p ret = (posicao_p) (arr + 1);

	/* calcula as posicoes possiveis pra certo mov_type */
	uchar w = handlers[e->mov_type](ret, &o);

	/* filtra as posicoes validas (i.e., dentro do mapa) */
	w = pos_filter(ret, w, posicao_valida);

	/* filtra as posicoes que podem ser jogadas */
	w = pospos_filter(e, ret, w, jogada_valida);

	quantas_jogadas(ret) = w;

	return ret;
#undef SIZE
}

accao_s accao_new (const char * nome, enum accao accao, posicao_s jog, posicao_s dest)
{
	assert(nome != NULL);
	assert(strlen(nome) < JOGADA_LINK_MAX_BUFFER);
	assert(accao < ACCAO_INVALID);

	accao_s ret = (accao_s) {
		.accao = accao,
		.jog = jog,
		.dest = dest,
	};
	strcpy(ret.nome, nome);

	return ret;
}

char * accao2str (accao_s accao)
{
	assert(accao.nome != NULL);
	assert(accao.accao < ACCAO_INVALID);

	static char ret[JOGADA_LINK_MAX_BUFFER] = "";
	memset(ret, 0, JOGADA_LINK_MAX_BUFFER);
	sprintf(ret,
		"%s,"
		"%08x,"
		"%02hhx,"
		"%02hhx,"
		"%02hhx,"
		"%02hhx",
		accao.nome,
		accao.accao,
		accao.jog.x,
		accao.jog.y,
		accao.dest.x,
		accao.dest.y
	       );
	return ret;
}

accao_s str2accao (const char * str)
{
	assert(str != NULL);
	accao_s ret = { 0 };
#ifdef NDEBUG
	sscanf(str,
#else
	int r = sscanf(str,
#endif
	       "%[^,],"
	       "%08x,"
	       "%02hhx,"
	       "%02hhx,"
	       "%02hhx,"
	       "%02hhx",
	       ret.nome,
	       &ret.accao,
	       &ret.jog.x,
	       &ret.jog.y,
	       &ret.dest.x,
	       &ret.dest.y
	      );
	assert(r == 6);
	return ret;
}

/**
 * @def SIZE
 * @brief Tamanho maximo, em bytes, das jogadas possiveis.
 */

jogada_p jogadas_possiveis (const estado_p e)
{
	assert(e != NULL);
	assert(e->nome != NULL);
	assert(e->mov_type < MOV_TYPE_QUANTOS);

	/*
	 * [0] uchar => quantas jogadas possiveis existem
	 * [1] jogada_s
	 * ..
	 * [N] jogada_s
	 */
#define SIZE (1 + (sizeof(jogada_s) * NJOGADAS))
	static uchar arr[SIZE] = "";
	memset(arr, 0, SIZE);

	posicao_p pos = posicoes_possiveis(e, e->jog.pos);
	assert(pos != NULL);

	jogada_p ret = (jogada_p) (arr + 1);
	uchar w = quantas_jogadas(ret) = quantas_jogadas(pos);

	for (size_t i = 0; i < w; i++) {
		char * link = accao2str(accao_new(e->nome, ACCAO_MOVE, e->jog.pos, pos[i]));
		assert(link != NULL);
		strcpy(ret[i].link, link);
		ret[i].dest = pos[i];
	}

	return ret;
#undef SIZE
}

/**
 * @brief Calcula o novo estado para o tipo de accao ACCAO_RESET.
 * @param e O estado de jogo actual.
 * @param accao A accao.
 * @returns O novo estado.
 */
estado_s accao_reset_handler (estado_s e, accao_s accao)
{
	UNUSED(accao);
	assert(e.nome != NULL);
	assert(accao.accao == ACCAO_RESET);
	return init_estado(0, 0, MOV_TYPE_QUANTOS, e.nome);
}

/**
 * @brief Calcula o novo estado para o tipo de accao ACCAO_MOVE.
 * @param ret O estado de jogo actual.
 * @param accao A accao.
 * @returns O novo estado.
 */
estado_s accao_move_handler (estado_s ret, accao_s accao)
{
	assert(ret.nome != NULL);
	assert(accao.accao == ACCAO_MOVE);

	/*
	 * se a posicao da accao nao for igual a do jogador
	 * ou a posicao do jogador nao for valida
	 * ou a posicao de destino nao for valida
	 * nao faz nada
	 */
	ifjmp(!posicao_igual(accao.jog, ret.jog.pos), out);
	ifjmp(!posicao_valida(accao.dest), out);

	size_t i = pos_inimigos_ind(ret.inimigo, accao.dest, ret.num_inimigos);

	ret = (i < ret.num_inimigos) ? /* se tiver inimigo */
		ataca_inimigo(ret, i) :
		move_jogador(ret, accao.dest);

	if (ret.matou)
		ret = move_jogador(ret, accao.dest);

	if (fim_de_ronda(&ret) && posicao_igual(ret.jog.pos, ret.porta))
		ret = init_estado(ret.nivel, (ret.score + (ret.jog.vida / 5)), ret.mov_type, ret.nome);

out:
	return ret;
}

enum mov_type mov_type_next (enum mov_type ret)
{
	assert(ret < MOV_TYPE_QUANTOS);
	return (ret + 1) % MOV_TYPE_QUANTOS;
}

/**
 * @brief Calcula o novo estado para o tipo de accao ACCAO_CHANGE_MT.
 * @param ret O estado de jogo actual.
 * @param accao A accao.
 * @returns O novo estado.
 */
estado_s accao_change_mt_handler (estado_s ret, accao_s accao)
{
	assert(ret.nome != NULL);
	assert(accao.accao == ACCAO_CHANGE_MT);
	assert(accao.dest.x < MOV_TYPE_QUANTOS);

	ifjmp(!posicao_igual(ret.jog.pos, accao.jog), out);

	ret.mov_type = accao.dest.x;
	if (!fim_de_ronda(&ret))
		ret.jog.vida--;
out:
	return ret;
}

/**
 * @brief Calcula o novo estado para o tipo de accao ACCAO_IGNORE.
 * @param ret O estado de jogo actual.
 * @param accao A accao.
 * @returns O novo estado.
 */
estado_s accao_ignore_handler (estado_s ret, accao_s accao)
{
	UNUSED(accao);
	assert(ret.nome != NULL);
	assert(accao.accao == ACCAO_IGNORE);
	return ret;
}

/**
 * @brief Tipo de funcoes que calculam um novo estado para um tipo de accao.
 */
typedef estado_s (* accao_handler) (estado_s ret, accao_s accao);

/**
 * @brief Devolve um array de apontadores para funcoes que calculam o novo estado para uma accao.
 * @returns Array de apontadores de funcoes.
 */
const accao_handler * accao_handlers (void)
{
	static const accao_handler ret[ACCAO_INVALID] = {
		[ACCAO_RESET]     = accao_reset_handler,
		[ACCAO_MOVE]      = accao_move_handler,
		[ACCAO_CHANGE_MT] = accao_change_mt_handler,
		[ACCAO_IGNORE]    = accao_ignore_handler,
	};
	return ret;
}

/**
 * @brief Calcula o novo estado de acordo com a accao recebida.
 * @param ret O estado actual.
 * @param accao A accao.
 * @returns O novo estado.
 */
estado_s corre_accao (estado_s ret, accao_s accao)
{
	ifjmp(accao.accao >= ACCAO_INVALID, out);

	const accao_handler * handlers = accao_handlers();
	ret = handlers[accao.accao](ret, accao);
out:
	return ret;
}

/**
 * @brief Actualiza o estado depois de jogar com o bot de indice I.
 * @param ret O estado actual.
 * @param I O indice do bot a jogar.
 * @returns O novo estado.
 */
estado_s bot_joga_aux (estado_s ret, size_t I)
{
	posicao_p posicoes = posicoes_possiveis(&ret, ret.inimigo[I].pos);
	assert(posicoes != NULL);
	/* se nao houverem posicoes possiveis, nao faz nada */
	ifjmp(quantas_jogadas(posicoes) == 0, out);

	quantas_jogadas(posicoes) = pospos_filter(&ret, posicoes, quantas_jogadas(posicoes), nao_tem_inimigos);
	/* se nao houverem posicoes possiveis, nao faz nada */
	ifjmp(quantas_jogadas(posicoes) == 0, out);

	size_t mp = pos_mais_perto(posicoes, quantas_jogadas(posicoes), ret.jog.pos);

	ifjmp(mp >= quantas_jogadas(posicoes), out);

	posicao_s p = posicoes[mp];

	/*
	 * se a posicao mais prox do jog for igual a do jog
	 * ataca o jog, senao move
	 */
	if (posicao_igual(ret.jog.pos, p))
		ret = ataca_jogador(&ret, I);
	else
		ret.inimigo[I].pos = p;

out:
	return ret;
}

/**
 * @brief Calcula o novo estado depois de todos os bots jogarem.
 * @param ret O estado actual.
 * @returns O novo estado.
 */
estado_s bot_joga (estado_s ret)
{
	assert(ret.nome != NULL);

	for (size_t i = 0; i < ret.num_inimigos && !fim_de_jogo(&ret); i++)
		ret = bot_joga_aux(ret, i);

	return ret;
}

char * pathname (const char * name)
{
	static char ret[11 + 20] = "";
	sprintf(ret, BASE_PATH "%s", name);
	return ret;
}

estado_s ler_estado (accao_s accao)
{
	assert(accao.nome != NULL);
	assert(accao.accao < ACCAO_INVALID);

	estado_s ret = { 0 };

	char * path = pathname(accao.nome);
	assert(path != NULL);

	FILE * f = fopen(path, "rb");

	check(f == NULL, "could not open state file to read");

	check(fread(&ret, sizeof(estado_s), 1, f) != 1,
	      "could not read from state file");

	fclose(f);

	if (fim_de_jogo(&ret)) {
		ret = init_estado(0, 0, MOV_TYPE_QUANTOS, ret.nome);
	} else {
		ret = corre_accao(ret, accao);
		ret = bot_joga(ret);
	}

	return ret;
}

void escreve_estado (const estado_p e)
{
	assert(e != NULL);
	assert(e->nome != NULL);

	char * path = pathname(e->nome);
	assert(path != NULL);

	FILE * f = fopen(path, "wb");

	check(f == NULL, "could not open state file to write");

	check(fwrite(e, sizeof(estado_s), 1, f) != 1,
	      "could not write to state file");

	fclose(f);
}

void update_highscore (const estado_p e, struct highscore hs[3])
{
	assert(e != NULL);
	assert(e->nome != NULL);
	assert(fim_de_jogo(e));
	assert(hs != NULL);

	size_t i = 0;
	for (i = 0; i < 3 && e->score < hs[i].score; i++);

	if (i < 3) {
		hs[i].score = e->score;
		strcpy(hs[i].nome, e->nome);
	}
}

void escreve_highscore (struct highscore hs[3])
{
	assert(hs != NULL);
	FILE * f = fopen(SCOREFILE_PATH, "wb");

	check(f == NULL,
	      "could not open highscore file to write");

	check(fwrite(hs, sizeof(struct highscore), 3, f) != 3,
	      "could not write to highscore file");

	fclose(f);
}

struct highscore * ler_highscore (void)
{
	static struct highscore ret[3] = {0};
	FILE * f = fopen(SCOREFILE_PATH, "rb");

	check(f == NULL,
	      "could not open highscore file to read");

	memset(ret, 0, 3 * sizeof(struct highscore));
	int n = fread(ret, sizeof(struct highscore), 3, f);
	UNUSED(n);

	ifnnull(f, fclose);

	return ret;
}
