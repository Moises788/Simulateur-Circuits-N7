#ifndef SOURCE_H
#define SOURCE_H
#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// CLASSE MÈRE (Abstraite)
class source
{
    protected:
        double Amp;    // Amplitude
        double Offset; // Decalage DC

    public:
        source();
        virtual ~source(); // Destructeur virtuel pour le polymorphisme
        virtual double trace_signal(double t)=0; // Methode pure (doit etre redefinie)
};

// CLASSE INTERMEDIAIRE : PERIODIQUE
class periodique : public source
{
    protected:
        double Periode;
        double Phase;

    public:
        periodique();
        virtual ~periodique();
        double trace_signal(double t)=0;
};

// CLASSE INTERMEDIAIRE : NON PERIODIQUE
class nonPeriodique : public source
{
    protected:
        double delay; // Retard du signal

    public:
        nonPeriodique();
        virtual ~nonPeriodique();
        double trace_signal(double t)=0;
};

// --- CLASSES CONCRETES ---

// ECHELON (Step)
class echelon : public nonPeriodique
{
    public:
        echelon(); // Interactif
        echelon(double Amp, double delay, double Offset); // Parametré
        ~echelon();
        double trace_signal(double t);
};

// IMPULSION (Pulse)
class pulse : public nonPeriodique
{
    protected:
        double pulseDuration;

    public:
        pulse();
        pulse(double Amp, double delay, double  pulseDuration,double Offset);
        ~pulse();
        double trace_signal(double t);
};

// TRIANGLE
class triangle : public periodique
{
    public:
        triangle();
        triangle(double Amp, double Phase, double Periode,double Offset);
        ~triangle();
        double trace_signal(double t);
};

// SINUS
class sinus : public periodique
{
    public:
        sinus();
        sinus(double Amp, double Phase, double Periode,double Offset);
        ~sinus();
        double trace_signal(double t);
};

// CRENEAU (Square)
class square : public periodique
{
    protected:
       double dutcicly; // Rapport cyclique

    public:
        square();
        square(double Amp, double Phase, double Periode, double dutcicly, double Offset);
        ~square();
        double trace_signal(double t);
};

#endif // SOURCE_H
