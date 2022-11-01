#include <iostream>
#include <cmath> // devido ao pow
#include <string> //devido ao salvar
#include <fstream>

#include "Polinomio.h"

using namespace std;

//Construtor default
Poly::Poly(): D(0), x(nullptr){}
//Por cópia
Poly::Poly(const Poly &P): D(P.D), x(nullptr){
    if(D>0) x = new double[D];
    for(unsigned i=0; i<D; i++) x[i] = P.x[i];
}
//especifico
Poly::Poly(unsigned grau): D(grau+1), x(nullptr){
    if(grau == 0){
        x = new double [D];
        x[0] = 0.0;
    }else{
        x = new double[D];
        for(unsigned i=0; i<D; i++){
            if(i == grau) x[i] = 1.0;
            else x[i] = 0.0;
        }
    }
}
//Destrutor
Poly::~Poly(){
    if(x!=nullptr) delete[] x;
}
void Poly::recriar(unsigned grau){
    D = grau + 1;
    if(grau == 0){
        if(x!=nullptr) delete[] x;
        x = new double[D];
        x[0] = 0.0;
    }else{
        if(x!=nullptr) delete[] x;
        x = new double[D];
        for(unsigned i=0; i<D; i++){
            if(i == grau) x[i] = 1.0;
            else x[i] = 0.0;
        }
    }
}
//Operador de atribuição, parece com o destrutor + construtor por copia
void Poly::operator=(const Poly &P){
    if(this != &P){
        if(D != P.D){
            if(x!=nullptr) delete[] x;
            D = P.D;
            if(D>0) x = new double[D];
            else x = nullptr;
        }
        for(unsigned i=0; i<D; i++) x[i] = P.x[i];
    }
}

bool Poly::empty() const{
    if(D == 0) return true;
    return false;
}
bool Poly::isZero() const {
    if(D == 1 && x[0] == 0.0) return true;
    return false;
}
int Poly::getGrau() const{
    if(this->empty()) return -1;
    return int(D)-1;
}
double Poly::getCoef(unsigned i) const{
    if(empty()) cerr << "Polinomio Vazio";
    if(i >= D) return 0.0;
    return x[i];
}
double Poly::operator[] (unsigned i) const{
    return this->getCoef(i);
}

double Poly::getValor(double valor) const{
    if(this->empty()) return 0.0;
    double soma(0);
    for(int i=0; i<=getGrau(); i++)soma = soma + x[i]*pow(valor, i);
    return soma;
}
double Poly::operator() (double valor) const{
    return this->getValor(valor);
}
void Poly::setCoef(unsigned i, double valor){
    if(this->getGrau() != 0){
        if(valor == 0 && i == this->getGrau()) cerr << "Parametros invalidos";
        else x[i] = valor;
    }else x[i] = valor;
}
ostream& operator<<(ostream& COUT, const Poly &p){
    if(!p.empty()){
        for(int i=p.getGrau(); i>=0; i--){
            if(p[i] == 0){
                if(i == 0 && p.getGrau() == 0) COUT << p[i];
            }else{
                if(p[i] < 0.0) COUT << '-';
                else if(i!=p.getGrau()) COUT << '+';
                if(abs(p[i]) != 1.0 || i == 0.0) COUT << abs(p[i]);
                if(i!= 0){
                    if(abs(p[i])!= 1.0) COUT << '*';
                    COUT << 'x';
                    if(i > 1) COUT << '^' << i;
                }
            }
        }
    }
    return COUT;
}
istream& operator>> (istream& CIN, const Poly &p){
    if(!p.empty()){
            double val = 0;
        for(int i = p.getGrau(); i >=0; i--){
            if(p.getGrau() > 0 && i != 0) cout << "Insira o valor do elemento x^" <<i << endl;
            else cout << "Insira o valor do termo independente" << endl;
            CIN >> val;
            if(p.getGrau() != 0){
                while(val == 0.0 && i == p.getGrau()){
                    cerr << "Valor de x^" <<i<< " não pode ser nulo" << endl;
                    cout << "Insira o valor de x^" << i<< " novamente" << endl;
                    CIN >> val;
                }
                p.x[i] = val;
            }p.x[i] = val;
        }
        return CIN;
    } cerr << "Polinomio Vazio";
}

bool Poly::salvar(string nome_arq){
    ofstream arquivo(nome_arq.c_str()); //c_str() converte string em array de char
    if(!arquivo.is_open()) return false;
    arquivo << "POLY " << this->D << endl;
    if(!this->empty()){
        for(int i = 0; i<=this->getGrau(); i++){
            if(i != this->getGrau()) arquivo << this->getCoef(i) << " ";
            else arquivo << this->getCoef(i) << endl;
        }
    }return false;
}
bool Poly::ler(string nome_arq){
    ifstream arquivo(nome_arq.c_str());
    if(!arquivo.is_open()) return false;
    string cabecalho;
    arquivo >> cabecalho;
    if(!arquivo.good() || cabecalho != "POLY"){
        arquivo.close();
        return false;
    }
    //unsigned prov_D;

    unsigned proD;
    arquivo >> proD;
    Poly ptemp(proD-1);
    if(!arquivo.good() || proD <= 0){
        arquivo.close();
        return false;
    }
    //Recebe todos os indices do Polinomio
    for(int i = 0; i<proD; i++){
        arquivo >> ptemp.x[i];
        if(i == proD && ptemp.x[i] == 0.0){   //verifica se o maior elemento é nulo
            arquivo.close();
            if(ptemp.x != nullptr) delete [] ptemp.x;
            return false;
        }
    }
    *this = ptemp;



    //delete [] prov_a;
}
Poly Poly::operator+(const Poly &p) const{
    if(this->empty() || this->isZero())return p;
    if(p.empty() || p.isZero())return *this;

    Poly prov(max(this->getGrau(), p.getGrau()));
    for(int i = 0; i<=prov.getGrau(); i++){
        prov.x[i] = getCoef(i) + p.getCoef(i);
    }
    //   VERIFICA E CONSERTA SE HOUVER ELEMENTO NULO NO MAIOR ELEMENTO (CANCELAMENTO DE ELEMENTOS 2X^2 - 2X^2)
    while(prov.getCoef(prov.getGrau()) == 0.0 && prov.getGrau() > 0){
        Poly prov2;
        prov2.D = prov.D - 1;
        if(prov2.D>0) prov2.x = new double[prov2.D];
        else prov2.x = nullptr;
        for(int i=0; i<=prov2.getGrau(); i++){
            prov2.x[i] = prov.x[i];
        }
        prov = prov2;
    }

    return prov;
}

Poly Poly::operator-(const Poly &p) const{

    if(this->empty() || this->isZero()) return -p;
    if(p.empty() || p.isZero()) return *this;

    Poly prov(max(this->getGrau(), p.getGrau()));
    for(int i = 0; i<=prov.getGrau(); i++){
        prov.x[i] = getCoef(i) - p.getCoef(i);
    }
    //   VERIFICA E CONSERTA SE HOUVER ELEMENTO NULO NO MAIOR ELEMENTO (CANCELAMENTO DE ELEMENTOS 2X^2 - 2X^2)
    while(prov.getCoef(prov.getGrau()) == 0.0 && prov.getGrau() > 0){
        Poly prov2;
        prov2.D = prov.D - 1;
        if(prov2.D>0) prov2.x = new double[prov2.D];
        else prov2.x = nullptr;
        for(int i=0; i<=prov2.getGrau(); i++){
            prov2.x[i] = prov.x[i];
        }
        prov = prov2;
        //if(prov2.x != nullptr) delete [] prov2.x;
    }
    return prov;

}
Poly Poly::operator-()const{
    if(empty())  return *this;
    if(isZero()) return *this;
    Poly prov(*this);
    for(int i = 0; i<= getGrau(); i++) prov.x[i] = -getCoef(i);
    return prov;
}

Poly Poly::operator*(const Poly &p) const{
    if(p.empty() || this->empty()) return Poly();
    if(p.isZero()|| this->isZero()) return Poly(0);

    Poly prov(this->getGrau() + p.getGrau());
    for(int k = 0; k<=prov.getGrau(); k++){
        prov.x[k] = 0.0;
    }
    for(int i =0; i<=this->getGrau(); i++){
        for(int j = 0; j<=p.getGrau(); j++){
            prov.x[i+j] = prov.x[i+j] + getCoef(i) * p.getCoef(j);

        }
    }
    return prov;
}
