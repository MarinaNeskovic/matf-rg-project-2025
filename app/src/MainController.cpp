#include <app/MainController.hpp>

#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>

namespace app {

void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
}

bool MainController::loop() {
    const auto platform =
        engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).state()
        == engine::platform::Key::State::JustPressed) {
        return false;
    }

    return true;
}

void MainController::update() {
    update_camera();
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw() {
    draw_backpack();
}

void MainController::end_draw() {
    engine::core::Controller::get<
        engine::platform::PlatformController>()->swap_buffers();
}

void MainController::draw_backpack() {
    auto graphics =
        engine::core::Controller::get<engine::graphics::GraphicsController>();

    auto resources =
        engine::core::Controller::get<engine::resources::ResourcesController>();

    auto shader = resources->shader("basic");
    auto backpack = resources->model("backpack");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4(
        "model",
        scale(glm::mat4(1.0f), glm::vec3(m_backpack_scale))
    );

    backpack->draw(shader);
}

void MainController::update_camera() {
    auto platform =
        engine::core::Controller::get<engine::platform::PlatformController>();

    auto camera =
        engine::core::Controller::get<
            engine::graphics::GraphicsController>()->camera();

    float dt = platform->dt();

    if (platform->key(engine::platform::KEY_W).state()
        == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    }

    if (platform->key(engine::platform::KEY_S).state()
        == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    }

    if (platform->key(engine::platform::KEY_A).state()
        == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    }

    if (platform->key(engine::platform::KEY_D).state()
        == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
    }

    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

} // namespace app