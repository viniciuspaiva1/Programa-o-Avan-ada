#ifndef POLINOMIO_H_
#define POLINOMIO_H_

#include <iostream>

class Poly{
private:
    unsigned D;
    double *x;

public:
    Poly();
    Poly(const Poly &P);
    explicit Poly(unsigned grau);
    ~Poly();
    void recriar (unsigned grau);
    void operator= (const Poly &P);
    bool empty() const;
    bool isZero() const;
    int getGrau() const;
    double getCoef(unsigned i) const;
    double operator[] (unsigned i) const;
    double getValor(double valor) const;
    double operator()(double valor) const;
    void setCoef(unsigned i, double valor);
    friend std::ostream& operator<<(std::ostream& COUT, const Poly &p);
    friend std::istream& operator>>(std::istream& CIN, const Poly &p);
    bool ler(std::string nome_arq);
    bool salvar(std::string nome_arq);
    Poly operator+(const Poly &p) const;
    Poly operator-(const Poly &p) const;
    Poly operator-()const;
    Poly operator*(const Poly &p) const;



};
#endif // POLINOMIO_H_
