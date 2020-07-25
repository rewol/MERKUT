#include "Altitude.h"
#include<math.h>
#include<iostream>

Altitude::Altitude(double h)
{
	height = h; // Feet !
	rho = rhoSeaLevel * exp(-0.0000297 * height); // slugs/ft3
	p = pSeaLevel * pow(1 - 6.876e-6 * height, 5.265);
	temp = (15 - 0.001981 * height) + 273; // Outputs temperature in Kelvin.
	sigma = rho / rhoSeaLevel;
	sigmap = p / pSeaLevel;
	palt = (518.4 / 0.00357) * (1 - pow(sigmap, 0.1903));
	a = 3.28 * pow(1.4 * 287 * temp, 0.5);
	teta = temp / tSeaLevel;
}

Altitude::Altitude(double h, double T)
{
	height = h;
	tSeaLevel = 15 + T + 273.16;
	p = pSeaLevel * pow(1 - 6.876e-6 * height, 5.265);
	sigmap = p / pSeaLevel;
	palt = (518.4 / 0.00357) * (1 - pow(sigmap, 0.1903));
	sigma = (288.16 / (T + 273.16)) * pow((1 - (0.001981 * palt) / 288.16), 5.256);
	//rho = rhoSeaLevel * sigma;
	temp = ((15 + T) - 0.001981 * height) + 273; // in Kelvin
	teta = temp / tSeaLevel;
	a = 3.28 * pow(1.4 * 287 * temp, 0.5);

	rhoSeaLevel = pSeaLevel / (1716.49 * tSeaLevel * 1.8); // dry air
	rho = rhoSeaLevel * sigma;
}

void Altitude::setAltitude(double h)
{
	height	= h; // Feet !
	p = pSeaLevel * pow(1 - 6.876e-6 * height, 5.265);
	rho		= rhoSeaLevel * exp(-0.0000297 * height); // slugs/ft3
	temp	= (15 - 0.001981 * height) + 273; // Outputs temperature in Kelvin.
	sigma	= rho / rhoSeaLevel;
	sigmap = p / pSeaLevel;
	a = 3.28 * pow(1.4 * 287 * temp, 0.5);
	teta = temp / tSeaLevel;
	palt = (518.4 / 0.00357) * (1 - pow(sigmap, 0.1903));
}

void Altitude::setAltitude(double h, double T)
{
	height = h;
	tSeaLevel = 15 + T + 273.16;
	p = pSeaLevel * pow(1 - 6.876e-6 * height, 5.265);
	sigmap = p / pSeaLevel;
	palt = (518.4 / 0.00357) * (1 - pow(sigmap, 0.1903));
	sigma = (288.16 / (T + 273.16)) * pow((1 - (0.001981 * palt) / 288.16), 5.256);
	//rho = rhoSeaLevel * sigma;
	temp = ((15 + T) - 0.001981 * height) + 273; // in Kelvin
	teta = temp / tSeaLevel;
	a = 3.28 * pow(1.4 * 287 * temp, 0.5);
	rhoSeaLevel = pSeaLevel / (1716.49 * tSeaLevel * 1.8); // dry air
	rho = rhoSeaLevel * sigma;
}

double Altitude::getrho() const
{
	return rho;
}

double Altitude::getSOS() const
{
	return a;
}

double Altitude::getSigmap() const
{
	return sigmap;
}

double Altitude::getTeta() const
{
	return teta;
}

