#include<iostream>
#include<iomanip>
#include<math.h>
#include "Altitude.h"
#include "Compound.h"


int main()
{
	Compound Merkut1(10000, 551, 188);
	Merkut1.revisedRfMethod();
	Merkut1.hoverCeiling();
	Merkut1.IGEpower();
	Merkut1.mission();
	return 0;
}