#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "estado.h"
#include "html.h"
#include "posicao.h"

int main (void)
{
	srandom(time(NULL));
	estado_s e = ler_estado(getenv("QUERY_STRING"));

	CONTENT_TYPE;

	ABRE_SVG(SVG_WIDTH, SVG_HEIGHT); {
	} FECHA_SVG;

	return 0;
}
