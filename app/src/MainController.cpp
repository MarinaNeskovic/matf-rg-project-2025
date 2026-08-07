#include <app/MainController.hpp>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
namespace app {
void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    engine::core::Controller::get<
        engine::platform::PlatformController>()->set_enable_cursor(false);
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
    draw_library();
    draw_piano();
    draw_candles();
}
void MainController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}
void MainController::set_light_uniforms(engine::resources::Shader* shader) {
    shader->set_vec3("moon_direction", glm::vec3(-0.3f, -1.0f, -0.2f));
    shader->set_vec3("moon_color", glm::vec3(1.2f, 1.2f, 1.3f));

    shader->set_vec3("lamp_position", glm::vec3(0.0f, 4.0f, 0.0f));
    shader->set_vec3("lamp_color", glm::vec3(2.5f, 2.2f, 1.7f));

    shader->set_vec3("spot_position", glm::vec3(0.0f, 8.0f, 0.0f));
    shader->set_vec3("spot_direction", glm::vec3(0.0f, -1.0f, 0.0f));
    shader->set_vec3("spot_color", glm::vec3(1.8f, 1.9f, 2.3f));
    shader->set_float("spot_cutoff", glm::cos(glm::radians(25.0f)));
    shader->set_float("spot_outer_cutoff", glm::cos(glm::radians(40.0f)));

    shader->set_vec3("fill_position", glm::vec3(-3.0f, 1.5f, -2.0f));
    shader->set_vec3("fill_color", glm::vec3(0.8f, 0.8f, 0.9f));
}

void MainController::update_camera() {
    auto platform =
        engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera =
        engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
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

void MainController::draw_piano() {
    auto graphics =
        engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources =
        engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("basic");
    auto piano = resources->model("piano");
    shader->use();
    set_light_uniforms(shader);
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4(
        "model",
        translate(glm::mat4(1.0f), glm::vec3(-6.9f, -1.05f, -6.05f))
        * scale(glm::mat4(1.0f), glm::vec3(m_piano_scale))
    );
    piano->draw(shader);
}

void MainController::draw_candles() {
    auto graphics =
        engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources =
        engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("basic");
    auto candle = resources->model("candle");
    shader->use();
    set_light_uniforms(shader);
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    const int candle_count = 16;
    const float radius = 2.0f;
    for (int i = 0; i < candle_count; ++i) {
        float angle = (float)i / (float)candle_count * 2.0f * 3.14159265f;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        shader->set_mat4(
            "model",
            translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z))
            * scale(glm::mat4(1.0f), glm::vec3(0.05f))
        );
        candle->draw(shader);
    }
}

void MainController::draw_library() {
    auto graphics =
        engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto resources =
        engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("basic");
    auto library = resources->model("library");
    shader->use();
    set_light_uniforms(shader);
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    shader->set_mat4("model", glm::mat4(1.0f));
    library->draw(shader);
}
} // namespace app