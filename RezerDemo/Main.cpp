#include "Application/Engine.h"

int main()
{
	// Set flags for tracking CPU memory leaks
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

	Engine engine;
	engine.run();

	return 0;
}