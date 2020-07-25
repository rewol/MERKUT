// Teknofest 2020 Helikopter tasarım yarışması için Refik Alper Tuncer ve Ahmet Semih Parlak tarafından geliştirilmiştir
// Tüm hakları saklıdır.

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
