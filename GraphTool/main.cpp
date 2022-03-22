#include "Include/Application.hpp"

#if defined(_DEBUG)
#define DOCTEST_CONFIG_IMPLEMENT
#include "3rdParty/doctest.h"
#endif

int main()
{
#if defined(_DEBUG)
	doctest::Context context;
	const int testResult{ context.run() };
	if (context.shouldExit()) {
		return testResult;
	}
#endif

	Application app;
	app.run();

	return 0;
}