#include <stdio.h>
#include <math.h>
#include <iostream>
#include "source.h"
#include "circuit.h"

using namespace std;

// Fonction de simulation pour les circuits du premier ordre (ex: RC, RL)
void simuler_ordre1(circuit1 *C, source *S, double h, double duration, int choix_methode)
{
    // Calcul du nombre total de pas de temps
    int npas = (int)(duration / h);

    // Allocation dynamique pour le tableau de temps
    double *t = new double[npas + 1];

    double vs = 0.0; // Tension de sortie
    double ve = 0.0; // Tension d'entrée

    // Ouverture des fichiers pour sauvegarder les résultats
    FILE *fich_ve = fopen("ve.txt", "wt");
    FILE *fich_vs = fopen("vs.txt", "wt");

    // Connexion de la source au circuit
    C->getsource(S);

    cout << "Calcul en cours (Ordre 1)... ";

    // Boucle de simulation temporelle
    for(int i = 0; i < npas; i++)
    {
        // Gestion du temps t[i]
        if (i == 0) {
            t[0] = 0;
        } else {
            t[i] = t[i-1] + h;
        }

        // Calcul et enregistrement de la tension d'entrée (Ve)
        ve = S->trace_signal(t[i]);
        fprintf(fich_ve, "%e\t %e \n", t[i], ve);

        // Résolution de l'équation différentielle selon la méthode choisie
        if (choix_methode == 1) {
            C->Euler(t[i], h, &vs); // Méthode d'Euler
        }
        else if (choix_methode == 2) {
             C->Heun(t[i], h, &vs); // Méthode de Heun
        }
        else if (choix_methode == 3) {
            C->Kutta(t[i], h, &vs); // Méthode de Runge-Kutta (RK4)
        }

        // Enregistrement de la tension de sortie (Vs)
        fprintf(fich_vs, "%e\t %e \n", t[i], vs);
    }

    cout << "Termine !" << endl;
    cout << "Resultats sauvegardes dans 've.txt' et 'vs.txt'." << endl;

    // Fermeture des fichiers et libération de la mémoire
    fclose(fich_ve);
    fclose(fich_vs);
    delete[] t;
}

// Fonction de simulation pour les circuits du second ordre (ex: RLC)
void simuler_ordre2(circuit2 *C, source *S, double h, double duration, int choix_methode)
{
    int npas = (int)(duration / h);
    double *t = new double[npas + 1];

    double vs = 0.0;  // Tension de sortie
    double dvs = 0.0; // Dérivée de la tension de sortie
    double ve = 0.0;  // Tension d'entrée

    FILE *fich_ve = fopen("ve.txt", "wt");
    FILE *fich_vs = fopen("vs.txt", "wt");

    C->getsource(S);

    cout << "Calcul en cours (Ordre 2)... ";

    for(int i = 0; i < npas; i++)
    {
        // Mise à jour du temps
        if (i == 0) {
           t[0] = 0;
        }
        else {
           t[i] = t[i-1] + h;
        }

        // Calcul de l'entrée
        ve = S->trace_signal(t[i]);
        fprintf(fich_ve, "%e\t %e \n", t[i], ve);

        // Résolution numérique (prend en compte vs et sa dérivée dvs)
        if (choix_methode == 1) {
            C->Euler2(t[i], h, &vs, &dvs);
        }
        else if (choix_methode == 2) {
            C->Heun2(t[i], h, &vs, &dvs);
        }
        else if (choix_methode == 3) {
             C->Kutta2(t[i], h, &vs, &dvs);
        }

        fprintf(fich_vs, "%e\t %e \n", t[i], vs);
    }

    cout << "Termine !" << endl;
    cout << "Resultats sauvegardes dans 've.txt' et 'vs.txt'." << endl;

    fclose(fich_ve);
    fclose(fich_vs);
    delete[] t;
}

int main()
{
    // Variables de contrôle pour les boucles while
    char continuer= 'o';
    char change_source= 'o';
    char modifier_composant = 'o';

    // Paramètres globaux de simulation
    double h = 0.1e-9;          // Pas de temps
    double duration = 500e-9;   // Durée totale

    // Boucle principale du programme
    while (continuer == 'o' || continuer == 'O')
    {
        // Pointeurs vers les objets (initialisés à NULL)
        circuit1 *C1 = NULL;
        circuit2 *C2 = NULL;
        source *S = NULL;

        // Variables pour stocker les choix de l'utilisateur
        int choix_ordre = 0;
        int choix_circuit = 0;
        int choix_methode = 0;
        int choix_source = 0;

        // Drapeaux de validation des entrées
        bool ordre_valide = false;
        bool circuit_valide = false;
        bool methode_valide = false;
        bool type_source = true;

        cout << "\n==========================================" << endl;
        cout << "    SIMULATEUR DE CIRCUITS ELECTRIQUES    " << endl;
        cout << "==========================================" << endl;

        // --- ÉTAPE 1 : Choix de l'ordre du circuit ---
        while (!ordre_valide) {
            cout << "\nQuel type de circuit souhaitez-vous ?" << endl;
            cout << " 1) Ordre 1 (RC, RL...)" << endl;
             cout << " 2) Ordre 2 (RLC...)" << endl;
            cout << "Choix : ";
            cin >> choix_ordre;

            // Vérification si l'entrée n'est pas un nombre
            if (cin.fail()) {
                cin.clear();// Réinitialise l'état d'erreur de cin pour permettre de nouvelles lectures.
                // Vide le buffer. "1000" est juste un nombre arbitraire supposé
                //"assez grand" pour tout effacer jusqu'au saut de ligne.
                cin.ignore(1000, '\n');
                cout << "Entree invalide. Veuillez entrer un nombre." << endl;
            }
            else {
                if (choix_ordre == 1 || choix_ordre == 2) {
                    ordre_valide = true;
                } else {
                    cout << "Choix invalide." << endl;
                }
            }
        }

        // --- ÉTAPE 2 : Choix du circuit spécifique et des composants ---
        while (!circuit_valide) {
            cout << "\n=== CHOIX DU CIRCUIT ===" << endl;

            // Affichage des options selon l'ordre choisi
            if (choix_ordre == 1) {
                cout << "Vous avez choisi l'ordre 1. Options :" << endl;
                cout << " 1) Circuit A (RC Serie)" << endl;
                cout << " 2) Circuit B (RC Serie avec resistance parallele)" << endl;
            } else {
                 cout << "Vous avez choisi l'ordre 2. Options :" << endl;
                cout << " 1) Circuit C (RLC Serie)" << endl;
                cout << " 2) Circuit D (RLC Parallele)" << endl;
            }

            cout << "Choix : ";
            cin >> choix_circuit;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Entree invalide. Veuillez entrer un nombre." << endl;
            }
            else {
            if (choix_circuit == 1 || choix_circuit == 2) {
                circuit_valide = true;

                // Instanciation des objets Circuit selon le choix
                if (choix_ordre == 1) {
                    if (choix_circuit == 1) {
                        // --- Circuit A (RC) ---
                        cout << "-> Circuit A (Defaut: R=50, C=1nF)" << endl;
                        cout << "Voulez-vous modifier les valeurs des composants ? (o/n) : ";
                        cin >> modifier_composant;
                        if (modifier_composant == 'o' || modifier_composant == 'O') {
                            C1 = new circuitA(); // Constructeur interactif
                        } else {
                            C1 = new circuitA(50, 1e-9); // Constructeur paramétré (défaut)
                        }
                    } else {
                        // --- Circuit B (RC Parallèle) ---
                        cout << "-> Circuit B (Defaut: R1=36, R2=180, C=1nF)" << endl;
                        cout << "Voulez-vous modifier les valeurs des composants ? (o/n) : ";
                         cin >> modifier_composant;
                        if (modifier_composant == 'o' || modifier_composant == 'O') {
                            C1 = new circuitB();
                        } else {
                            C1 = new circuitB(36, 180, 1e-9);
                        }
                    }
                }
                else { // Ordre 2
                    if (choix_circuit == 1) {
                        // --- Circuit C (RLC Série) ---
                        cout << "-> Circuit C (Defaut: R=120, L=1uH, C=0.1nF)" << endl;
                        cout << "Voulez-vous modifier les valeurs des composants ? (o/n) : ";
                        cin >> modifier_composant;
                        if (modifier_composant == 'o' || modifier_composant == 'O') {
                             C2 = new circuitC();
                        } else {
                            C2 = new circuitC(120, 1e-6, 0.1e-9);
                        }
                    } else {
                        // --- Circuit D (RLC Parallèle) ---
                        cout << "-> Circuit D (Defaut: R=1.2, L=10nH, C=0.1uF)" << endl;
                        cout << "Voulez-vous modifier les valeurs des composants ? (o/n) : ";
                        cin >> modifier_composant;
                        if (modifier_composant == 'o' || modifier_composant == 'O') {
                            C2 = new circuitD();
                        } else {
                             C2 = new circuitD(1.2, 10e-9, 0.1e-6);
                        }
                    }
                }
            } else {
                cout << "Choix invalide." << endl;
            }
        }
            }

        // --- ÉTAPE 3 : Choix de la méthode numérique ---
        while (!methode_valide) {
            cout << "\n=== CHOIX DE LA METHODE DE RESOLUTION ===" << endl;
            cout << "Quelle methode numerique souhaitez-vous utiliser ?" << endl;
             cout << " 1) Euler" << endl;
            cout << " 2) Heun" << endl;
            cout << " 3) Runge-Kutta (RK4)" << endl;
            cout << "Choix : ";
            cin >> choix_methode;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Entree invalide. Veuillez entrer un nombre." << endl;
            }
            else {
            if (choix_methode >= 1 && choix_methode <= 3) {
                methode_valide = true;
            } else {
                cout << "Choix invalide. Veuillez entrer un nombre entre 1 et 3." << endl;
                }
            }
        }

        // --- ÉTAPE 4 : Choix et configuration de la source ---
        while(type_source){
            cout << "\n=== CHOIX DE LA SOURCE ===" << endl;
            cout << "Quelle source d'alimentation desirez-vous ?" << endl;
            cout << " 1) Sinus" << endl;
            cout << " 2) Triangle" << endl;
            cout << " 3) Creneau (Square)" << endl;
            cout << " 4) Impulsion (Pulse)" << endl;
            cout << " 5) Echelon" << endl;
            cout << "Choix : ";
            cin >> choix_source;

                        if (cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Entree invalide. Veuillez entrer un nombre." << endl;
            }
            else {

            // Switch pour gérer les différents types de sources
            switch(choix_source) {
                case 1: // Sinus
                     cout << "Vous avez choisi la source sinus." << endl;
                     cout << "Defaut -> Amp: 1V, Phase: 0, Periode: 1e-7s" << endl;
                     cout << "\nSouhaitez-vous modifier les proprietes de la source ? (o/n) : ";
                     cin >> change_source;
                     if (change_source == 'o' || change_source == 'O') {
                         S = new sinus(); // Constructeur interactif
                     } else {
                         S = new sinus(1, 0, 1e-7, 0); // Constructeur paramétré
                     }
                     type_source = false; // Sortir de la boucle
                     break;
                case 2: // Triangle
                     cout << "Vous avez choisi la source triangle." << endl;
                     cout << "Defaut -> Amp: 1V, Phase: 0, Periode: 1e-7s" << endl;
                     cout << "\nSouhaitez-vous modifier les proprietes de la source ? (o/n) : ";
                     cin >> change_source;
                     if (change_source == 'o' || change_source == 'O') {
                         S = new triangle();
                     } else {
                         S = new triangle(1, 0, 1e-7, 0);
                     }
                     type_source = false;
                     break;
                case 3: // Créneau (Square)
                     cout << "Vous avez choisi la source creneau." << endl;
                     cout << "Defaut -> Amp: 1V, Phase: 0, Periode: 1e-7s" << endl;
                     cout << "\nSouhaitez-vous modifier les proprietes de la source ? (o/n) : ";
                     cin >> change_source;
                     if (change_source == 'o' || change_source == 'O') {
                         S = new square();
                     } else {
                         S = new square(1, 0, 1e-7, 0.5,0);
                     }
                     type_source = false;
                     break;
                case 4: // Impulsion (Pulse)
                     cout << "Vous avez choisi la source impulsion." << endl;
                     cout << "Defaut -> Amp: 1V, Phase: 0, Duree: 1e-7s" << endl;
                     cout << "\nSouhaitez-vous modifier les proprietes de la source ? (o/n) : ";
                     cin >> change_source;
                     if (change_source == 'o' || change_source == 'O') {
                         S = new pulse();
                     } else {
                         S = new pulse(1, 0, 1e-7,0);
                     }
                     type_source = false;
                     break;
                case 5: // Échelon (Step)
                     cout << "Vous avez choisi la source echelon." << endl;
                     cout << "Defaut -> Amp: 1V, Phase: 0" << endl;
                     cout << "\nSouhaitez-vous modifier les proprietes de la source ? (o/n) : ";
                     cin >> change_source;
                     if (change_source == 'o' || change_source == 'O') {
                         S = new echelon();
                     } else {
                         S = new echelon(1, 0, 0);
                     }
                     type_source = false;
                     break;
                default:
                     cout << "La source n'existe pas, veuillez reessayer." << endl;
                     type_source = true; // Rester dans la boucle
                     break;
            }
        }

            }

        // --- ÉTAPE 5 : Lancement de la simulation ---
        if (C1 != NULL) {
            // Simulation pour circuit d'ordre 1
            simuler_ordre1(C1, S, h, duration, choix_methode);
            delete C1; // Nettoyage de la mémoire du circuit
        } else if (C2 != NULL) {
            // Simulation pour circuit d'ordre 2
            simuler_ordre2(C2, S, h, duration, choix_methode);
            delete C2; // Nettoyage de la mémoire du circuit
        }

        // Nettoyage de la mémoire de la source
        if (S != NULL) {
            delete S;
        }

        // Demande pour recommencer
        cout << "\nNouvelle simulation ? (o/n) : ";
        cin >> continuer;
    }

    cout << "Fin du programme." << endl;
    return 0;
}
