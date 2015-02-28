/**
\file GPIOClass.cpp

\brief Implémentation de la classe CGPIO pour piloter les Entrées/Sorties Tout Ou Rien (E/S TOR)
*/
#include <fstream>
#include <string>
#include <iostream>

#include "GPIOClass.h"

using namespace std;

CGPIO::CGPIO(int gNum, CGPIODirection dir, CGPIOValue val)
{
	this->gpioNum = gNum;
	this->direction = dir;
	this->value = val;
	this->dirPath = "/sys/class/gpio/gpio" + to_string(this->gpioNum) + "/direction";
	this->valPath = "/sys/class/gpio/gpio" + to_string(this->gpioNum) + "/value";
}

bool CGPIO::init()
{
        if (this->gpioNum < 0) {
                error = "OPERATION FAILED: A GPIO cant have a negative value";
                return false;
        }
    
	if (!exportGPIO())
		return false;

	if (!fixDirection(this->direction)) {
		unexportGPIO();
		return false;
	}

	readWriteValue.open(valPath.c_str()); 
	if (!readWriteValue.is_open()){
		unexportGPIO();
		error = "OPERATION FAILED: Unable to set the value of GPIO " + to_string(this->gpioNum) +
			    "\nMaybe you need to be root !\n";
		return false;
	}

	if (this->direction == CGPIODirection::OUT) fixValue(this->value);

	return true;
}

bool CGPIO::close()
{
        // Fermeture du fichier 'value' de la broche
	if (readWriteValue.is_open())
		readWriteValue.close();// close value file

        // Remise en entréee de la broche concernée (config d'origine)
        if (!fixDirection(CGPIODirection::IN))
            return false;
        
        // Suppression de la broche au niveau du sysfs
	if (!unexportGPIO())
		return false;
	return true;
}

bool CGPIO::fixDirection(CGPIODirection dir)
{
	ofstream setdirgpio(dirPath.c_str()); // open direction file for gpio
	if (!setdirgpio.is_open()){
		error =  "OPERATION FAILED: Unable to set direction of GPIO " + to_string(this->gpioNum) +
			     "\nMaybe you need to be root !\n";
		return false;
	}

	if (dir == CGPIODirection::IN)
	{
		setdirgpio << "in";
		this->direction = CGPIODirection::IN;
	}
	else
	{
		setdirgpio << "out";
		this->direction = CGPIODirection::OUT;
	}
	
	setdirgpio.close(); // close direction file
	return true;
}


bool CGPIO::fixValue(CGPIOValue val)
{
	if (this->direction == CGPIODirection::IN) {
		error = "OPERATION FAILED: Unable to write on input GPIO " + to_string(this->gpioNum);
		return false;
	}
	else
		(val == CGPIOValue::LOW) ? readWriteValue << "0" : readWriteValue << "1"; //write value to value file
	
	readWriteValue.flush();
	return true;
}

void CGPIO::fixHigh()
{
	readWriteValue << "1";
	readWriteValue.flush();
}

void CGPIO::fixLow()
{
	readWriteValue << "0";
	readWriteValue.flush();
}

bool CGPIO::readValue(CGPIOValue& val)
{
	string temp;

	if (this->direction == CGPIODirection::OUT) {
		error = "OPERATION FAILED: Unable to read on output GPIO " + to_string(this->gpioNum);
		return false;
	}
	else
	{
		readWriteValue >> temp;  //read gpio value
		if (temp == "0")
			val = CGPIOValue::LOW;
		else
			val = CGPIOValue::HIGH;
		return true;
	}
}


int CGPIO::getNum() const {

	return this->gpioNum;

}

string CGPIO::getLastError()
{
	string temp = this->error;
	this->error = "No error \n";
	return temp;
}

bool CGPIO::exportGPIO()
{
	string exportPath = "/sys/class/gpio/export";
	ofstream exportgpio(exportPath.c_str()); // Open "export" file. Convert C++ string to C string. Required for all Linux pathnames

	if (!exportgpio.is_open()){
		error = "OPERATION FAILED: Unable to export GPIO " + to_string(this->gpioNum) +
			"\nMaybe you need to be root !\n";
		return false;
	}

	exportgpio << to_string(this->gpioNum); //write GPIO number to export
	exportgpio.close(); //close export file
	return true;
}

bool CGPIO::unexportGPIO()
{
	string unexportPath = "/sys/class/gpio/unexport";
	ofstream unexportgpio(unexportPath.c_str()); //Open unexport file
	if (!unexportgpio.is_open()){
		error = "OPERATION FAILED: Unable to unexport GPIO " + to_string(this->gpioNum) +
			"\nMaybe you need to be root !\n";
		return false;
	}

	unexportgpio << to_string(this->gpioNum); //write GPIO number to unexport
	unexportgpio.close(); //close unexport file
	return true;
}