
/* How Jasper Works:
 *  - Computer Vision
 *  - Aimbot
 *
 * Main Thread:
 *  |-> calls NewFrame() to poll for
 *
 */

#include <iostream>
#include <Jasp.hpp>

int main(void) {

    auto ctx = Jasp::CreateContext();

    while (true) {

        Jasp::NewFrame();

        auto& targets = Jasp::GetTargets();
        for (auto& target : targets) {
            std::cout << target << std::endl;
        }

    }

    Jasp::Terminate();

}