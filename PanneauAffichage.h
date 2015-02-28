/* 
 * File:   PanneauAffichage.h
 * Author: olivier
 *
 * Created on 26 février 2015, 14:17
 */

#ifndef PANNEAUAFFICHAGE_H
#define	PANNEAUAFFICHAGE_H

#include <cstdint>
#include <exception>
#include <vector>
#include "GPIOClass.h"

class PanneauAffichage {
public:
    static const vector<int> numberDPDown; //= {119, 65, 59, 107, 77, 110, 126, 67, 127, 111};
    static const vector<int> numberDPUp;   //= {119, 20, 59, 62, 92, 110, 111, 52, 127, 126};
    
    PanneauAffichage(int nbAfficheurs, int pinOE, int pinLE, int pinData, int pinClk);
    virtual ~PanneauAffichage();
    
    class Erreur : exception {
    public:
        Erreur(string message) {
            this->message = message;
        }
        // Nécessaire car virtuel dans la classe exception
        virtual ~Erreur() throw() {}
        // Implémentation de la méthode 'what' qui affiche un message
        virtual const char* what(void) const throw ()
        {
                return message.c_str();
        }
    private:
        string message;
    };
    
    void init() throw(Erreur);
    void close() throw(Erreur);
    void fadeIn();
    void fadeOut();
    void displayNumber(string number) throw(Erreur);
    void displayNumberWithLeadingZero(string number) throw(Erreur);
    void displayDateTime();
    
private:
    
    
    CGPIO* oe;
    CGPIO* le;
    CGPIO* data;
    CGPIO* clk;
    
    int nbAfficheurs;
    int pinOE, pinLE, pinData, pinClk;
    bool isInitialized;
    
    void sendOneByte(int value);
    void latchValue();
    void outputEnable();
    void outputDisable();

};

#endif	/* PANNEAUAFFICHAGE_H */

