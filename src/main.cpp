#include <caustx/Engine.h>

int main() {
    CaustXEngine engine;

    if (engine.Init(1280, 720, "CaustX Volumetric Engine")) {
        engine.Run();
    }

    return 0;
}
