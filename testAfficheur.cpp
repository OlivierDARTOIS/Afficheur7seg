#include <iostream>
#include <cstdint>
#include <vector>

#include "PanneauAffichage.h"

using namespace std;

int main() {
        // Constructeur du panneau, les arguments dans l'ordre sont
        // Nb d'afficheurs du panneau
        // broche pour le signal OE (par défaut 18)
        // broche pour le signal LE (par défaut 22)
        // broche pour le signal Data (par défaut 10)
        // broche pour le signal Clk (par défaut 11)
        PanneauAffichage monPanneau(2, 18, 22, 10, 11);
        
        try {
            monPanneau.init();
        }
        catch (PanneauAffichage::Erreur& e) {
            cerr << e.what() << endl;
            monPanneau.close();
            exit (-1);
        }
        
        try {
        monPanneau.displayNumberWithLeadingZero("99");
        }
        catch (PanneauAffichage::Erreur& e) {
            cerr << e.what() << endl;
        }
        
        monPanneau.fadeIn();
        monPanneau.fadeOut();
        
        for (int i=0; i<10; i++) {
                monPanneau.displayDateTime();
                sleep(5);
        }
        
        try {
                monPanneau.close();
        }
        catch (PanneauAffichage::Erreur& e) {
            cerr << e.what() << endl;
            //exit (-1);
        }
        
        cout << "Ok" << endl;
	return 0;
}

