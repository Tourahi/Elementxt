#include "Build.hpp"

#ifdef RUN_TESTS

#include "../tools/utest.h"

#include "../modules/ui/Gui.hpp"


UTEST(cpp, ASSERT_EQ) { ASSERT_EQ(sizeof(guiChar), 1); }

#endif