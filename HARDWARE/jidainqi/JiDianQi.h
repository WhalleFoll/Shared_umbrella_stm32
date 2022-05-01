#ifndef __JIDIANQI_H
#define __JIDIANQI_H
#include "sys.h"
					  
////////////////////////////////////////////////////////////////////////////////// 
#define JD1 PGout(15) // PG15
#define JD2 PGout(13) // PG13
#define JD3 PGout(11)// PG11
#define JD4 PGout(9) // PG19	

void JiDainQi_Init(void); 
void jidian_p(u8 i);
		 				    
#endif
