#include "Compound.h"

Compound::Compound(double gross, double payload, double crew) : wgrossguess(gross)
{
	
	segment = 0;
	Wgross = gross;
	Wpayload = payload;
	Wcrew = crew;
	Altitude height(11500);		// Forward flight altitude, for first iteration
	alt = &height;
	a = alt->getSOS();			// Speed of sound is set for first iteration
	density = alt->getrho();	// Density is set for first iteration
	iterate();					// Calculates main physical parameters based on Wgross, a and density
	Preq = 0;					// Sets first power req
	WfuelReq = 0;               // Initialize required fuel
	
}

void Compound::emptyWeight()
{
	// PROUTY EQUATIONS
	/*double g = 32.2;
	double j = 3.14 * pow(radius, 4) / 2;
	double Lf = 13;
	double Swet = 680;
	double nol = 3;
	double transmission = 4000;
	double rpm = 20000;
	Wrotor = 0.026 * pow(nob, 0.66) * chord * pow(radius, 1.3) * pow(radius * maxAngSpeed, 0.67);
	Whub = 0.0037 * pow(nob, 0.28) * pow(radius, 1.5) * pow(radius * maxAngSpeed, 0.43) * (0.67 * Wrotor + g * j / (radius * radius), 0.55);
	Wfuselage = 6.9 * pow(Wgross / 1000, 0.49) * Lf * pow(Swet, 0.25);
	Wldg = 40 * pow(Wgross / 1000, 0.67) * pow(nol, 0.54);
	Wpusher = 200;
	Wdrive = 13.6 * pow(transmission, 0.82) * pow(rpm / 1000, 0.037);
	Wavionic = 200;
	Wequipment = 23 * (Wgross / 1000, 1.3);
	Wengine = 650;*/
	// Calculate Empty Weight
	Wempty = Wgross * 0.45;
	//Wempty = Wrotor + Whub + Wfuselage + Wldg + Wpusher + Wdrive + Wavionic + Wequipment + Wengine;
	Wfuel = Wgross - Wempty - Wpayload - Wcrew;

}

void Compound::setHeight(double h)
{
	// Changes density and speed of sound as per given altitude
	alt->setAltitude(h);
	a = alt->getSOS();
	density = alt->getrho();
	maxTipSpeed = a * maxTipMach;
	maxAngSpeed = maxTipSpeed / radius;
}

void Compound::setHeightNS(double h, double T)
{
	// Changes density and speed of sound as per given altitude and offset SL temp
	alt->setAltitude(h, T);
	a = alt->getSOS();
	density = alt->getrho();
	maxTipSpeed = a * maxTipMach;
	maxAngSpeed = maxTipSpeed / radius;
}

void Compound::iterate()
{
	T					= Wgross / 2;
	radius				= pow(T / (2 * 3.14 * DL), 0.5);
	maxTipSpeed			= a * maxTipMach;
	maxAngSpeed			= maxTipSpeed / radius;
	rotorArea			= 3.14 * pow(radius, 2);
	coArea				= 2 * rotorArea;
	C_T					= Wgross / (coArea * density * pow(maxTipSpeed, 2));//10.4028/www.scientific.net/AMM.826.40 EQ-6
	solidity			= C_T / BL; // Leishman, Eq 2-62
	chord				= (solidity * 3.14 * radius) / (2 * nob); // Leishman Eq 2-61
	AR					= radius / chord;
	mean_C_L			= (6 * C_T / solidity); //  Leishman 3-160
	aoa					= mean_C_L / cla;		// Leishman 3-160
	mean_C_D			= 0.009 + 0.3 * aoa * aoa; // FOR NACA 63-15 family, GIT Thesis Reference 8 Eq 3-13
}

void Compound::compressibleLoss()
{
		// Johnson Model
	double Mdd = 0.6; // Buna airfoil drag divergence Mach sayýsý yazýlacak
	double dMdd = maxTipMach - Mdd;
	if (dMdd >= 0) { dcpo = solidity * (0.007 * dMdd + 0.052 * pow(dMdd, 2)); } // Leishman Eq 5-30
	else { dcpo = 0; }

		// Harris Model
		/*double dcpo;
		double Mdd; // Buna airfoil drag divergence Mach sayýsý yazýlacak
		double tc; // Buna airfoil' in thickness / chord oraný yazýlacak
		double dMdd = maxTipMach - Mdd;
		double Mtilt = (pow(maxTipMach, 2) - 1) / (1.79 * pow(maxTipMach, 1.333) * pow(tc, 0.666));
		if (dMdd >= 0) { dcpo = 0.3 * pow(1 + advRatio, 2.5) * pow(tc, 2.5) * (Mtilt + 1); }
		else { dcpo = 0; }*/
}

void Compound::maxEnduranceSpeed()
{
	// Leishman Eq 5-71
	T = Wgross / 2;
	VmaxEndurance = pow(T / (2 * density * coArea), 0.5) * pow(4 * K / (3 * f / rotorArea), 0.25);
}

void Compound::maxRangeSpeed()
{
	// Leishman Eq 5-76
	T = Wgross / 2;
	VmaxRange= pow(T / (2 * density * coArea), 0.5) * pow(4 * K / (f / rotorArea), 0.25);
}

void Compound::climbPerformance(double P)
{
	double Vc;
	Vc = (Pinstalled - P)*550 / Wgross;
	if (Vc < forwardMaxClimb)
	{
		climb = true;
	}
}

void Compound::requiredPower(double P)
{
	if (P >= Preq) 
	{ 
		Preq = P;
		Pmaximum = (1 / eff) * (Preq + Paccess);
		if (Pmaximum >= Pinstalled)
		{
			power = true;
		}
	}
}

void Compound::requiredFuel(double wf)
{
	WfuelReq = WfuelReq + wf;
	if (WfuelReq >= Wtank)
	{
		fuel = true;
	}
}

void Compound::segmentData(double w, double p)
{
	if (segment == 8) { segment = 0; }
	segment = segment + 1;
	cout << "MISSION SEGMENT " << segment << " :" << endl;
	cout << "Fuel spent on Mission Segment No: " << segment << " is: " << w << endl;
	cout << "Pow needed on Mission Segment No: " << segment << " is: " << p << endl;
	cout << "------------------------------------------------------------------------" << endl;
	cout << endl;
	cout << endl;
}

void Compound::helicopterData()
{
	cout << "Helicopter Properties" << endl;
	cout << "---------------------" << endl;
	cout << left << setw(15) << "Radius" << setw(15) << radius << setw(15) << "ft" << endl;
	cout << left << setw(15) << "Chord" << setw(15) << chord << setw(15) << "ft" << endl;
	cout << left << setw(15) << "Solidity" << setw(15) << solidity << setw(15) <<  endl;
	cout << left << setw(15) << "Aspect Ratio" << setw(15) << AR << setw(15) << endl;
	cout << left << setw(15) << "Mean CL" << setw(15) << mean_C_L << setw(15) << endl;
	cout << endl;
	cout << "Power Requirements" << endl;
	cout << "---------------------" << endl;
	cout << left << setw(15) << "Max Power Req." << setw(15) << Pmaximum << setw(15) << "hp" << endl;
	cout << endl;
	cout << "Weight Distribution" << endl;
	cout << "---------------------" << endl;
	cout << left << setw(15) << "Wgross" << setw(15) << Wgross << setw(15) << "lbs" << endl;
	cout << left << setw(15) << "Wempty" << setw(15) << Wempty << setw(15) << "lbs" << endl;
	cout << left << setw(15) << "Wfuel" << setw(15) << Wfuel << setw(15) << "lbs" << endl;
	cout << left << setw(15) << "Wpayload" << setw(15) << Wpayload << setw(15) << "lbs" << endl;
	cout << left << setw(15) << "Wcrew" << setw(15) << Wcrew << setw(15) << "lbs" << endl;
	cout << endl;
	cout << "Re-Design Needs (Empty if none req.)" << endl;
	cout << "---------------------" << endl;
	if (climb == true)
	{
		cout << "Climb power velocity cannot be achieved" << endl;
	}
	if (power == true)
	{
		cout << "Maximum power required is more than installed Power" << endl;
	}
	if (fuel == true)
	{
		cout << "Fuel tank is not sufficient for required fuel in mission" << endl;
	}
	cout << endl << endl;
}

void Compound::recordData()
{
	fstream file;
	string filename = "main.txt";
	file.open(filename, fstream::out);
	file << "Helicopter Properties" << endl;
	file << "---------------------" << endl;
	file << left << setw(15) << "Radius" << setw(15) << radius << setw(15) << "ft" << endl;
	file << left << setw(15) << "Chord" << setw(15) << chord << setw(15) << "ft" << endl;
	file << left << setw(15) << "Solidity" << setw(15) << solidity << setw(15) << endl;
	file << left << setw(15) << "Aspect Ratio" << setw(15) << AR << setw(15) << endl;
	file << left << setw(15) << "Mean CL" << setw(15) << mean_C_L << setw(15) << endl;
	file << endl;
	file << "Power Requirements" << endl;
	file << "---------------------" << endl;
	file << left << setw(15) << "Max Power Req." << setw(15) << Pmaximum << setw(15) << "hp" << endl;
	file << endl;
	file << "Weight Distribution" << endl;
	file << "---------------------" << endl;
	file << left << setw(15) << "Wgross" << setw(15) << Wgross << setw(15) << "lbs" << endl;
	file << left << setw(15) << "Wempty" << setw(15) << Wempty << setw(15) << "lbs" << endl;
	file << left << setw(15) << "Wfuel" << setw(15) << Wfuel << setw(15) << "lbs" << endl;
	file << left << setw(15) << "Wpayload" << setw(15) << Wpayload << setw(15) << "lbs" << endl;
	file << left << setw(15) << "Wcrew" << setw(15) << Wcrew << setw(15) << "lbs" << endl;
	file << endl;
	file.close();
}

void Compound::recordMissionData()
{
	fstream file;
	string name;
	cout << endl;
	cout << "ENTER MISSION NAME: ";
	cin >> name;
	name = name + ".txt";
	file.open(name, fstream::out);
	file << "Helicopter Properties" << endl;
	file << "---------------------" << endl;
	file << left << setw(15) << "Radius" << setw(15) << radius << setw(15) << "ft" << endl;
	file << left << setw(15) << "Chord" << setw(15) << chord << setw(15) << "ft" << endl;
	file << left << setw(15) << "Solidity" << setw(15) << solidity << setw(15) << endl;
	file << left << setw(15) << "Aspect Ratio" << setw(15) << AR << setw(15) << endl;
	file << left << setw(15) << "Mean CL" << setw(15) << mean_C_L << setw(15) << endl;
	file << endl;
	file << "Power Requirements" << endl;
	file << "---------------------" << endl;
	file << left << setw(15) << "Max Power Req." << setw(15) << Pmaximum << setw(15) << "hp" << endl;
	file << endl;
	file << "Mission Stats" << endl;
	file << "Wgross after mission is " << Wgross << " lbs" << endl;
	file << "Fuel left after mission is: " << Wfuel << " lbs" << endl;
	file << "Max power required in mission is " << Pmaximum << " hp" << endl;
	file.close();
}

void Compound::hoverPower(double time)
{
	// 10.4028/www.scientific.net/AMM.826.40 Coaxial Equations
	// Leishman Eq 5-13
	T = Wgross / 2;
	C_T = Wgross / (coArea * density * pow(maxTipSpeed, 2));
	double B = 1 - pow(2 * C_T / (2 * nob), 0.5);
	Pih		= B * kint * pow(T, 1.5) / pow(2 * coArea * density, 0.5);
	//Pih			= 2 * B * kint * pow(T, 1.5) / pow(2 * coArea * density, 0.5);
	Poh			= 0.125 * Cd0 * density * coArea * solidity * pow(maxTipSpeed, 3);
	//Poh = 0.125 * Cd0 * density * rotorArea * solidity * pow(maxTipSpeed, 3);
	Phover		= (Pih + Poh) / 550;
	requiredPower(Phover);
	double Wspent = sfc * Phover * time;
	requiredFuel(Wspent);
	wff = Wspent;
	//segmentData(Wspent, Phover);
}

void Compound::forwardPower(double time, double vel)
{
	// Leishman Eq 5-17, 2-161, 2-162
	// Seddon Eq 7-9
	T = Wgross / 2;
	advRatio = vel / maxTipSpeed;
	//Piw = 2 * k * kint * pow(T, 2) / (2 * density * coArea * vel);
	Piw = k * kint * pow(T, 2) / (2 * density * coArea * vel);
	double Cpo = (0.125 * Cd0 * solidity * (1 + K * pow(advRatio, 2)));
	compressibleLoss();
	Cpo = Cpo + dcpo;
	Pow = density * coArea * pow(maxTipSpeed, 3) * Cpo;
	//Pow = density * rotorArea * pow(maxTipSpeed, 3) * Cpo;
	Ppw = 0.5 * f * density * pow(vel, 3);
	Pforward = (Piw + Pow + Ppw) / 550;
	//cout << Ppw/550 << endl;
	requiredPower(Pforward);
	double Wspent = time * Pforward * sfc;
	requiredFuel(Wspent);
	wff = Wspent;
	//segmentData(Wspent, Pforward);
}

void Compound::forwardPowerClimb(double time, double vel)
{
	T = Wgross / 2;
	advRatio = vel / maxTipSpeed;
	Piw = k * kint * pow(T, 2) / (2 * density * coArea * vel);
	//Piw = 2 * k * kint * pow(T, 2) / (2 * density * coArea * vel);
	double Cpo = (0.125 * Cd0 * solidity * (1 + K * pow(advRatio, 2)));
	compressibleLoss();
	Cpo = Cpo + dcpo;
	Pow = density * coArea * pow(maxTipSpeed, 3) * Cpo;
	//Pow = density * rotorArea * pow(maxTipSpeed, 3) * Cpo;
	Ppw = 0.5 * f * density * pow(vel, 3);
	Pforward = (Piw + Pow + Ppw) / 550;
	climbPerformance(Pforward);
	PforwardClimb = (forwardMaxClimb * Wgross)/550;
	requiredPower(Pforward + PforwardClimb);
	double Wspent = time * (Pforward + PforwardClimb) * sfc;
	requiredFuel(Wspent);
	wff = Wspent;
	//segmentData(Wspent, Pforward + PforwardClimb);
}

void Compound::forwardPowerDescend(double time, double vel)
{
	T = Wgross / 2;
	advRatio = vel / maxTipSpeed;
	Piw = k * kint * pow(T, 2) / (2 * density * coArea * vel);
	//Piw = 2 * k * kint * pow(T, 2) / (2 * density * coArea * vel);
	double Cpo = (0.125 * Cd0 * solidity * (1 + K * pow(advRatio, 2)));
	compressibleLoss();
	Cpo = Cpo + dcpo;
	Pow = density * coArea * pow(maxTipSpeed, 3) * Cpo;
	//Pow = density * rotorArea * pow(maxTipSpeed, 3) * Cpo;
	Ppw = 0.5 * f * density * pow(vel, 3);
	Pforward = (Piw + Pow + Ppw) / 550;
	PforwardDescend = (forwardMaxClimb * Wgross)/550;
	requiredPower(Pforward + PforwardDescend);
	double Wspent = time * (Pforward - PforwardDescend) * sfc;
	requiredFuel(Wspent);
	wff = Wspent;
	//segmentData(Wspent, Pforward - PforwardDescend);
}

void Compound::rfmethod()
{
	// GIT Thesis Reference 7, Chapter 4, Eq 4-1, 4-2
	Altitude height(4922);
	alt = &height;	
	double margin = 100;
	double iter = 1;
	while (margin > 10)
	{
		climb = false;
		fuel = false;
		power = false;
		WfuelReq = 0;
		Preq = 0;
		Pmaximum = 0;

		//cout << "ITERATION NO: " << iter << endl;
		//cout << left << "- - - - - - - - - - - " << endl;
		// Constructor ile ilk deðerler hesaplandý
		emptyWeight(); // Wepmty ve Wfuel available hesaplanýr

		// REQ FUEL HESAPLAMA
		// 1. Segment
		setHeight(4922);
		hoverPower(0.0833);
		// 2. Segment
		setHeight(8202);
		maxEnduranceSpeed();
		forwardPowerClimb(0.133, VmaxEndurance);
		// 3. Segment
		setHeight(11483);
		forwardPower(0.856, Vmax);
		// 4. Segment
		maxEnduranceSpeed();
		forwardPower(0.5, VmaxEndurance);
		// 5. Segment
		hoverPower(0.0833);
		// 6. Segment
		forwardPower(0.856, Vmax);
		// 7. Segment
		setHeight(8202);
		maxEnduranceSpeed();
		forwardPowerDescend(0.133, VmaxEndurance);
		// 8. Segment
		setHeight(4922);
		hoverPower(0.0833);

		margin = fabs(Wfuel - WfuelReq);				// Check fuel difference
		Wgross = WfuelReq + Wempty + Wpayload + Wcrew;	// Set new gross weight
		setHeight(11483);								// Set altitude at which properties are calculated
		iterate();										// Calculate new properties
		iter = iter + 1;								// Increase iteration number
	}
	helicopterData();
	recordData();
	
}

void Compound::revisedRfMethod()
{
	// GIT Thesis Reference 7, Chapter 4, Eq 4-1, 4-2, with mission segmen fuel modification
	Altitude height(4922);
	alt = &height;
	double margin = 100;
	double iter = 1;

	while (margin > 10)
	{
		climb = false;
		fuel = false;
		power = false;
		WfuelReq = 0;
		Preq = 0;
		Pmaximum = 0;
		emptyWeight();			// Set Empty Weight

		// REQ FUEL HESAPLAMA
		// 1. Segment
		setHeight(4922);
		hoverPower(0.0833);
		Wgross = Wgross - wff;
		// 2. Segment
		setHeight(8202);
		maxEnduranceSpeed();
		forwardPowerClimb(0.133, VmaxEndurance);
		Wgross = Wgross - wff;
		// 3. Segment *************
		setHeight(11483);
		forwardPower(1, Vmax);
		Wgross = Wgross - wff;
		// 4. Segment
		maxEnduranceSpeed();
		forwardPower(0.5, VmaxEndurance);
		Wgross = Wgross - wff;
		// 5. Segment
		hoverPower(0.0833);
		Wgross = Wgross - wff - Wpayload;		
		// 6. Segment ***************
		forwardPower(1, Vmax);
		Wgross = Wgross - wff;
		// 7. Segment
		setHeight(8202);
		maxEnduranceSpeed();
		forwardPowerDescend(0.133, VmaxEndurance);
		Wgross = Wgross - wff;
		// 8. Segment
		setHeight(4922);
		hoverPower(0.0833);
		Wgross = Wgross - wff;

		margin = fabs(Wfuel - WfuelReq);				// Check fuel difference
		Wgross = WfuelReq + Wempty + Wpayload + Wcrew;	// Set new gross weight
		setHeight(11483);								// Set altitude at which properties are calculated
		iterate();										// Calculate new properties
		iter = iter + 1;								// Increase iteration number
	}
	recordData();
	helicopterData();
	recordweight();
}

void Compound::reset()
{
	Wgross = wgrossguess;
	DL = 20;
	BL = 0.1;
	nob = 4;
	Pinstalled = 4000;
	Wtank = 3000;
	maxTipMach = 0.69;
	setHeight(11483);
	iterate();
	Wempty			= 0;
	Wfuel			= 0;
	WfuelReq		= 0;
	Preq			= 0;
	Pmaximum		= 0;
	climb = false;
	power = false;
	fuel = false;
}

void Compound::fuelSpent(double wf, int i)
{
	cout << "Fuel spent in mission segment " << i << " is: " << wf << endl;
	Wfuel = Wfuel - wf;
	Wgross = Wgross - wf;
	if (Wfuel <= 0)
	{
		cout << "Fuel is depleted" << endl;
	}
}

void Compound::hoverPower(double time, int i)
{
	T = Wgross / 2;
	C_T = Wgross / (coArea * density * pow(maxTipSpeed, 2));
	double B = 1 - pow(2 * C_T / (2 * nob), 0.5);
	Pih = B * kint * pow(T, 1.5) / pow(2 * coArea * density, 0.5);
	//Pih = 2* B * kint * pow(T, 1.5) / pow(2 * coArea * density, 0.5);
	Poh = 0.125 * Cd0 * density * coArea * solidity * pow(maxTipSpeed, 3);
	//Poh = 0.125 * Cd0 * density * rotorArea * solidity * pow(maxTipSpeed, 3);
	Phover = (Pih + Poh) / 550;
	requiredPower(Phover);
	double Wspent = sfc * Phover * time;
	fuelSpent(Wspent, i);
	missionStats(Phover, i);
}

void Compound::forwardPower(double time, double vel, int i)
{
	T = Wgross / 2;
	advRatio = vel / maxTipSpeed;
	Piw = k * kint * pow(T, 2) / (2 * density * coArea * vel);
	//Piw = 2 * k * kint * pow(T, 2) / (2 * density * coArea * vel);
	double Cpo = (0.125 * Cd0 * solidity * (1 + K * pow(advRatio, 2)));
	compressibleLoss();
	Cpo = Cpo + dcpo;
	Pow = density * coArea * pow(maxTipSpeed, 3) * Cpo;
	//Pow = density * rotorArea * pow(maxTipSpeed, 3) * Cpo;
	Ppw = 0.5 * f * density * pow(vel, 3);
	Pforward = (Piw + Pow + Ppw) / 550;
	requiredPower(Pforward);
	double Wspent = time * Pforward * sfc;
	fuelSpent(Wspent, i);
	missionStats(Pforward, i);
}

void Compound::forwardPowerClimb(double time, double vel, int i)
{
	T = Wgross / 2;
	advRatio = vel / maxTipSpeed;
	Piw = k * kint * pow(T, 2) / (2 * density * coArea * vel);
	//Piw = 2 * k * kint * pow(T, 2) / (2 * density * coArea * vel);
	double Cpo = (0.125 * Cd0 * solidity * (1 + K * pow(advRatio, 2)));
	compressibleLoss();
	Cpo = Cpo + dcpo;
	Pow = density * coArea * pow(maxTipSpeed, 3) * Cpo;
	//Pow = density * rotorArea * pow(maxTipSpeed, 3) * Cpo;
	Ppw = 0.5 * f * density * pow(vel, 3);
	Pforward = (Piw + Pow + Ppw) / 550;
	climbPerformance(Pforward);
	PforwardClimb = (forwardMaxClimb * Wgross) / 550;
	requiredPower(Pforward + PforwardClimb);
	double Wspent = time * (Pforward + PforwardClimb) * sfc;
	fuelSpent(Wspent, i);
	missionStats(Pforward + PforwardClimb, i);
}

void Compound::forwardPowerDescend(double time, double vel, int i)
{
	T = Wgross / 2;
	advRatio = vel / maxTipSpeed;
	Piw = k * kint * pow(T, 2) / (2 * density * coArea * vel);
	//Piw = 2 * k * kint * pow(T, 2) / (2 * density * coArea * vel);
	double Cpo = (0.125 * Cd0 * solidity * (1 + K * pow(advRatio, 2)));
	compressibleLoss();
	Cpo = Cpo + dcpo;
	Pow = density * coArea * pow(maxTipSpeed, 3) * Cpo;
	//Pow = density * rotorArea * pow(maxTipSpeed, 3) * Cpo;
	Ppw = 0.5 * f * density * pow(vel, 3);
	Pforward = (Piw + Pow + Ppw) / 550;
	PforwardDescend = (forwardMaxClimb * Wgross) / 550;
	requiredPower(Pforward + PforwardDescend);
	double Wspent = time * (Pforward - PforwardDescend) * sfc;
	fuelSpent(Wspent, i);
	missionStats(Pforward - PforwardDescend, i);
}

void Compound::missionStats(double P, int i)
{
	cout << "Power Required in mission segment " << i << " is: " << P << endl;
}

void Compound::mission()
{
	Altitude height(4922);
	alt = &height;

	climb = false;
	fuel = false;
	power = false;
	Preq = 0;
	Pmaximum = 0;
	// REQ FUEL HESAPLAMA
	// 1. Segment
	setHeight(4922);
	hoverPower(0.0833, 1);
	// 2. Segment
	setHeight(8202);
	maxEnduranceSpeed();
	cout << VmaxEndurance << endl;
	forwardPowerClimb(0.121, VmaxEndurance, 2);
	// 3. Segment *********
	setHeight(11483);
	forwardPower(0.2, Vmax, 3);
	forwardPower(0.2, Vmax, 3);
	forwardPower(0.2, Vmax, 3);
	forwardPower(0.2, Vmax, 3);
	forwardPower(0.2, Vmax, 3);
	// 4. Segment
	maxEnduranceSpeed();
	cout << VmaxEndurance << endl;
	forwardPower(0.5, VmaxEndurance, 4);
	// 5. Segment
	hoverPower(0.0833, 5);
	Wgross = Wgross - Wpayload;
	// 6. Segment ************
	forwardPower(0.2, Vmax, 6);
	forwardPower(0.2, Vmax, 6);
	forwardPower(0.2, Vmax, 6);
	forwardPower(0.2, Vmax, 6);
	forwardPower(0.2, Vmax, 6);
	// 7. Segment
	setHeight(8202);
	maxEnduranceSpeed();
	cout << VmaxEndurance << endl;
	forwardPowerDescend(0.121, VmaxEndurance, 7);
	// 8. Segment
	setHeight(4922);
	hoverPower(0.037, 8);

	cout << "----" << endl;
	cout << "Fuel left is " << Wfuel << " lbs" << endl;
	cout << "Wgross is " << Wgross << " lbs" << endl;

	recordMissionData();


}

void Compound::NSmission()
{
	double offset = 50;
	Altitude height(4922, offset);
	alt = &height;

	climb = false;
	fuel = false;
	power = false;
	Preq = 0;
	Pmaximum = 0;
	// REQ FUEL HESAPLAMA
	// 1. Segment
	setHeightNS(4922, offset);
	hoverPower(0.0833, 1);
	// 2. Segment
	setHeightNS(8202, offset);
	maxEnduranceSpeed();
	forwardPowerClimb(0.121, VmaxEndurance, 2);
	// 3. Segment *********
	setHeightNS(11483, offset);
	forwardPower(0.2, Vmax, 3);
	forwardPower(0.2, Vmax, 3);
	forwardPower(0.2, Vmax, 3);
	forwardPower(0.2, Vmax, 3);
	forwardPower(0.2, Vmax, 3);
	// 4. Segment
	maxEnduranceSpeed();
	forwardPower(0.5, VmaxEndurance, 4);
	// 5. Segment
	hoverPower(0.0833, 5);
	Wgross = Wgross - Wpayload;
	// 6. Segment ************
	forwardPower(0.2, Vmax, 6);
	forwardPower(0.2, Vmax, 6);
	forwardPower(0.2, Vmax, 6);
	forwardPower(0.2, Vmax, 6);
	forwardPower(0.2, Vmax, 6);
	// 7. Segment
	setHeightNS(8202, offset);
	maxEnduranceSpeed();
	forwardPowerDescend(0.121, VmaxEndurance, 7);
	// 8. Segment
	setHeightNS(4922, offset);
	hoverPower(0.037, 8);

	cout << "----" << endl;
	cout << "Fuel left is " << Wfuel << " lbs" << endl;
	cout << "Wgross is " << Wgross << " lbs" << endl;

	cout << "Available engine power is: " << 1400 * height.getSigmap() / height.getTeta() << endl;

}

void Compound::parameter()
{

	Altitude height(4922);
	alt = &height;

	climb = false;
	fuel = false;
	power = false;
	Preq = 0;
	Pmaximum = 0;

		// 1. Segment
		setHeight(4922);
		hoverPower(0.0833, 1);
		// 2. Segment
		setHeight(9023);
		maxEnduranceSpeed();
		double t = ((13124 - 4922) / forwardMaxClimb)/3600;
		forwardPowerClimb(t, VmaxEndurance, 2);
		
		setHeight(13124);
		//maxEnduranceSpeed();
		maxRangeSpeed();

		T = Wgross / 2;
		advRatio = VmaxRange / maxTipSpeed;
		Piw = k * kint * pow(T, 2) / (2 * density * coArea * VmaxRange);
		double Cpo = (0.125 * Cd0 * solidity * (1 + K * pow(advRatio, 2)));
		compressibleLoss();
		Cpo = Cpo + dcpo;
		Pow = density * coArea * pow(maxTipSpeed, 3) * Cpo;
		Ppw = 0.5 * f * density * pow(VmaxRange, 3);
		Pforward = (Piw + Pow + Ppw) / 550;

		cout << Pforward << endl;
		cout << VmaxRange << endl;
		cout << Wfuel << endl;

	/*	double tt = (Wfuel - 100) / (sfc * Pforward);
		cout << tt << endl;*/
		double rr = (3600 * VmaxRange * (Wfuel - 100) / (sfc * Pforward))*0.0003; // in km
		cout << rr << endl;
}

void Compound::serviceCeiling()
{
	Altitude height(15000);
	alt = &height;
	setHeight(39850);

	maxEnduranceSpeed();
	double vel = VmaxEndurance;
	T = Wgross / 2;
	advRatio = vel / maxTipSpeed;
	Piw = k * kint * pow(T, 2) / (2 * density * coArea * vel);
	double Cpo = (0.125 * Cd0 * solidity * (1 + K * pow(advRatio, 2)));
	Pow = density * coArea * pow(maxTipSpeed, 3) * Cpo;
	Ppw = 0.5 * f * density * pow(vel, 3);
	Pforward = (1/eff)*(Piw + Pow + Ppw + 1*Wgross) / 550;	

	double psl = 1400;
	double palt = psl*height.getSigmap() / height.getTeta();
	cout << "Available power is: " << palt << endl;
	cout << "Required power is: " << Pforward << endl;


}

void Compound::recordweight()
{
	wempty_final = Wempty;
	wfuel_final = Wfuel;
	wgross_final = Wgross;
}

void Compound::IGEpower()
{
	// Leishman p260 Eq 5.103, steph lee, Eq 56
	double z = 3 * radius;													// IGE distance, z/D > 0.5 !
	double D = 2 * radius;													// Rotor Diameter
	double x = z / D;
	double AA = 0.9926;
	double BB = 0.0379;
	double kg = 1 / (AA + BB * pow((2 * radius / z), 2));

	Altitude height(z);		// Forward flight altitude, for first iteration
	alt = &height;
	
	T = Wgross / 2;															
	C_T = Wgross / (coArea * density * pow(maxTipSpeed, 2));				
	double B = 1 - pow(2 * C_T / (2 * nob), 0.5);
	Pih = B * kint * pow(T, 1.5) / pow(2 * coArea * density, 0.5);			// OGE Hover Power
	Poh = 0.125 * Cd0 * density * coArea * solidity * pow(maxTipSpeed, 3);	// Profile Hover Power

	double Pih_IGE = Pih * (-0.1276 * pow(x, 4) + 0.7080 * pow(x, 3) - 1.4569 * pow(x, 2) + 1.3432 * x
		+ 0.5147);

	PhoverIGE = (Poh + kg * Pih_IGE) / 550;

	cout << endl;
	cout << "IGE power req in " << z << " ft from ground is " << PhoverIGE << " hp" << endl;
	cout << "OGE power req in " << z << " ft from ground is " << (Pih + Poh) / 550 << " hp" << endl;
	cout << endl;

}

void Compound::setEFPA(double F)
{
	f = F;
}

void Compound::plotForward()
{
	Altitude height(11483);		// Forward flight altitude, for first iteration
	alt = &height;
	setHeight(11483);
	double vel = 0;
	
	
	fstream file;
	string filename = "forwardpower.txt";
	file.open(filename, fstream::out);
	file << left << setw(15) << "Vel" << setw(15) << "Piw" << setw(15) << "Pow" << setw(15) << "Ppw" << endl;

	for (vel = 30; vel <= 450; vel = vel + 10)
	{
		T = Wgross / 2;
		advRatio = vel / maxTipSpeed;
		Piw = k * kint * pow(T, 2) / (2 * density * coArea * vel);
		double Cpo = (0.125 * Cd0 * solidity * (1 + K * pow(advRatio, 2)));
		compressibleLoss();
		Cpo = Cpo + dcpo;
		Pow = density * coArea * pow(maxTipSpeed, 3) * Cpo;
		Ppw = 0.5 * f * density * pow(vel, 3);
		Pforward = (Piw + Pow + Ppw) / 550;		
		file << left << setw(15) << vel << setw(15) << Piw / 550 << setw(15) << Pow / 550 << setw(15) << Ppw / 550 << setw(15) << Pforward << endl;
	}
	
	// Power required at W'
	double Wprime = Wgross - (Wfuel / 2);
	double Cpo = 0;

	T = Wprime / 2;
	VmaxRange = pow(T / (2 * density * coArea), 0.5) * pow(4 * K / (f / rotorArea), 0.25);
	advRatio = VmaxRange / maxTipSpeed;
	Piw = k * kint * pow(T, 2) / (2 * density * coArea * VmaxRange);
	Cpo = (0.125 * Cd0 * solidity * (1 + K * pow(advRatio, 2)));
	Pow = density * coArea * pow(maxTipSpeed, 3) * Cpo;
	Ppw = 0.5 * f * density * pow(VmaxRange, 3);
	Pforward = (Piw + Pow + Ppw) / 550;

	cout << VmaxRange << endl;
	cout << Pforward << endl;
	

	//T = Wprime / 2;
	//VmaxEndurance = pow(T / (2 * density * coArea), 0.5) * pow(4 * K / (3 * f / rotorArea), 0.25);
	//advRatio = VmaxEndurance / maxTipSpeed;
	//Piw = k * kint * pow(T, 2) / (2 * density * coArea * VmaxEndurance);
	//Cpo = (0.125 * Cd0 * solidity * (1 + K * pow(advRatio, 2)));
	//Pow = density * coArea * pow(maxTipSpeed, 3) * Cpo;
	//Ppw = 0.5 * f * density * pow(VmaxEndurance, 3);
	//Pforward = (Piw + Pow + Ppw) / 550;
	//
	//cout << VmaxEndurance << endl;
	//cout << Pforward << endl;
	

	file << endl;
	file.close();
}

void Compound::hoverCeiling()		
{
	Altitude height(13125, 15);		// Forward flight altitude, for first iteration
	alt = &height;
	setHeightNS(13125, 15);
	T = Wgross / 2;
	C_T = Wgross / (coArea * density * pow(maxTipSpeed, 2));
	double B = 1 - pow(2 * C_T / (2 * nob), 0.5);
	Pih = B * kint * pow(T, 1.5) / pow(2 * coArea * density, 0.5);
	//Poh = 0.125 * Cd0 * density * coArea * solidity * pow(maxTipSpeed, 3);
	Poh = 0.125 * Cd0 * density * rotorArea * solidity * pow(maxTipSpeed, 3);
	Phover = 2 * (Pih + Poh + Wgross*1) / 550;

	cout << "Power available at 4000 m is: " << 1400 * height.getSigmap() / height.getTeta() << endl;
	cout << "Power req by hover at 4000 m is: " << Phover << endl;

}

