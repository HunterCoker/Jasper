#include "Application.hpp"

int main(void) {

    Application* app = Application::GetInstance();
    app->Run();

    return 0;
}