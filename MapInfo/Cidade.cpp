#include "Cidade.h"

Cidade::Cidade()
{
	this->exit = false;
	this->dll = new DLLProfessores();
}

Cidade::~Cidade()
{
	delete this->dll;
}

BOOL Cidade::isExit() const
{
	return this->exit;
}

void Cidade::setExit(BOOL val)
{
	this->exit = val;
}
