/* 
 * File:   PanneauAffichage.cpp
 * Author: olivier
 * 
 * Created on 26 février 2015, 14:17
 */
#include <iostream>
#include <exception>
#include <ctime>
#include "PanneauAffichage.h"

const vector<int> PanneauAffichage::numberDPDown= {119, 65, 59, 107, 77, 110, 126, 67, 127, 111};
const vector<int> PanneauAffichage::numberDPUp= {119, 20, 59, 62, 92, 110, 111, 52, 127, 126};

PanneauAffichage::PanneauAffichage(int nbAfficheurs, int pinOE, int pinLE, int pinData, int pinClk) {
    this->nbAfficheurs = nbAfficheurs;
    this->isInitialized = false;
    this->oe = nullptr;
    this->le = nullptr;
    this->data = nullptr;
    this->clk = nullptr;
    this->pinOE = pinOE;
    this->pinLE = pinLE;
    this->pinData = pinData;
    this->pinClk = pinClk;
}

PanneauAffichage::~PanneauAffichage() {
    // On peut appeler un delete sur un pointeur nul, dans ce cas
    // le delete ne fait rien
    delete this->oe;
    delete this->le;
    delete this->data;
    delete this->clk;
}

void PanneauAffichage::init() throw (PanneauAffichage::Erreur) {
    if (this->isInitialized)
        return;
    
    if (this->nbAfficheurs < 1) {
        throw (Erreur("Le nombre d\'afficheur doit être supérieur ou égale à 1"));
        return;
    }
    
    if (this->pinOE<0 || this->pinLE<0 || this->pinData<0 || this->pinClk<0) {
        throw (Erreur("Une broche ne peut avoir une valeur negative"));
        return;
    }
    
    if (this->pinOE == this->pinLE || this->pinOE == this->pinData || this->pinOE == this->pinClk ||
        this->pinLE == this->pinData || this->pinLE == this->pinClk || this->pinData == this->pinClk) {
        throw (Erreur("Les numeros de broche doivent être tous différents"));
        return;
    }
    
    this->oe   = new CGPIO(this->pinOE, CGPIO::CGPIODirection::OUT, CGPIO::CGPIOValue::HIGH);
    this->le   = new CGPIO(this->pinLE, CGPIO::CGPIODirection::OUT, CGPIO::CGPIOValue::LOW);
    this->data = new CGPIO(this->pinData, CGPIO::CGPIODirection::OUT, CGPIO::CGPIOValue::LOW);
    this->clk  = new CGPIO(this->pinClk, CGPIO::CGPIODirection::OUT, CGPIO::CGPIOValue::LOW);
    
    if (!this->oe->init()) {
        throw (Erreur(this->oe->getLastError()));
        return;
    }
    
    if (!this->le->init()) {
        throw (Erreur(this->le->getLastError()));
        return;
    }
    
    if (!this->data->init()) {
        throw (Erreur(this->data->getLastError()));
        return;
    }
    
    if (!this->clk->init()) {
        throw (Erreur(this->clk->getLastError()));
        return;
    }
    
    this->isInitialized = true;
}

void PanneauAffichage::close() throw(PanneauAffichage::Erreur) {
    /*
    if (!this->isInitialized) {
        throw (Erreur("Le panneau ne peut être fermé car il n\'a pas été initialisé"));
        return;
    }
    */
    if (!this->oe->close()) {
        throw (Erreur(this->oe->getLastError()));
        return;    
    }
    
    if (!this->le->close()) {
        throw (Erreur(this->le->getLastError()));
        return;    
    }
    
    if (!this->data->close()) {
        throw (Erreur(this->data->getLastError()));
        return;    
    }
    
    if (!this->clk->close()) {
        throw (Erreur(this->clk->getLastError()));
        return;    
    }
}

void PanneauAffichage::displayNumber(string number) throw(PanneauAffichage::Erreur) {
    if (number.size() > this->nbAfficheurs) {
        throw (Erreur("Nombre trop grand pour être affiché"));
        return;
    }
    
    if (number.size() == 0) {
        throw (Erreur("Le nombre a affiché ne peut être vide"));
        return;
    }
    
    for (int i=0; i<number.size(); i++) {
        if (number.at(i) < '0' || number.at(i) > '9') {
            throw (Erreur("Affichage des chiffres 0 à 9 uniquement"));
            return;
        }
    }
    
    outputDisable();
    
    // Si le nombre a affiché est plus petit que le nb d'fficheurs
    // on "éteint" les afficheurs concernés
    if (number.size() < this->nbAfficheurs) {
        for (int i=0; i<(this->nbAfficheurs - number.size()); i++)
            sendOneByte(0);
    }
    
    for (int i=0; i<number.size(); i++) {
        sendOneByte(numberDPDown.at(number.at(i) - '0'));
    }
    
    latchValue();
    //outputEnable();
}

void PanneauAffichage::displayNumberWithLeadingZero(string number) throw(PanneauAffichage::Erreur) {
    // Si le nombre a affiché est plus petit que le nb d'fficheurs
    // on rajoute des zeros devant
    string temp;
    if (number.size() < this->nbAfficheurs) {
        for (int i=0; i<(this->nbAfficheurs - number.size()); i++)
            temp +="0";
    }
    number = temp + number;
    displayNumber(number);
}

void PanneauAffichage::displayDateTime() {
        
    std::time_t result = std::time(NULL);
    char mbstr[30];
    std::strftime(mbstr, sizeof(mbstr), "%d%m%y%H%M%S", std::localtime(&result));
    string dateheure(mbstr);         
        
    string jour = dateheure.substr(0,2);
    string mois = dateheure.substr(2,2);
    string annee= dateheure.substr(4,2);
    string heure = dateheure.substr(6,2);
    string min = dateheure.substr(8,2);
          
    displayNumber(jour);
    fadeIn();
    fadeOut();
    displayNumber(mois);
    fadeIn();
    fadeOut();
    displayNumber(annee);
    fadeIn();
    fadeOut();
       
    sleep(1);
        
    displayNumber(heure);
    fadeIn();
    fadeOut();    
    displayNumber(min);
    fadeIn();
    fadeOut();
}

void PanneauAffichage::fadeIn() {
        for (int j=0; j<100; j++) {
                oe->fixLow();
		usleep(100*j);
		oe->fixHigh();
		usleep(10000-100*j);	
	}
	oe->fixLow();
}

void PanneauAffichage::fadeOut() {
        for (int j=0; j<100; j++) {
                oe->fixHigh();
                usleep(100*j);
                oe->fixLow();
                usleep(10000-100*j);
	}
	oe->fixHigh();
}

void PanneauAffichage::sendOneByte(int value) {
    value = value & 0xFF;
    for (int i=0; i<8; i++) {
        if ((value&0x01) == 1)
            data->fixHigh();
        else
            data->fixLow();
        clk->fixHigh();
	clk->fixLow();
        value = value >> 1;
    }
}

void PanneauAffichage::latchValue() {
    le->fixHigh();
    le->fixLow();
}

void PanneauAffichage::outputEnable() {
    oe->fixLow();
}

void PanneauAffichage::outputDisable() {
    oe->fixHigh();
}