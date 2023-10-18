#include <stdbool.h>

struct conta {
	float saldo;
	/* Outros campos. */
};

bool saque(struct conta *c, float valor)
{
	float saldo = c->saldo;
	if (valor > saldo) {
		return false;
	}

	c->saldo = saldo - valor;

	return true;
}

int main()
{
	return 0;
}
