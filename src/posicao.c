#include "check.h"

#include <stdio.h>
#include <math.h>

#include "posicao.h"


bool posicao_valida (posicao_s p)
{
	/*
	 * Como `abcissa` e `ordenada` sao inteiros sem
	 * sinal, nao e preciso comparar com 0
	 */
	return p.x < TAM && p.y < TAM;
}

bool posicao_igual (posicao_s p1, posicao_s p2)
{
	return p1.x == p2.x && p1.y == p2.y;
}

bool pos_elem (const posicao_p p, size_t num, posicao_s q)
{
	assert(p != NULL);
	bool ret = false;
	for (size_t i = 0;
	     i < num && !(ret = posicao_igual(p[i], q));
	     i++);
	return ret;
}

size_t pos_filter (posicao_p p, size_t num, bool (* f) (posicao_s))
{
	assert(p != NULL);
	assert(num > 0);
	assert(f != NULL);

	size_t w = 0;

	for (size_t r = 0; r < num; r++)
		if (f(p[r]))
			p[w++] = p[r];

	return w;
}

size_t pos_sq_dist (posicao_s p1, posicao_s p2)
{
#define min(A, B) ((A) < (B)) ? (A) : (B)
	size_t dx = p2.x - min(p1.x, p2.x);
	size_t dy = p2.y - min(p1.y, p2.y);
	return dx + dy;
#undef min
}

size_t pos_mais_perto (const posicao_p ps, size_t N, posicao_s p)
{
	assert(ps != NULL);
	assert(N > 0);
	size_t ret = 0;
	for (size_t i = 1; i < N; i++)
		ret = (pos_sq_dist(ps[ret], p) >= pos_sq_dist(ps[i], p)) ?
			i :
			ret;
	return ret;
}

posicao_s posicao_new (abcissa x, ordenada y)
{
	return (posicao_s) {
		.x = x,
		.y = y,
	};
}
