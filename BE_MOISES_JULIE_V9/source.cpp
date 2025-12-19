#include "source.h"
#include <cmath>
#include <iostream>

using namespace std;

// ==========================================
// CLASSE SOURCE (Mère)
// ==========================================
source::source(){
    // Constructeur vide
}

source::~source(){} // Destructeur

// ==========================================
// CLASSES ABSTRAITES INTERMEDIAIRES
// ==========================================
periodique::periodique(){
}


periodique::~periodique(){}

nonPeriodique::nonPeriodique(){
}


nonPeriodique::~nonPeriodique(){}

// ==========================================
// CLASSE ECHELON (Step)
// ==========================================
echelon::echelon(){
    // Constructeur interactif pour le menu
    cout << "=== Configuration de la source ===" << endl;
    cout << "Entrez l'amplitude maximale : ";
    cin >> this->Amp;
    cout << "Entrez le retard (delay) : ";
    cin >> this->delay;

    // Affichage récapitulatif
    cout << "Vous avez choisi la source echelon." << endl;
    cout << "Amplitude : " << this->Amp << " V" << endl;
    cout << "Retard : " << this->delay << endl;
}

// Constructeur paramétré pour les tests
echelon::echelon(double Amp, double delay, double Offset) {
    this->Amp = Amp;
    this->delay = delay;
    this->Offset = Offset;
}

echelon::~echelon(){}

// Génération du signal échelon
double echelon::trace_signal(double t){
    if (t < 0) {
        return 0;
    } else {
        // Le signal s'active après le temps de délai
        if (t > this->delay) {
            return this->Amp + this->Offset;
        } else {
            return 0;
        }
    }
}

// ==========================================
// CLASSE PULSE (Impulsion)
// ==========================================
pulse::pulse(){
    cout << "=== Configuration de la source ===" << endl;
    cout << "Entrez l'amplitude maximale : ";
    cin >> this->Amp;
    cout << "Entrez le retard (delay) : ";
    cin >> this->delay;
    cout << "Entrez la duree de l'impulsion : ";
    cin >> this->pulseDuration;

    cout << "Vous avez choisi la source impulsion." << endl;
    cout << "Amplitude : " << this->Amp << " V" << endl;
    cout << "Retard : " << this->delay << endl;
    cout << "Duree : " << this->pulseDuration << " s" << endl;
}

pulse::pulse(double Amp, double delay, double pulseDuration, double Offset){
    this->Amp = Amp;
    this->delay = delay;
    this->Offset = Offset;
    this->pulseDuration = pulseDuration;
}
pulse::~pulse(){}

double pulse::trace_signal(double t){
    // Le signal est haut uniquement dans la fenêtre [delay, delay + durée]
    if ((t >= this->delay) && (t < this->delay + this->pulseDuration)) {
        return (this->Amp + this->Offset);
    } else {
        return (0);
    }
}

// ==========================================
// CLASSE TRIANGLE
// ==========================================
triangle::triangle(){
    cout << "=== Configuration de la source ===" << endl;
    cout << "Entrez l'amplitude maximale : ";
    cin >> this->Amp;
    cout << "Entrez l'offset : ";
    cin >> this->Offset;
    cout << "Entrez la phase en degres : ";
    cin >> this->Phase;
    cout << "Entrez la periode : ";
    cin >> this->Periode;

    cout << "Vous avez choisi la source triangle." << endl;
    cout << "Amplitude : " << this->Amp << " V" << endl;
    cout << "Offset : " << this->Offset << " V" << endl;
    cout << "Phase : " << this->Phase << endl;
    cout << "Periode : " << this->Periode << " s" << endl;

    // Conversion de la phase (degrés -> temps)
    this->Phase = (this->Phase * this->Periode) / 360.0;
}

triangle::triangle(double Amp, double Phase, double Periode, double Offset){
    this->Amp = Amp;
    this->Periode = Periode;
    // Conversion phase degrés -> temps
    this->Phase = (Phase * this->Periode) / 360.0;
    this->Offset = Offset;
}
triangle::~triangle(){}

double triangle::trace_signal(double t){
    // Calcul du décalage effectif
    int phiPeriods = this->Phase / this->Periode;
    double phi_eff = this->Phase - phiPeriods * this->Periode;

    // Logique pour créer les pentes montantes et descendantes
    if (t < phi_eff)
    {
        if (phi_eff > this->Periode / 2.0)
        {
            if (t < phi_eff - (this->Periode / 2.0))
            {
               return 2 * (this->Amp / this->Periode) * (t + (this->Periode - phi_eff)) + this->Offset;
            }
            else
            {
               return -2 * (this->Amp / this->Periode) * (t - phi_eff) + this->Offset;
            }
        }
        else
        {
           return -2 * (this->Amp / this->Periode) * (t - phi_eff) + this->Offset;
        }
    }
    else
    {
        // Gestion de la périodicité
        int nPeriods = (t - phi_eff) / this->Periode;
        double t_eff = t - phi_eff - nPeriods * this->Periode;

        if (t_eff <= this->Periode / 2.0)
        {
           return 2 * (this->Amp / this->Periode) * t_eff + this->Offset;
        }
        else
        {
           return -2 * (this->Amp / this->Periode) * (t_eff - this->Periode) + this->Offset;
        }
    }
}

// ==========================================
// CLASSE SINUS
// ==========================================
sinus::sinus(){
    cout << "=== Configuration de la source ===" << endl;
    cout << "Entrez l'amplitude maximale : ";
    cin >> this->Amp;
    cout << "Entrez l'offset : ";
    cin >> this->Offset;
    cout << "Entrez la phase en degres : ";
    cin >> this->Phase;
    cout << "Entrez la periode : ";
    cin >> this->Periode;

    cout << "Vous avez choisi la source triangle." << endl;
    cout << "Amplitude : " << this->Amp << " V" << endl;
    cout << "Offset : " << this->Offset << " V" << endl;
    cout << "Phase : " << this->Phase << endl;
    cout << "Periode : " << this->Periode << " s" << endl;

    // Conversion phase degrés -> radians
    this->Phase = this->Phase * (M_PI / 180.0);
}

sinus::sinus(double Amp, double Phase, double Periode, double Offset){
    this->Amp = Amp;
     this->Periode = Periode;
    // Conversion phase degrés -> radians pour le calcul sin()
    this->Phase = Phase * (M_PI / 180.0);
    this->Offset = Offset;
}
sinus::~sinus(){}

double sinus::trace_signal(double t){
    if (t < 0) {
        return 0;
    } else {
        // Formule classique : A * sin(wt + phi) + offset
        return (this->Amp) * sin(2 * M_PI * (t / this->Periode)+ this->Phase) ;
    }
}

// ==========================================
// CLASSE SQUARE (Créneau)
// ==========================================
square::square(){
cout << "=== Configuration de la source ===" << endl;
    cout << "Entrez l'amplitude maximale : ";
    cin >> this->Amp;
    cout << "Entrez l'offset : ";
    cin >> this->Offset;
    cout << "Entrez la phase en degres : ";
    cin >> this->Phase;
    cout << "Entrez la periode : ";
    cin >> this->Periode;

    cout << "Vous avez choisi la source triangle." << endl;
    cout << "Amplitude : " << this->Amp << " V" << endl;
    cout << "Offset : " << this->Offset << " V" << endl;
    cout << "Phase : " << this->Phase << endl;
    cout << "Periode : " << this->Periode << " s" << endl;
     cout << "Dutcicly : " << this->dutcicly << " s" << endl;


    // Conversion phase degrés -> temps
    this->Phase = (this->Phase * this->Periode) / 360.0;
}

square::square(double Amp, double Phase, double Periode, double dutcicly, double Offset){
    this->Amp = Amp;
    this->Periode = Periode;
    this->Phase = (Phase * this->Periode) / 360.0;
    this->dutcicly = dutcicly;
    this->Offset = Offset;
}
square::~square(){}

double square::trace_signal(double t){
    if (t <= 0) {
        return 0;
    }

    double tmod;

    // Calcul du temps modulo la période pour répéter le signal
    if (t >= this->Phase) {
        tmod = fmod(t - this->Phase, this->Periode);
    } else {
        tmod = this->Periode + fmod(t - this->Phase, this->Periode);
    }

    // Comparaison avec le rapport cyclique
    if (tmod <= this->dutcicly * this->Periode) {
        return (this->Amp) + this->Offset; // État haut
    } else {
        return (0) + this->Offset; // État bas
    }
}
