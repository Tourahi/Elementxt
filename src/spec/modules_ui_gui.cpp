#include "Build.hpp"

#ifdef RUN_TESTS

#include "../tools/utest.h"
#include "../tools/log.h"

#include "../modules/ui/Gui.hpp"
#include "../modules/ui/GuiInternal.hpp"

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

#endif