#include <iostream>
#include <fstream>
#include <iomanip>
#include "circuito.h"

using namespace std;

///
/// As strings que definem os tipos de porta
///

// Funcao auxiliar que testa se uma string com nome de porta eh valida
// Caso necessario, converte os caracteres da string para maiusculas
bool validType(std::string& Tipo)
{
  if (Tipo.size()!=2) return false;
  Tipo.at(0) = toupper(Tipo.at(0));
  Tipo.at(1) = toupper(Tipo.at(1));
  if (Tipo=="NT" ||
      Tipo=="AN" || Tipo=="NA" ||
      Tipo=="OR" || Tipo=="NO" ||
      Tipo=="XO" || Tipo=="NX") return true;
  return false;
}

// Funcao auxiliar que retorna um ponteiro que aponta para uma porta alocada dinamicamente
// O tipo da porta alocada depende do parametro string de entrada (AN, OR, etc.)
// Caso o tipo nao seja nenhum dos validos, retorna nullptr
// Pode ser utilizada nas funcoes: Circuito::setPort, Circuito::digitar e Circuito::ler
ptr_Port allocPort(std::string& Tipo)
{
  if (!validType(Tipo)) return nullptr;

  if (Tipo=="NT") return new Port_NOT;
  if (Tipo=="AN") return new Port_AND;
  if (Tipo=="NA") return new Port_NAND;
  if (Tipo=="OR") return new Port_OR;
  if (Tipo=="NO") return new Port_NOR;
  if (Tipo=="XO") return new Port_XOR;
  if (Tipo=="NX") return new Port_NXOR;

  // Nunca deve chegar aqui...
  return nullptr;
}

///######### CLASSE CIRCUITO #########///


///CONSTRUTOR
Circuito::Circuito(): Nin(0),id_out(), out_circ(), ports(){}

///CONTRUTOR POR CÓPIA
Circuito::Circuito(const Circuito& C){
    Nin=C.Nin;
    id_out.resize(C.id_out.size());
    out_circ.resize(C.out_circ.size());

    for(unsigned i=0; i<C.id_out.size(); i++){
        id_out.at(i) = C.id_out.at(i);
        out_circ.at(i) = C.out_circ.at(i);
    }
    for(unsigned i=0; i<C.ports.size(); i++){
        ports.push_back(C.ports.at(i)->clone());
    }
}

///DESTRUTOR
Circuito::~Circuito(){
    clear();
}

///CLEAR
void Circuito::clear(){
    Nin = 0;
    id_out.clear();
    out_circ.clear();
    for(int i = 0; i<ports.size(); i++){
            delete ports.at(i);
    }
    ports.clear();
}

///OPERRATOR = (ATRIBUIÇÃO)
void Circuito::operator=(const Circuito& C){
    clear();
    Nin=C.Nin;
    id_out.resize(C.id_out.size());
    out_circ.resize(C.out_circ.size());

    for(unsigned i=0; i<C.id_out.size(); i++){
        id_out.at(i) = C.id_out.at(i);
        out_circ.at(i) = C.out_circ.at(i);
    }
    for(unsigned i=0; i<C.ports.size(); i++){
        ports.push_back(C.ports.at(i)->clone());
    }
}

void Circuito::resize(unsigned NI, unsigned NO, unsigned NP){
    if(NI > 0 && NO > 0 && NP >0){
        clear();
        Nin = NI;
        id_out.resize(NO, 0);
        ports.resize(NP, nullptr);
        out_circ.resize(NO, bool3S::UNDEF);
    }
}

/// ***********************
/// Funcoes de testagem
/// ***********************

// Retorna true se IdInput eh uma id de entrada do circuito valida (entre -1 e -NInput)
bool Circuito::validIdInput(int IdInput) const
{
  return (IdInput<=-1 && IdInput>=-int(getNumInputs()));
}

// Retorna true se IdOutput eh uma id de saida do circuito valida (entre 1 e NOutput)
bool Circuito::validIdOutput(int IdOutput) const
{
  return (IdOutput>=1 && IdOutput<=int(getNumOutputs()));
}

// Retorna true se IdPort eh uma id de porta do circuito valida (entre 1 e NPort)
bool Circuito::validIdPort(int IdPort) const
{
  return (IdPort>=1 && IdPort<=int(getNumPorts()));
}

// Retorna true se IdOrig eh uma id valida para a origem do sinal de uma entrada de porta ou
// para a origem de uma saida do circuito (podem vir de uma entrada do circuito ou de uma porta)
// validIdOrig == validIdInput OR validIdPort
bool Circuito::validIdOrig(int IdOrig) const
{
  return validIdInput(IdOrig) || validIdPort(IdOrig);
}

// Retorna true se IdPort eh uma id de porta valida (validIdPort) e
// a porta estah definida (estah alocada, ou seja, != nullptr)
bool Circuito::definedPort(int IdPort) const
{
  if (!validIdPort(IdPort)) return false;
  if (ports.at(IdPort-1)==nullptr) return false;
  return true;
}

// Retorna true se IdPort eh uma porta existente (definedPort) e
// todas as entradas da porta com Id de origem valida (usa getId_inPort e validIdOrig)
bool Circuito::validPort(int IdPort) const
{
  if (!definedPort(IdPort)) return false;
  for (unsigned j=0; j<getNumInputsPort(IdPort); j++)
  {
    if (!validIdOrig(getId_inPort(IdPort,j))) return false;
  }
  return true;
}

// Retorna true se o circuito eh valido (estah com todos os dados corretos):
// - numero de entradas, saidas e portas valido (> 0)
// - todas as portas validas (usa validPort)
// - todas as saidas com Id de origem validas (usa getIdOutput e validIdOrig)
// Essa funcao deve ser usada antes de salvar ou simular um circuito
bool Circuito::valid() const
{
  if (getNumInputs()==0) return false;
  if (getNumOutputs()==0) return false;
  if (getNumPorts()==0) return false;
  for (unsigned i=0; i<getNumPorts(); i++)
  {
    if (!validPort(i+1)) return false;
  }
  for (unsigned i=0; i<getNumOutputs(); i++)
  {
    if (!validIdOrig(getIdOutput(i+1))) return false;
  }
  return true;
}

/// ***********************
/// Funcoes de consulta
/// ***********************

///RETORNA O NÚMERO DE ENTRADADAS DO CIRCUITO
unsigned Circuito::getNumInputs() const{
    return Nin;
}

///RETORNA O NÚMERO DE SAIDAS DO CIRCUITO
unsigned Circuito::getNumOutputs() const{
    return id_out.size();
}

///RETORNA O NÚMERO DE PORTAS DO CIRCUITO
unsigned Circuito::getNumPorts() const{
    return ports.size();
}

///RETORNA O ID DE UMA SAÍDA
int Circuito::getIdOutput(int IdOutput) const{
    if(!validIdOutput(IdOutput)) return 0;
    return id_out.at(IdOutput-1);
}

///RETORNA O RESULTADO DE UMA SAÍDA
bool3S Circuito::getOutput(int IdOutput) const{
    if(!validIdOutput(IdOutput)) return bool3S::UNDEF;
    return out_circ.at(IdOutput-1);
}

///RETORNA O TIPO DA DE UMA PORTA
std::string Circuito::getNamePort(int IdPort) const{
    if(!definedPort(IdPort)) return "??";
    return ports.at(IdPort-1)->getName();
}

///RETORNA O NUMERO DE ENTRADAS DE UMA PORTA
unsigned Circuito::getNumInputsPort(int IdPort) const{
    if(!definedPort(IdPort)) return 0;
    return ports.at(IdPort-1)->getNumInputs();
}

///RETORNA O ID DE UMA ENTRADA DE UMA PORTA
int Circuito::getId_inPort(int IdPort, unsigned I) const{
    if(definedPort(IdPort) && validIdOrig(IdPort)){
        return ports.at(IdPort-1)->getId_in(I);
    }
    return 0;
}

/// ***********************
/// Funcoes de modificacao
/// ***********************

///MUDA A ORIGEM DA SAÍDA
void Circuito::setIdOutput(int IdOut, int IdOrig){
    if(validIdOrig(IdOrig) && validIdOutput(IdOut)) id_out.at(IdOut-1)=IdOrig;
}

///MUDA TIPO DE PORTA
void Circuito::setPort(int IdPort, std::string Tipo, unsigned NIn){
    if(validIdPort(IdPort) && validType(Tipo)){
        delete ports.at(IdPort-1);
        ports.at(IdPort-1) = allocPort(Tipo);
        ports.at(IdPort-1)->setNumInputs(NIn);
    }
}

///MUDA O ID DA PORTA
void Circuito::setId_inPort(int IdPort, unsigned I, int IdOrig) const{
    if(validIdPort(IdPort) && validIdOrig(IdOrig) && ports.at(IdPort-1)->validIndex(I))
         ports.at(IdPort-1)->setId_in(I,IdOrig);

}

/// ***********************
/// E/S de dados
/// ***********************

///FUNÇÃO PARA DIGITAR UM CIRCUITO
void Circuito::digitar(){

    unsigned ent(0), saida(0), portas(0);
    std::string tipo;


    cout << "Digite o numero de entradas: ";
    while(ent <= 0){
        cin >> ent;
    }
    cout << "Digite o numero de saidas: ";
    while(saida <= 0){
        cin >> saida;
    }
    cout << "Digite o numero de portas: ";
    while(portas <= 0){
        cin >> portas;
    }

    resize(ent, saida, portas);

    for(unsigned i=0; i<portas; i++){


        cout << "Digite o tipo da porta " << i+1 << endl;
        cout << "Opcoes: NT, AN, NA, OR, NO, XO, NX" << endl;
        cin >> tipo;
        while(!validType(tipo)){
            cout << "Tipo invalido!" << endl;
            cout << "Digite o tipo da porta " << i+1<< ":";
                cin >> tipo;
        }
        if(ports.at(i) != nullptr) delete ports.at(i);  ///lembrar de fazer  nos outros
        ports.at(i) = allocPort(tipo);
        do{
            ports.at(i)->digitar();
        }while(!validPort(i+1));


    }
    for(unsigned i = 0; i <saida; i++){
        int idSaida;
        cout << "Digite o Id da saida " << i+1 << ":";
            cin >> idSaida;
        while(!validIdOrig(idSaida)){
            cout << "Id Invalido!" << endl;
            cout << "Digite o Id da saida " << i+1 << ":";
                cin >> idSaida;
        }
        id_out.at(i) = (idSaida);
    }

}

///FUNÇÃO PARA LER UM CIRCUITO A PARTIR DE UM ARQUIVO
bool Circuito::ler(const std::string& arq){

     ///ABRE O ARQUIVO
     ifstream arqv(arq.c_str());

     try{
        if (!arqv.is_open()) throw 1;

        std::string prov, prov1;
        unsigned in, out, portas;
        unsigned idprov;
        char test;

        arqv >> prov >> in >> out >> portas >> prov1;

        if (!arqv.good() || prov != "CIRCUITO" ||
                in<=0 || out<=0 || portas<=0 || prov1 != "PORTAS") throw 2;
        resize(in,out,portas);


        //LENDO AS PORTAS
        for(unsigned i=0 ; i<portas; i++){

            string nomePorta;
            arqv >> idprov >> test;

            if(idprov != i+1 || test != ')') throw 3;

            //LÊ O TIPO DA PORTA, AN, XO....
            arqv >> nomePorta;

            if(!validType(nomePorta)) throw 4; //redundancia pois o alloc ja testa o tipo
            //Cria uma porta do tipo correspondente
            ptr_Port portaProv  = allocPort(nomePorta);
            if(!portaProv->ler(arqv)) throw 5;
            //Adiciona no Vetor de portas caso esteja tudo certo
            ports.at(i) = portaProv;
        }

        arqv >> prov;
        if(prov != "SAIDAS") throw 6;
        for(int i=0 ; i<out; i++){
            int saida;
            arqv >> idprov >> test;
            if(idprov != i+1 || test != ')') throw 7;
            arqv >> saida;
            id_out.at(i) = saida;
        }

        bool circ_valid = valid();
        if(!circ_valid) throw 8;

    }
    catch (int i)
  {
    //ERRO DE LEITURA
    clear();
    return false;
  }

  return true;
}

///FUNÇÃO IMPRIMIR
std::ostream& Circuito::imprimir(std::ostream& O) const{

    if(!valid()){
        cout << "ERROR!! Circuito invalido!!" << endl;
        exit(0);
    }

    O << "CIRCUITO " << getNumInputs() << " " << id_out.size() << " " <<  ports.size() << endl;
    O << "PORTAS" << endl;

    for(unsigned i=0; i<ports.size(); i++){
        O << i+1 <<") " << *ports.at(i) ;

        O << endl;
    }
    O << "SAIDAS" << endl;
    for(unsigned i=0; i<id_out.size(); i++){
       O << i+1 <<") " << id_out.at(i) << endl;
    }

    return O;

}

///FUNÇÃO PARA SALVAR EM ARQUIVO
bool Circuito::salvar(const std::string& arq) const{

    if(!this->valid()) return false;

    ofstream nome_arq(arq.c_str());

    if (!nome_arq.is_open()) return false;

    nome_arq << *this;


    nome_arq.close();

    return true;
}

/// ***********************
/// SIMULACAO (funcao principal do circuito)
/// ***********************

bool Circuito::simular(const std::vector<bool3S>& in_circ){

    bool tudo_def, alguma_def;
    int id;
    vector<bool3S> in_port;

    for(unsigned i=0; i<getNumPorts(); i++){
        ports.at(i)->setOutput(bool3S::UNDEF);
    }
    do{
        tudo_def=true;
        alguma_def=false;
        //cout << "entrei aqui 1" << endl;
        for(unsigned i=0; i<getNumPorts(); i++){
            if(ports.at(i)->getOutput()  == bool3S::UNDEF){
                for(unsigned j = 0; j<ports.at(i)->getNumInputs(); j++){
                    id = ports.at(i)->getId_in(j);
                    if(id >0){
                        //in_port.at(j) = ports.at(id - 1)->getOutput();
                        in_port.push_back(ports.at(id - 1)->getOutput());
                    }else{
                        //in_port.at(j) = in_circ.at(-id-1);
                        in_port.push_back(in_circ.at(-id-1));
                    }

                }
                ports.at(i)->simular(in_port);
                if (ports.at(i)->getOutput()== bool3S::UNDEF) tudo_def = false;
                else alguma_def = true;
            }
           //cout << "entrei aqui 2" << endl;
        }
    }while(!tudo_def && alguma_def);

    for(unsigned j = 0; j<getNumOutputs(); j++){
        id = id_out.at(j);
        if(id > 0) out_circ.at(j) = ports.at(id-1)->getOutput();
        else out_circ.at(j) = in_circ.at(-id-1);
    }
}

///SOBRECARGA DO OPERADOR <<
std::ostream& operator<<(std::ostream& O, const Circuito& C){
    if(!C.valid()){
       cout << "ERROR!! Circuito inválido!!";
    }
    C.imprimir(O);
}

