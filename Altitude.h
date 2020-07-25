#ifndef ALTITUDE
#define ALTITUDE

class Altitude
{
friend class Rotorcraft;
friend class Performance;
friend class Selection;
friend class Coaxial;
friend class Compound;

private:
	double height;
	double p;
	double rho;
	double temp;
	double sigma;
	double a;
	double rhoSeaLevel = 0.002378;
	double pSeaLevel = 2116.4;
	double tSeaLevel = 288;
	double sigmap;
	double teta;
	double palt;
public:
	Altitude(double); // Constructor
	Altitude(double, double); // NOT ISA Constructor
	void setAltitude(double); // Input is in feet !
	void setAltitude(double, double); // h is in feet, T 
	double getrho() const;
	double getSOS() const;
	double getSigmap() const;
	double getTeta() const;
};

#endif