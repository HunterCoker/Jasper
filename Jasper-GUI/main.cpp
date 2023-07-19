#include "application.hpp"

int main(void) {
	
	Application* app = new Application;
	app->Run();
	delete app;
	
	return 0;
}