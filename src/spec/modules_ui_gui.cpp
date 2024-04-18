#include "Build.hpp"

#ifdef RUN_TESTS

#include "../tools/utest.h"
#include "../tools/log.h"

#include "../modules/ui/Gui.hpp"
#include "../modules/ui/GuiInternal.hpp"

#include <stdio.h>
#include <iostream>
#include <bitset>
#include <climits>



template<typename T>
void binRep(const T& a)
{
    const char* beg = reinterpret_cast<const char*>(&a);
    const char* end = beg + sizeof(a);
    while(beg != end)
        std::cout << std::bitset<CHAR_BIT>(*beg++) << ' ';
    std::cout << '\n';
}

// DUMMY TEST
/*UTEST(GUI, guiChar) { ASSERT_EQ(sizeof(guiChar), 1); }*/

UTEST(GUI, guiMemset) {
	struct smtg {
		int x;
	};

	smtg conf;

	guiMemset(&conf, 1, 2);

	// should look like this
	// 257 -> 00000000 00000000 00000001 00000001
	ASSERT_EQ((int)conf.x, (int)257);
}


UTEST(GUI, guiPtrAdd) {
	typedef struct pixel
	{
		unsigned int red;	
		unsigned int blue;	
		unsigned int green;	
	};

	int pixelMSize = sizeof(int);

	pixel *p = (pixel *)malloc(sizeof(pixel) * 4);

	p[0].red = 1;
	p[0].blue = 2;
	p[0].green = 3;

    //printf("Memory address of p[0]: %p\n", (void *)&p[0]);   
   	pixel *pp = guiPtrAdd(pixel, p, pixelMSize*2);
   	//printf("Memory address of pp[0]: %p\n", (void *)&pp[0]);

   	//printf("pp[0]: %d\n", pp[0].red);

   	ASSERT_EQ(pp[0].red, (unsigned int)3);
}



#endif