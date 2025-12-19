#include "circuit.h"
#include "source.h"
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>

using namespace std;

// =======================
// METHODES CLASSE CIRCUIT
// =======================

// Constructeur par defaut
circuit::circuit(){}

// Associe l'objet source au circuit via un pointeur
void circuit::getsource(source *Src){
    this->Src = Src;
}

// Destructeur
circuit::~circuit(){}

// =======================
// METHODES CIRCUIT ORDRE 1
// =======================

circuit1::circuit1(){}
circuit1::~circuit1(){}

// Resolution par Euler (y_n+1 = y_n + h * f)
void circuit1::Euler(double t, double h, double *vs){
    double f;
    f = this->f1(*vs, t); // Appel polymorphe de l'equation diff
    *vs = *vs + h * f;
}

// Resolution par Heun (Predicteur-Correcteur)
void circuit1::Heun(double t, double h, double *vs){
    double un1, un2;

    un1 = this->f1(*vs, t);                  // Pente au debut
    un2 = this->f1(*vs + h * un1, t + h);    // Pente a la fin (predite)

    *vs = *vs + 0.5 * h * (un1 + un2);       // Moyenne des pentes
}

// Resolution par Runge-Kutta 4 (RK4)
void circuit1::Kutta(double t, double h, double *vs){
    double un1, un2, un3, un4;

    un1 = this->f1(*vs, t);
    un2 = this->f1(*vs + 0.5 * h * un1, t + 0.5 * h);
    un3 = this->f1(*vs + 0.5 * h * un2, t + 0.5 * h);
    un4 = this->f1(*vs + h * un3, t + h);

    // Moyenne ponderee des 4 pentes
    *vs = *vs + (h / 6.0) * (un1 + 2 * un2 + 2 * un3 + un4);
}

// =======================
// METHODES CIRCUIT ORDRE 2
// =======================

circuit2::circuit2(){}
circuit2::~circuit2(){}

// Euler pour systeme d'ordre 2 (Vs et dVs)
void circuit2::Euler2(double t, double h, double *vs, double *dvs){
    double f;
    f = this->f2(*vs, *dvs, t, h); // Calcul acceleration

    *dvs = *dvs + h * f;       // Mise a jour vitesse
    *vs = *vs + h * (*dvs);    // Mise a jour position
}

// Heun pour systeme d'ordre 2
void circuit2::Heun2(double t, double h, double *vs, double *dvs){
    double un1, un2, dv1, dv2;

    // Etape 1 : Pentes initiales
    un1 = *dvs;
    dv1 = this->f2(*vs, *dvs, t, h);

    // Etape 2 : Estimation au point suivant
    un2 = *dvs + h * dv1;
    dv2 = this->f2(*vs + h * un1, *dvs + h * dv1, t + h, h);

    // Mise a jour finale
    *vs += 0.5 * h * (un1 + un2);
    *dvs += 0.5 * h * (dv1 + dv2);
}

// RK4 pour systeme d'ordre 2
void circuit2::Kutta2(double t, double h, double *vs, double *dvs){
    double un1, un2, un3, un4; // Pentes pour Vs (vitesse)
    double dv1, dv2, dv3, dv4; // Pentes pour dVs (acceleration)

    // k1
    un1 = *dvs;
    dv1 = this->f2(*vs, *dvs, t, h);

    // k2
    un2 = *dvs + 0.5 * h * dv1;
    dv2 = this->f2(*vs + 0.5 * h * un1, *dvs + 0.5 * h * dv1, t + 0.5 * h, h);

    // k3
    un3 = *dvs + 0.5 * h * dv2;
    dv3 = this->f2(*vs + 0.5 * h * un2, *dvs + 0.5 * h * dv2, t + 0.5 * h, h);

    // k4
    un4 = *dvs + h * dv3;
    dv4 = this->f2(*vs + h * un3, *dvs + h * dv3, t + h, h);

    // Application de la formule RK4
    *vs += (h / 6.0) * (un1 + 2 * un2 + 2 * un3 + un4);
    *dvs += (h / 6.0) * (dv1 + 2 * dv2 + 2 * dv3 + dv4);
}

// =======================
// CIRCUIT A (RC Serie)
// =======================

// Constructeur interactif
circuitA::circuitA(){
    cout << "=== Configuration du Circuit A (RC) ===" << endl;
    cout << "Entrez la valeur de R (Ohms) : ";
    cin >> this->R;
    cout << "Entrez la valeur de C (Farads) : ";
    cin >> this->C;
    cout << "-> Circuit A (R="<< this->R << " Ohms, C= "<< this->C << " F)" << endl;
}

// Constructeur parametre
circuitA::circuitA(double R, double C){
    this->R = R;
    this->C = C;
}

circuitA::~circuitA(){}

// Equation diff: dVs/dt = (Ve - Vs) / RC
double circuitA::f1(double vs, double t){
    return ((this->Src->trace_signal(t) - vs) / (this->R * this->C));
}

// =======================
// CIRCUIT B (Modele Diode/RC)
// =======================

circuitB::circuitB(){
    cout << "=== Configuration du Circuit B ===" << endl;
    cout << "Entrez la valeur de R1 (Ohms) : ";
    cin >> this->R1;
    cout << "Entrez la valeur de R2 (Ohms) : ";
    cin >> this->R2;
    cout << "Entrez la valeur de C (Farads) : ";
    cin >> this->C;
    cout << "-> Circuit B (R1="<< this->R1 << " Ohms, R2="<< this->R2 << " Ohms, C= "<< this->C << " F)" << endl;
}

circuitB::circuitB(double R1, double R2, double C){
    this->R1 = R1;
    this->R2 = R2;
    this->C = C;
}

circuitB::~circuitB(){}

// Equation avec seuil de diode (0.6V)
double circuitB::f1(double vs, double t){
    double ve = this->Src->trace_signal(t);
    double vbe = 0.6; // Tension seuil

    // Si diode passante
    if (ve > vbe) {
        return -1 * (1 / (this->R1 * this->C) + 1 / (this->R2 * this->C)) * vs + (ve - vbe) / (this->R1 * this->C);
    } else {
        // Diode bloquee, decharge C dans R2
        return -vs / (this->R2 * this->C);
    }
}

// =======================
// CIRCUIT C (RLC Serie)
// =======================

circuitC::circuitC(){
    cout << "=== Configuration du Circuit C (RLC) ===" << endl;
    cout << "Entrez la valeur de R (Ohms) : ";
    cin >> this->R;
    cout << "Entrez la valeur de L (Henry) : ";
    cin >> this->L;
    cout << "Entrez la valeur de C (Farads) : ";
    cin >> this->C;
    cout << "-> Circuit C (R="<< this->R << " Ohms, L="<< this->L << " H, C= "<< this->C << " F)" << endl;
}

circuitC::circuitC(double R, double L, double C){
    this->R = R;
    this->L = L;
    this->C = C;
}
circuitC::~circuitC(){}

// Equation RLC Serie canonique
double circuitC::f2(double vs, double dvs, double t, double h){
    double ve = this->Src->trace_signal(t);
    // d2Vs/dt2 en fonction de Vs, dVs/dt et Ve
    return (-(this->R / this->L) * (dvs) + (ve - vs) / (this->L * this->C));
}

// =======================
// CIRCUIT D (RLC Parallele/Bouchon)
// =======================

circuitD::circuitD(){
    cout << "=== Configuration du Circuit D ===" << endl;
    cout << "Entrez la valeur de R (Ohms) : ";
    cin >> this->R;
    cout << "Entrez la valeur de L (Henry) : ";
    cin >> this->L;
    cout << "Entrez la valeur de C (Farads) : ";
    cin >> this->C;
    cout << "-> Circuit D (R="<< this->R << " Ohms, L="<< this->L << " H, C= "<< this->C << " F)" << endl;
}

circuitD::circuitD(double R, double L, double C){
    this->R = R;
    this->L = L;
    this->C = C;
}
circuitD::~circuitD(){}

double circuitD::f2(double vs, double dvs, double t, double h){
    // Approximation numerique de la derivee de l'entree (dVe/dt)
    double dve = (this->Src->trace_signal(t + h) - this->Src->trace_signal(t)) / h;

    // Equation specifique circuit D
    return (((dve - dvs) / (this->R * this->C)) - ((vs) / (this->L * this->C)));
}
