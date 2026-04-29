#include "App/NeneEngineApp.h"
#include <iostream>

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
	try {
		NeneEngine::NeneEngineApp app;
		if (!app.Init(1280, 720, "NeneEngine")) {
			std::cerr << "Failed to initialize application\n";
			return -1;
		}

		app.Run();
		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "Fatal error: " << e.what() << "\n";
		return -1;
	}
}
