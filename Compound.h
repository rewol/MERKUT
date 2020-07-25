#pragma once
#include<math.h>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include "Altitude.h"
using namespace std;
class Compound
{
friend class Mission;
protected: // Program Inputs
	double DL				= 10;			// Disk Loading
	double kint				= 1.418;		// Intermeshing rotor coef.
	double maxTipMach		= 0.62;			// Max. Tip Mach
	double Cd0				= 0.011;		// Avg. rotor draf coef
	double hoverMaxClimb	= 15;			// Climb speed in hover
	double forwardMaxClimb	= 15;			// Climb speed in fwd flight
	double sfc				= 0.92;			// Specific Fuel Consumption
	double f				= 8;			// Equivalent drag area ft2
	double BL				= 0.1;			// Blade loading ARAÞTIRILMALI 
	double nob				= 4;			// Number of Blades
	double k				= 1.15;			// Non-ideal cases in hover flight
	double K				= 4.65;			// Fwd flight loss coefficient due to nonidealities
	double Vmax				= 365;			// Maximum forward speed
	double cla				= 5.73;			// Avg. mean lift curve slope for an airfoil
	double Paccess			= 10;			// Accesory power in [hp]
	double eff				= 0.9;			// Efficiency factor in power calculation
	double Pinstalled		= 4000;			// Installed engine power 
	double Wtank			= 3000;			// Fuel tank capacity
	
public:
	Compound(double, double, double);		// Wgross and Wpayload and Wcrew
	void rfmethod();						// RF iteration to estimate Wgross and Wfuel
	void revisedRfMethod();					// Revised RF method with varying Wgross
	void mission();							// Calculates real mission
	void NSmission();						// Calcualates real mission with dif. air conditions
	void parameter();
	void serviceCeiling();					// calculates service ceiling in fwd flight
	void hoverCeiling();					// calculates hover ceiling
	void recordweight();
	void IGEpower();						// Calculates IGE Power requirement
	void setEFPA(double);					// Sets f after engineering analysis on demand
	void plotForward();
	
				
protected:
	void setHeight(double);					// Input is mission height
	void setHeightNS(double, double);		// Input is mission height and offset temp
	void iterate();							// Performs intermediate property calculations
	void requiredPower(double);				// Tracks maximum power required in missions
	void requiredFuel(double);				// Tracks required fuel in missions
	void emptyWeight();						// Calculates empty weight based on Prouty's equations
	void hoverPower(double);				// Input is mission time
	void forwardPower(double, double);		// Inputs are mission time and velocity
	void forwardPowerClimb(double, double);	// Inputs are mission time and velocity
	void forwardPowerDescend(double, double);// Inputs are mission time and velocity
	void compressibleLoss();				// Compressible losses are added, if exists
	void maxEnduranceSpeed();				// Sets max endurance speed
	void maxRangeSpeed();					// Sets max range speed
	void climbPerformance(double);			// Calculates available climb velocity
	void segmentData(double, double);		// Shows fuel req and power req in each segment
	void helicopterData();					// Shows finalized helicopter data
	void recordData();						// Records helicopter data into a file
	void recordMissionData();				// Records specified mission data into text file
	void reset();							// Resets rotorcraft's values to default

private:
	void fuelSpent(double, int);
	void hoverPower(double, int);				
	void forwardPower(double, double, int);		
	void forwardPowerClimb(double, double, int);	
	void forwardPowerDescend(double, double, int);
	void missionStats(double, int);

protected:									// WEIGHTS
	double Wgross;
	double Wpayload;
	double Wcrew;
	double Wfuel;
	double WfuelReq;
	double Wempty;
	double Wrotor;
	double Whub;
	double Wfuselage;
	double Wldg;
	double Wpusher;
	double Wdrive;
	double Wavionic;
	double Wequipment;
	double Wengine;

protected:
	double wgross_final;
	double wfuel_final;
	double wempty_final;

private:									// POWER REQUIREMENTS
	double Preq;							// Power required from mission
	double Pmaximum;						// Power required in total !
	double Pih;
	double Poh;
	double Phover;
	double PhoverIGE;
	double Piw;
	double Pow;
	double Ppw;
	double Pforward;
	double PforwardClimb;
	double PforwardDescend;
	double wff;
protected:									// INTERMEDIATE PROPERTIES
	double T;
	double radius;
	double rotorArea;
	double coArea;
	double chord;
	double solidity;
	double C_T;
	double aoa;
	double mean_C_L;
	double mean_C_D;
	double maxTipSpeed;
	double maxAngSpeed;
	double advRatio;
	double dcpo;
	double VmaxEndurance;
	double VmaxRange;
	double segment;
	double AR;
	double const wgrossguess;
private:									// ALTITUDE PROPERTIES
	Altitude* alt;
	double density;
	double a;
private:
	bool climb = false;
	bool power = false;
	bool fuel = false;
	bool mis = false;
	
};