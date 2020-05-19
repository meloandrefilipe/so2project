#include "Cidade.h"

Cidade::Cidade()
{
	this->exit = false;
}

Cidade::~Cidade()
{
}

BOOL Cidade::isExit() const
{
	return this->exit;
}

void Cidade::setExit(BOOL val)
{
	this->exit = val;
}
