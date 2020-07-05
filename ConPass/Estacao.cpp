#include "Estacao.h"

Estacao::Estacao()
{
	this->dll = new DLLProfessores();
	this->exit = false;
	this->hReadPipe = NULL;
	this->hWritePipe = NULL;
	this->hMutex = CreateMutex(NULL, FALSE, NULL);
	if (this->hMutex == NULL)
	{
		_tprintf(TEXT("Central CreateMutex error: %d\n"), GetLastError());
		return;
	}

}

Estacao::~Estacao()
{
	delete this->dll;
	CloseHandle(this->hReadPipe);
	CloseHandle(this->hWritePipe);
	ReleaseMutex(this->hMutex);
	CloseHandle(this->hMutex);
}

vector<Passageiro*> Estacao::getPassageiros()
{
	return this->passageiros;
}

vector<HANDLE*> Estacao::getHandles()
{
	return this->handles;
}

DWORD Estacao::connectPipes()
{
	HANDLE hEventA = CreateEvent(NULL, TRUE, FALSE, EVENT_CONPASS_A);
	HANDLE hEventB = CreateEvent(NULL, TRUE, FALSE, EVENT_CONPASS_B);
	if (hEventA == NULL) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento de espera pelo pipe!"), TYPE::ERRO);
		return EXIT_FAILURE;
	}
	WaitForSingleObject(hEventA, INFINITE);
	this->hWritePipe = CreateFile(NAMED_PIPE_CONPASS_A, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (this->hWritePipe == INVALID_HANDLE_VALUE) {
		this->dll->log((TCHAR*)TEXT("Erro a aceder ao named pipe!"), TYPE::ERRO);
		return EXIT_FAILURE;
	}
	WaitForSingleObject(hEventB, INFINITE);
	this->hReadPipe = CreateFile(NAMED_PIPE_CONPASS_B, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (this->hReadPipe == INVALID_HANDLE_VALUE) {
		this->dll->log((TCHAR*)TEXT("Erro a aceder ao named pipe!"), TYPE::ERRO);
		return EXIT_FAILURE;
	}

	CloseHandle(hEventA);
	CloseHandle(hEventB);
	return EXIT_SUCCESS;
}

void Estacao::addPassageiro(Passageiro* p)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->passageiros.push_back(p);
	ReleaseMutex(this->hMutex);
}

void Estacao::deletePassageiro(Passageiro* p)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	for (auto iter = passageiros.begin(); iter != passageiros.end();) {
		if (_tcscmp((*iter)->getId(), p->getId()) == 0) {
			delete* iter;
			iter = passageiros.erase(iter);
			break;
		}
		else {
			++iter;
		}
	}
	ReleaseMutex(this->hMutex);
}

void Estacao::execStatus(Passageiro* passageiro){
	if (passageiro != nullptr) {
		tstringstream msg;
		switch (passageiro->getStatus())
		{
		case STATUS::ACEITE:
			msg << "[ACEITE] " << "O passageiro " << passageiro->getId() << " entrou no sistema!" << endl;
			this->addPassageiro(passageiro);
			break;
		case STATUS::TRANSPORTAR:
			msg << "[TRANSPORTAR] " << "Quero ser transportado!" << endl;
			this->deletePassageiro(passageiro);
			break;
		case STATUS::NOSTREET:
			msg << "[SEM ESTRADA] " << "A localização selecionada para o passageiro " << passageiro->getId() << " não é uma estrada!" << endl;
			this->deletePassageiro(passageiro);
			break;
		case STATUS::ENTREGUE:
			msg << "[ENTREGUE] " << "O passageiro " << passageiro->getId() << " foi entregue no seu destino <" << passageiro->getDestRow() << " ,"<< passageiro->getDestCol() << ">!" << endl << "Sayonara! :D" << endl;
			this->deletePassageiro(passageiro);
			break;
		case STATUS::EMESPERA:
			msg << "[EM ESPERA] " << "O passageiro " << passageiro->getId() << " está em lista de espera por um taxi!"  << endl;
			break;
		case STATUS::ACAMINHO:
			msg << "[A CAMINHO] " << "O taxi "<< passageiro->getPlate() <<" foi designado para o passageiro " << passageiro->getId() << " e demora "<< passageiro->getTimeToArrive() << " segundos!" << endl;
			break;
		case STATUS::NOCARRO:
			msg << "[TAXI] " << "O passageiro " << passageiro->getId() << " foi recolhido pelo taxi" << passageiro->getPlate() <<" e está a caminho do seu destino, chega em "<< passageiro->getTimeToArrive() <<" segundos!" << endl;
			break;
		case STATUS::BUFFERCHEIO:
			msg << "[BUFFERCHEIO] " << "O passageiro " << passageiro->getId() << " foi rejeitado devido ao grando fluxo de passageiros na aplicação!" << endl << "Tente novamente mais tarde!" << endl; 
			passageiro->setStatus(STATUS::ACEITE);
			break;
		case STATUS::SEMINTERESSE:
			msg << "[SEM TRANSPORTE] " << "O passageiro " << passageiro->getId() << " não recebeu pedidos de interesse de transporte!" << endl << "Tente novamente mais tarde!" << endl;
			passageiro->setStatus(STATUS::ACEITE);
			break;
		default:
			msg << "Algo de errado não está certo!" << endl;
			break;
		}
		msg << "COMMAND: ";
		_tprintf(TEXT("\n"));
		_tprintf(msg.str().c_str());
	}
	else {
		this->dll->log((TCHAR*)TEXT("Impossivel atualizar o passageiro!"), TYPE::ERRO);
	}

}

void Estacao::setExit(BOOL val)
{
	this->exit = val;
}

void Estacao::addHandle(HANDLE* h)
{
	this->handles.push_back(h);
}

Passageiro* Estacao::addMove(TCHAR* id, int row, int col)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	for (int i = 0; i < passageiros.size(); i++)
	{
		if (_tcscmp(passageiros[i]->getId(), id) == 0) {
			if (passageiros[i]->getStatus() == STATUS::ACEITE) {
				passageiros[i]->setDestRow(row);
				passageiros[i]->setDestCol(col);
				passageiros[i]->setStatus(STATUS::TRANSPORTAR);
			}
			else {
				passageiros[i]->setStatus(STATUS::ERRO);
			}
			ReleaseMutex(this->hMutex);
			return passageiros[i];
		}
	}
	ReleaseMutex(this->hMutex);
	return nullptr;
}

PASSENGER Estacao::readNamedPipe(){
	PASSENGER passageiro;
	DWORD noBytesRead;
	BOOL readedNamedPipe = ReadFile(this->hReadPipe, &passageiro, sizeof(PASSENGER), &noBytesRead, NULL);
	if (!readedNamedPipe) {
		this->dll->log((TCHAR*)TEXT("Erro a ler do named pipe!"), TYPE::ERRO);
		return passageiro;
	}

	return passageiro;
}

BOOL Estacao::writeNamedPipe(Passageiro* client)
{
	PASSENGER passageiro = client->getStruct();
	DWORD noBytesWrite;
	_tcscpy_s(passageiro.id, PASSENGER_NAME_SIZE, client->getId());
	passageiro.col = client->getCol();
	passageiro.row = client->getRow();
	passageiro.dest_col = client->getDestCol();
	passageiro.dest_row = client->getDestRow();
	passageiro.status = client->getStatus();
	BOOL writedNamedPipe = WriteFile(this->hWritePipe, &passageiro, sizeof(PASSENGER), &noBytesWrite, NULL);
	if (!writedNamedPipe) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel escrever no Named Pipe Conpass!"), TYPE::ERRO);
	}
	return writedNamedPipe;
}

Passageiro* Estacao::getPassageiro(TCHAR* id)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	for (int i = 0; i < passageiros.size(); i++)
	{
		if (_tcscmp(passageiros[i]->getId(), id) == 0) {
			ReleaseMutex(this->hMutex);
			return passageiros[i];
		}
	}
	ReleaseMutex(this->hMutex);
	return nullptr;
}

Passageiro* Estacao::updateClient(PASSENGER p)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	for (int i = 0; i < this->passageiros.size(); i++)
	{
		if (_tcscmp(this->passageiros[i]->getId(), p.id) == 0) {
			this->passageiros[i]->update(p);
			ReleaseMutex(this->hMutex);
			return this->passageiros[i];
		}
	}
	ReleaseMutex(this->hMutex);
	return new Passageiro(&p);
}

BOOL Estacao::isExit() const
{
	return this->exit;
}