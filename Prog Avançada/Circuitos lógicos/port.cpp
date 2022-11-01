#include <fstream>
#include "port.h"

//
// CLASSE PORT
//

/// ***********************
/// Inicializacao e finalizacao
/// ***********************

// Construtor (recebe como parametro o numero de entradas da porta)
// Dimensiona o array id_in e inicializa elementos com valor invalido (0),
// inicializa out_port com UNDEF
Port::Port(unsigned NI):id_in(NI,0),out_port(bool3S::UNDEF)
{
  // Nao pode testar o parametro NI com validNumInputs pq o construtor de
  // Port eh chamado pelo construtor de Port_NOT, mas sem que ocorra
  // polimorfismo nesse momento.
  // Entao seria chamada a versao base de validNumInputs, que testaria se
  // estah entre 2 e 4 e geraria erro ao criar portas NOT
  /*
  if (!validNumInputs(NI))
  {
    id_in.clear();
  }
  */
}

// Construtor por copia
Port::Port(const Port& P):id_in(P.id_in),out_port(P.out_port)
{
}

// Destrutor (nao faz nada)
Port::~Port() {}

/// ***********************
/// Funcoes de testagem
/// ***********************

// Retorna true se um numero de entradas eh possivel para a porta, ou seja, se NI >= 2
// (na funcao correspondente na porta NOT, substituir por NI==1)
bool Port::validNumInputs(unsigned NI) const
{
  return (NI>=2);
}

// Retorna true se um indice (qual entrada da porta) eh valido (entre 0 e NumInputs-1)
bool Port::validIndex(unsigned I) const
{
  return (/*I>=0 && */I<getNumInputs());
}

// Retorna true se a porta eh valida (estah com todos os dados corretos):
// - validNumInputs eh true
// - Todas as id de entrada da porta sao diferentes de zero.
bool Port::valid() const
{
  if (!validNumInputs(getNumInputs())) return false;
  for (unsigned i=0; i<getNumInputs(); i++) if (id_in.at(i) == 0) return false;
  return true;
}

/// ***********************
/// Funcoes de consulta
/// ***********************

// Caracteristicas da porta
unsigned Port::getNumInputs() const
{
  return id_in.size();
}

// Saida logica da porta
bool3S Port::getOutput() const
{
  return out_port;
}

// Depois de testar o parametro (validIndex), retorna o valor de id_in[I]
// ou 0 se indice invalido
int Port::getId_in(unsigned I) const
{
  if (!validIndex(I)) return 0;
  return id_in.at(I);
}

/// ***********************
/// Funcoes de modificacao
/// ***********************

// Fixa o numero de entradas da porta
// Depois de testar o parametro (validNumInputs), se o novo numero de entradas for igual ao
// anterior n�o faz nada; caso seja diferente, redimensiona e inicializa os elementos do
// array id_in com valor invalido (0)
void Port::setNumInputs(unsigned NI)
{
  if (validNumInputs(NI) && NI!=getNumInputs())
  {
    id_in.resize(NI);
    for (unsigned i=0; i<getNumInputs(); i++) id_in.at(i) = 0;
  }
}

// Fixa o valor logico da saida da porta (?, F, T)
void Port::setOutput(bool3S S)
{
  out_port=S;
}

// Fixa a origem do sinal da I-esima entrada da porta como sendo Id
// Depois de testar os parametros (validIndex, Id!=0), faz: id_in[I] <- Id
void Port::setId_in(unsigned I, int Id)
{
  if (validIndex(I) && Id!=0) id_in.at(I) = Id;
}

/// ***********************
/// E/S de dados
/// ***********************

// Leh uma porta do teclado. O usuario deve digitar:
// - o numero de entradas da porta (testa com validNumInputs); e
// - a id de cada uma das entradas da porta (testa se != 0)
// Alternativamente, pode testar a porta como um todo ao final da digitacao (usa valid)
// Se o usuario digitar um dado invalido, o metodo deve pedir que ele digite novamente
// Este metodo para digitar uma porta com numero variavel de entradas (>= 2)
// vale para todas as ports, exceto NOT.
// O metodo virtual digitar tem que ser refeito para a NOT.
// Nao precisa ser reimplementado nas demais ports
// ATENCAO: o metodo NAO vai solicitar que o usuario digite o tipo de porta
// (NT,AN,NA,OR,NO,XO,NX). Esse valor jah deve ter sido digitado previamente e a porta
// criada dinamicamente (new) do tipo certo, para que seja chamado o metodo virtual
// digitar apropriado para o tipo de porta.
void Port::digitar()
{
  unsigned Nin;
  do
  {
    std::cout << " Numero de entradas da porta: ";
    std::cin >> Nin;
    if (this->validNumInputs(Nin))
    {
      id_in.resize(Nin);
      for (unsigned i=0; i<getNumInputs(); i++)
      {
        std::cout << "  Entrada " << i << " da porta: ";
        std::cin >> id_in.at(i);
      }
    }
    else id_in.clear();
  }
  while(!valid());
}

// Leh uma porta da stream ArqI. Deve fixar o valor:
// - do numero de entradas da porta; e
// - da id de cada uma das portas
// Retorna true se tudo OK, false se houve erro
// Este metodo nao eh virtual, pois pode ser feito generico de forma a servir para
// todas as ports.
// Basta que o metodo teste o numero de entradas com a funcao virtual validNumInputs()
// ATENCAO: o metodo NAO vai ler do arquivo o tipo de porta
// (NT,AN,NA,OR,NO,XO,NX). Esse valor jah deve ter sido lido previamente e a porta
// criada dinamicamente do tipo certo, para que seja chamado o metodo virtual ler
// apropriado para o tipo de porta.
bool Port::ler(std::istream& ArqI)
{
  try
  {
    unsigned Nin;
    char c;

    ArqI >> Nin;
    if (!ArqI.good() || !validNumInputs(Nin)) throw 1;
    id_in.resize(Nin);
    ArqI >> c;
    if (!ArqI.good() || c != ':') throw 2;
    for (unsigned i=0; i<getNumInputs(); i++)
    {
      ArqI >> id_in.at(i);
      if (!ArqI.good() || id_in.at(i) == 0) throw 3;
    }
  }
  catch (int erro)
  {
    id_in.clear();
    return false;
  }
  return true;
}

// Imprime a porta na ostrem ArqO (cout ou uma stream de arquivo, tanto faz)
// Imprime:
// - a string com o nome da porta + ESPACO
// - o numero de entradas colado com ':'; e
// - ESPACO + as ids de cada uma das entradas
// Este metodo nao eh virtual, pois pode ser feito generico de forma a servir para
// todas as ports.
// Basta que o metodo imprima o resultado da chamada aa funcao virtual getName() para o nome
// Os outros dados a serem impressos sao iguais em todas as portas
// Retorna a propria ostream O recebida como parametro de entrada, para que possa
// ser encadeada
std::ostream& Port::imprimir(std::ostream& ArqO) const
{
  ArqO << this->getName() << ' ';
  ArqO << getNumInputs() << ':';
  for (unsigned j=0; j<getNumInputs(); j++)
  {
    ArqO << ' ' << id_in.at(j);
  }
  return ArqO;
}

// Operador << com comportamento polimorfico
// Serve para todas as ports (NO, AND, NOR, etc.)
std::ostream& operator<<(std::ostream& O, const Port& X)
{
  return (&X)->imprimir(O);
};

////////////////////////
/// AS OUTRAS PORTS  ///
////////////////////////

/// +++ PORTA NOT +++ ///

///CONTRUTOR
Port_NOT::Port_NOT(): Port(1){}

///CLONE
ptr_Port Port_NOT::clone() const{
    return new Port_NOT(*this);
}

///RETORNA NOME DA PORTA
std::string Port_NOT::getName() const{
    return "NT";
}

///VALIDA ENTRADA
bool Port_NOT::validNumInputs (unsigned NI) const{
    if(NI != 1) return false;
    return true;
}

///DIGITAR
void Port_NOT::digitar(){
    int id;
    std::cout << "Digite a ID de entrada da porta NOT: ";
    std:: cin >> id;
    id_in.at(0)=(id);
}

///SIMULAR
void Port_NOT::simular(const std::vector<bool3S>& in_port){
    if(in_port.size() != 1){
        out_port = bool3S::UNDEF;
        return;
    }
    out_port = ~in_port.at(0);
}
/// +++ PORTA AND +++ ///

///CONTRUTOR
Port_AND::Port_AND(): Port(){}

///CLONE
ptr_Port Port_AND::clone() const{
    return new Port_AND(*this);
}

///RETORNA NOME DA PORTA
std::string Port_AND::getName() const{
    return "AN";
}

///SIMULAR
void Port_AND::simular(const std::vector<bool3S>& in_port){
    out_port = in_port.at(0);
    for(unsigned i=1; i<getNumInputs(); i++){
        out_port &=in_port.at(i);
    }
}


/// +++ PORTA NAND +++ ///

///CONTRUTOR
Port_NAND::Port_NAND(): Port(){}

///CLONE
ptr_Port Port_NAND::clone() const{
    return new Port_NAND(*this);
}

///RETORNA NOME DA PORTA
std::string Port_NAND::getName()const{
    return "NA";
}

///SIMULAR
void Port_NAND::simular(const std::vector<bool3S>& in_port){

    out_port = in_port.at(0);
    for(unsigned i=1; i<getNumInputs(); i++){
        out_port &=in_port.at(i);
    }
    out_port = ~out_port;
}

/// +++ PORTA OR +++ ///

///CONTRUTOR
Port_OR::Port_OR():Port() {}

///CLONE
ptr_Port Port_OR::clone() const{
    return new Port_OR(*this);
}

///RETORNA NOME DA PORTA
std::string Port_OR::getName() const{
    return "OR";
}

///SIMULAR
void Port_OR::simular(const std::vector<bool3S>& in_port){
    out_port = in_port.at(0);
    for(unsigned i=1; i<getNumInputs(); i++){
        out_port |=in_port.at(i);
    }
}

/// +++ PORTA NOR +++ /////

///CONTRUTOR
Port_NOR::Port_NOR():Port() {}

///CLONE
ptr_Port Port_NOR::clone() const{
    return new Port_NOR(*this);
}

///RETORNA NOME DA PORTA
std::string Port_NOR::getName() const{
    return "NO";
}

///SIMULAR
void Port_NOR::simular(const std::vector<bool3S>& in_port){
    out_port = in_port.at(0);
    for(unsigned i=1; i<getNumInputs(); i++){
        out_port |= in_port.at(i);
    }
    out_port = ~out_port;
}

/// +++ PORTA XOR +++ ///

///CONTRUTOR
Port_XOR::Port_XOR():Port(){}

///CLONE
ptr_Port Port_XOR::clone() const{
    return new Port_XOR(*this);
}

///RETORNA NOME DA PORTA
std::string Port_XOR::getName()const{
    return "XO";
}

///SIMULAR
void Port_XOR::simular(const std::vector<bool3S>& in_port){

    out_port = in_port.at(0);
    for(unsigned i=1; i<getNumInputs(); i++){
        out_port ^= in_port.at(i); //enxugar escrita &=
    }

}

/// +++ PORTA NXOR +++ ///

///CONTRUTOR
Port_NXOR::Port_NXOR():Port() {}

///CLONE
ptr_Port Port_NXOR::clone() const{
    return new Port_NXOR(*this);
}

///RETORNA NOME DA PORTA
std::string Port_NXOR::getName()const{
    return "NX";
}

///SIMULAR
void Port_NXOR::simular(const std::vector<bool3S>& in_port){

    out_port = in_port.at(0);
    for(unsigned i=1; i<getNumInputs()-1; i++){
        out_port = out_port ^ in_port.at(i); //enxugar escrita &=
    }
    out_port = ~out_port;
}

