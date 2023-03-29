#include <iostream>
#include "Engine.h"

int main()
{
	// Init engine
	Engine Engine;

	// Main loop
	while (Engine.running())
	{
		// Update
		Engine.update();

		// Render
		Engine.render();
	}

	return 0;
}
