/**
\mainpage Classe de pilotage des Entrées/Sorties Tout Ou Rien multicartes (BBB, RPi, etc...)
\author Olivier DARTOIS
\date Février 2015

Cette classe GPIO permet d'exploiter les broches d'Entrée/Sorties (E/S)
en mode Tout Ou Rien (TOR) des cartes Linux embarqué (BeagleBone, RPi, Olimex)
par le système 'sysfs' géré par le noyau.

En utilisant cette méthode, le code est utilisable sur de nombreuses plateformes
au détriment de la vitesse...
Un test sur un exemple simple qui fait une alternance de 0 et de 1 dans une boucle 
infinie sur une RaspberryPi donne à l'oscilloscope une fréquence d'environ 60KHz
avec une charge processeur de prés de 100%. Pour piloter des systèmes qui ne nécessite
pas une vitesse de transmission élevée cela suffira.

\file GPIOClass.h
Déclaration de la classe CGPIO
\class CGPIO
\brief Cette classe permet de piloter les Entrées/Sorties Tout Ou Rien (E/S TOR)

La plupart des méthodes renvoient un booléen qui
indique si l'action demandée a réussi ou échoué. Il est donc de la responsabilité de
l'utilisateur de cette classe de tester le booléen de retour.
Pour obtenir une description de l'erreur, il faut faire appel à la méthode getLastError().

*/

#ifndef GPIO_CLASS_H
#define GPIO_CLASS_H

#include <cstdint>
#include <string>
#include <fstream>

using namespace std;

class CGPIO
{
public:
	/** \enum CGPIODirection
	* Cette énumération décrit les possibilités de choix de direction d'une broche d'entrée/sortie
	*/
	enum class CGPIODirection : uint8_t {
		IN,		///< Broche d'entrée/sortie en mode entrée 
		OUT		///< Broche d'entrée/sortie en mode sortie
	};

	/** \enum CGPIOValue
	* Cette énumération décrit les possibilités de choix de niveau logique d'une broche d'entrée/sortie
	*/
	enum class CGPIOValue : uint8_t {
		LOW,	///< Fixe le niveau logique bas pour la broche concernée
		HIGH	///< Fixe le niveau logique haut pour la broche concernée
	};

	/**
	* \brief Constructeur de la classe CGPIO
	*
	* Ce constructeur initialise les attributs privé de la classe.
	*
	* \param[in] gNum Numéro de la broche E/S concernée, le numéro est géré par le noyau et dépend de la carte concernée.
	* \param[in] dir Direction de la broche E/S (soit entrée, soit sortie) de type CGPIODirection. Par défaut, la direction est en entrée.
	* \param[in] val Valeur logique pour la broche concernée mise en sortie. Par défaut, la sortie est mise à l'état bas.
	*/
	CGPIO(int gNum, CGPIODirection dir = CGPIODirection::IN, CGPIOValue val = CGPIOValue::LOW);

	/**
	* \brief Méthode init
	*
	* Cette méthode doit être appellé en premier aprés instancié un objet de la classe CGPIO. Cette méthode appelle
	* les méthodes exportGPIO(), fixDirection() puis ouvre un fichier qui permettra de fixer le niveau logique pour une broche
	* mise en sortie ou de lire le niveau logique pour une broche mise en entrée. Enfin si la broche concernée est en sortie
	* fixe le niveau de celle-ci par la méthode fixValue().
	*
	* \return booléen qui indique si la méthode init() a échoué (false) ou réussi (true)
	*/
	bool init();

	/**
	* \brief Méthode close
	*
	* Cette méthode doit être appellé en dernier pour libérer proprement une broche. Cette méthode ferme le fichier
	* qui permettait l'accés à la valeur logique d'une broche puis appelle la méthode unexportGPIO().
	*
	* \return booléen qui indique si la méthode close() a échoué (false) ou réussi (true)
	*/
	bool close();

	/**
	* \brief Méthode fixDirection
	*
	* Cette méthode permet de fixer le sens de la broche soit en entrée (CGPIODirection::IN) ou en sortie 
	* (CGPIODirection::OUT).
	*
	* \param[in] dir Sens de la broche (entrée ou sortie) de type CGPIODirection.
	* \return booléen qui indique si la méthode fixDirection() a échoué (false) ou réussi (true)
	*/
	bool fixDirection(CGPIODirection dir);
	
	/**
	* \brief Méthode fixValue
	*
	* Cette méthode permet de fixer le niveau logique (0:0V ou 1:3,3V) sur une broche mise en sortie. Cette méthode
	* retournera faux si vous l'appelez pour une broche configurée en entrée.
	*
	* \param[in] val Niveau logique souhaité en sortie de la broche de type CGPIOValue::HIGH ou CGPIOValue::LOW.
	* \return booléen qui indique si la méthode fixValue() a échoué (false) ou réussi (true)
	*/
    bool fixValue(CGPIOValue val);
	
	/**
	* \brief Méthode fixHigh
	*
	* Cette méthode permet de fixer le niveau logique de la broche concernée à l'etat HAUT ou 1 logique. 
	* Cette méthode ne vérifie rien pour être le plus rapide possible. Attention donc à ne pas l'utiliser
	* avec  une broche configurée en entrée.
	*/
	void fixHigh();
	
	/**
	* \brief Méthode fixLow
	*
	* Cette méthode permet de fixer le niveau logique de la broche concernée à l'etat BAS ou 0 logique. 
	* Cette méthode ne vérifie rien pour être le plus rapide possible. Attention donc à ne pas l'utiliser
	* avec  une broche configurée en entrée.
	*/
	void fixLow();

	/**
	* \brief Méthode readValue
	*
	* Cette méthode permet de lire le niveau logique présent sur une broche configurée en entrée. 
	*
	* \param[in,out] val Valeur logique présente sur la broche de type CGPIOValue::HIGH pour un niveau logique haut ou 
	* CGPIOValue::LOW pour un niveau logique bas. Attentien ce paramètre est passé par référence pour pouvoir être renvoyé
	* vers la fonction appelante.
	* \return booléen qui indique si la méthode readValue() a échoué (false) ou réussi (true)
	*/
    bool readValue(CGPIOValue& val);
    
	/**
	* \brief Méthode getNum
	*
	* Cette méthode permet de renvoyer le numéro de la broche sur laquelle travaille la classe. Il s'agit d'un ascesseur
	* puisqu'il renvoie la valeur de l'attribut privé gpioNum.
	*
	* \return un entier qui représente le numéro de la broche
	*/
	int getNum() const;

	/**
	* \brief getLastError
	*
	* Cette méthode renvoie le dernier message d'erreur sous forme de chaine de caractères (type string). Il faut
	* l'appeler aprés chaque appel d'une méthode qui renvoie un booléen faux. 
	*
	* \return une chaine de caractère (string) qui contient le message d'erreur
	*/
	string getLastError();

private:
	/// Entier qui représente la broche concernée
	int gpioNum;  
	/// Chaine qui contient le chemin pour fixer le sens (la direction) de la broche concernée (généralement /sys/class/gpio/gpioX/direction)
	string dirPath;
	/// Chaine qui contient le chemin pour fixer la valeur (le niveau logique) de la broche concernée (généralement /sys/class/gpio/gpioX/value)
	string valPath;
	/// la direction (le sens) de la broche concernée, donc soit entrée, soit sortie. Attention cette variable est du
	/// type CGPIODirection
	CGPIODirection direction;
	/// la valeur actuelle de la broche : généralement utilisé lors d'une lecture. Attention cette variable est 
	/// du type CGPIOValue
	CGPIOValue value;
	/// cette chaine contient le dernier message d'erreur
	string error;
	/// La variable qui contient l'identifiant du pseudo fichier qui permet de piloter la broche. Comme une broche
	/// peut être en entrée ou en sortie, cette variable est du type fstream (permet de lire ou d'écrire dans ce fichier)
	fstream readWriteValue;
	
	/**
	* \brief exportGPIO
	*
	* Cette méthode permet d'exporter la broche concernée au niveau du pseudo système de fichiers sysfs.
	*
	* \return un booléen qui indique si l'exportation a fonctionné (true) ou non (false)
	*/
	bool exportGPIO();

	/**
	* \brief unexportGPIO
	*
	* Cette méthode permet d'enlever la broche concernée au niveau du pseudo système de fichiers sysfs.
	*
	* \return un booléen qui indique si le retrait de la broche a fonctionné (true) ou non (false)
	*/
	bool unexportGPIO();
	
};

#endif
