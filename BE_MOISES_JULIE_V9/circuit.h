#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <cmath>
#include "source.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =======================
// CLASSE MÈRE (Abstraite)
// =======================
class circuit{
    protected:
        source *Src; // Pointeur vers la source de tension

    public:
        circuit();
        virtual ~circuit();
        void getsource(source *Src); // Lier la source au circuit

};

// =======================
// CLASSE ABSTRAITE ORDRE 1
// =======================
class circuit1: public circuit{
    protected:
        double C; // Condensateur (Farads)

    public:
        circuit1();

        // Methodes de resolution numerique (1er ordre)
        void Euler(double t, double h, double *vs);
        void Heun(double t, double h, double *vs);
        void Kutta(double t, double h, double *vs);

        virtual ~circuit1();

        // Equation differentielle f(vs, t) a redefinir (virtuelle pure)
        virtual double f1(double vs, double t)=0;

};

// =======================
// CLASSE ABSTRAITE ORDRE 2
// =======================
class circuit2: public circuit{
    protected:
        double R; // Resistance
        double L; // Bobine (Inductance)
        double C; // Condensateur

    public:
        circuit2(); // Constructeur par defaut

        // Methodes de resolution numerique (2eme ordre)
        void Euler2(double t, double h, double *vs, double *dvs);
        void Heun2(double t, double h, double *vs, double *dvs);
        void Kutta2(double t, double h, double *vs, double *dvs);

        virtual ~circuit2();

        // Equation differentielle f(vs, dvs, t) a redefinir
        virtual double f2(double vs, double dvs, double t, double h)=0;

};

// =======================
// CIRCUITS CONCRETS
// =======================

// Circuit A (RC Serie)
class circuitA: public circuit1{
    protected:
        double R; // Resistance

    public:
        circuitA(); // Constructeur interactif
        circuitA(double R, double C); // Constructeur parametre
        ~circuitA();
        double f1(double vs, double t); // Equation caracteristique A
};

// Circuit B (RC avec R parallele)
class circuitB: public circuit1{
    protected:
        double R1;
        double R2;
        double f1(double vs, double t); // Equation caracteristique B

    public:
        circuitB();
        circuitB(double _R1,double _R2, double _C);
        ~circuitB();
};

// Circuit C (RLC Serie)
class circuitC: public circuit2{
    public:
        circuitC();
        circuitC(double R,double L,double C); // Constructeur parametre
        ~circuitC();
        double f2(double vs, double dvs, double t, double h); // Equation caracteristique C
};

// Circuit D (RLC Parallele)
class circuitD: public circuit2{
    public:
        circuitD();
        circuitD(double _R,double _L, double _C);
        ~circuitD();
        double f2(double vs, double dvs, double t, double h); // Equation caracteristique D
};

#endif // CIRCUIT_H
