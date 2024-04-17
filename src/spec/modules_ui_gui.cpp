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
UTEST(GUI, guiChar) { ASSERT_EQ(sizeof(guiChar), 1); }

UTEST(GUI, guiMemset) {
	struct smtg {
		int x;
	};

	smtg conf;

	guiMemset(&conf, 1, 2);

	// should look like this
	// 257 -> 00000000 00000000 00000001 00000001
	ASSERT_EQ(conf.x, 257);
}


UTEST(GUI, allignPtr) {
	typedef struct pixel
	{
		unsigned char red;	
		unsigned char blue;	
		unsigned char green;	
	};

	pixel *p = (pixel *)malloc(sizeof(pixel) * 4);

	printf("Before alignment:\n");
	printf("Memory address of pixels[0]: %p\n", (void *)&p[0]);

	binRep((void *)&p[0]);

	p = (pixel*)GUI_ALIGN_PTR_BACK(p, 4);

	printf("After alignment:\n");
	printf("Memory address of pixels[0]: %p\n", (void *)&p[0]);

	binRep((void *)&p[0]);
}



#endif