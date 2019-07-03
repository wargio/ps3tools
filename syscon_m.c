#include "syscon_m.h"

u16 Get_Soft_ID(int i){
	u16 sc_soft_id[11];
	sc_soft_id[ 0] = 0x0B8E;
	sc_soft_id[ 1] = 0x0C16;
	sc_soft_id[ 2] = 0x0D52;
	sc_soft_id[ 3] = 0x0DBF;
	sc_soft_id[ 4] = 0x0E69;
	sc_soft_id[ 5] = 0x0F29;
	sc_soft_id[ 6] = 0x0F38;
	sc_soft_id[ 7] = 0x065D;
	sc_soft_id[ 8] = 0x0832;
	sc_soft_id[ 9] = 0x08C2;
	sc_soft_id[10] = 0x0918;

	return sc_soft_id[i];
}

