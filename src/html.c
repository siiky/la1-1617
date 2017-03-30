#include "estado.h"
#include "jogo.c"

#include "html.h"

void imprime_casa (size_t l, size_t c)
{
	char * cores[] = { COR_PAR, COR_IMPAR };
	char * cor = cores[(l + c) % 2];
	QUADRADO(l, c, ESCALA, cor);
}

void imprime_tabuleiro (void)
{
	for (size_t l = 0; l < TAM; l++)
		for (size_t c = 0; c < TAM; c++)
			imprime_casa(l, c);
}

void imprime_jogo (estado_p e)
{
	char * str = NULL;

	if (e == NULL || (str = estado2str(e)) == NULL)
		return;

	printf("<A XLINK:HREF=http://localhost/cgi-bin/rogue?%s>", str); {
	imprime_tabuleiro();
	} printf("</A>");
}
