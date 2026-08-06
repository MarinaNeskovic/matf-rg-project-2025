#include <app/MainController.hpp>
#include <engine/core/Engine.hpp>

namespace app {

class MyApp final : public engine::core::App {
    void app_setup() override {
        auto main_controller = register_controller<MainController>();

        main_controller->after(
            engine::core::Controller::get<
                engine::core::EngineControllersEnd>()
        );
    }
};

} // namespace app

int main(int argc, char **argv) {
    return std::make_unique<app::MyApp>()->run(argc, argv);
}