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
		int y;
	};

	smtg conf;

	guiMemset(&conf, 0, sizeof(conf));

	ASSERT_EQ(conf.x, 0);
	ASSERT_EQ(conf.y, 0);
}

#endif