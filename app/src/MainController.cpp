#include <app/MainController.hpp>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <spdlog/spdlog.h>
namespace app {
void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();
    engine::core::Controller::get<
        engine::platform::PlatformController>()->set_enable_cursor(false);

    auto camera = engine::core::Controller::get<
        engine::graphics::GraphicsController>()->camera();
    camera->Position = glm::vec3(0.0f, 3.0f, 15.0f);

    init_bloom_buffers();
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
    update_event_chain();
    update_light_controls();

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    m_animation_time += platform->dt();
}
void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}
void MainController::draw_scene_objects() {
    draw_library();
    draw_piano();
    draw_candles();
}

void MainController::draw() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    int width = platform->window()->width();
    int height = platform->window()->height();

    // 1. Crtaj scenu u HDR framebuffer (dva izlaza: puna scena + svetli delovi)
    engine::graphics::OpenGL::bind_framebuffer(m_hdr_fbo);
    engine::graphics::OpenGL::clear_buffers();
    draw_scene_objects();

    // 2. Izdvoji svetle delove iz scene teksture
    auto bright_shader = resources->shader("bright_extract");
    engine::graphics::OpenGL::bind_framebuffer(m_pingpong_fbo[0]);
    bright_shader->use();
    bright_shader->set_int("scene_texture", 0);
    engine::graphics::OpenGL::bind_texture_unit(0, m_bright_color_tex);
    engine::graphics::OpenGL::draw_screen_quad();

    // 3. Gausovsko zamucenje - naizmenicno horizontalno/vertikalno, 10 prolaza
    auto blur_shader = resources->shader("blur");
    bool horizontal = true;
    int blur_passes = 10;
    blur_shader->use();
    for (int i = 0; i < blur_passes; ++i) {
        engine::graphics::OpenGL::bind_framebuffer(m_pingpong_fbo[horizontal ? 1 : 0]);
        blur_shader->set_bool("horizontal", horizontal);
        engine::graphics::OpenGL::bind_texture_unit(0, i == 0 ? m_pingpong_tex[0] : m_pingpong_tex[!horizontal]);
        engine::graphics::OpenGL::draw_screen_quad();
        horizontal = !horizontal;
    }

    // 4. Spoji originalnu scenu sa zamucenim sjajem, nacrtaj na ekran
    engine::graphics::OpenGL::bind_framebuffer(0);
    engine::graphics::OpenGL::clear_buffers();
    auto combine_shader = resources->shader("bloom_combine");
    combine_shader->use();
    combine_shader->set_int("scene_texture", 0);
    combine_shader->set_int("bloom_texture", 1);
    combine_shader->set_float("exposure", 1.0f);
    engine::graphics::OpenGL::bind_texture_unit(0, m_scene_color_tex);
    engine::graphics::OpenGL::bind_texture_unit(1, m_pingpong_tex[!horizontal]);
    engine::graphics::OpenGL::draw_screen_quad();
}

void MainController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}
void MainController::set_light_uniforms(engine::resources::Shader* shader) {
    shader->set_vec3("moon_direction", glm::vec3(-0.3f, -1.0f, -0.2f));
    shader->set_vec3("moon_color", glm::vec3(0.85f, 0.85f, 0.9f));

    float lamp_boost = (m_event_stage >= 2) ? 1.6f : 1.0f;
    shader->set_vec3("lamp_position", glm::vec3(0.0f, 4.0f, 0.0f));
    shader->set_vec3("lamp_color", glm::vec3(1.3f, 1.1f, 0.85f) * lamp_boost * m_lamp_intensity);

    glm::vec3 spot_base = (m_event_stage >= 1)
        ? glm::vec3(1.4f, 1.2f, 1.5f)
        : glm::vec3(0.5f, 0.5f, 0.6f);
    shader->set_vec3("spot_position", glm::vec3(0.0f, 8.0f, 0.0f));
    shader->set_vec3("spot_direction", glm::vec3(0.0f, -1.0f, 0.0f));
    shader->set_vec3("spot_color", spot_base * m_spot_intensity);
    shader->set_float("spot_cutoff", glm::cos(glm::radians(25.0f)));
    shader->set_float("spot_outer_cutoff", glm::cos(glm::radians(40.0f)));

    shader->set_vec3("fill_position", glm::vec3(-3.0f, 1.5f, -2.0f));
    shader->set_vec3("fill_color", glm::vec3(0.4f, 0.4f, 0.45f));
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
    translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))
    * scale(glm::mat4(1.0f), glm::vec3(m_piano_scale))
    * translate(glm::mat4(1.0f), glm::vec3(-7.45f, 0.0f, -5.99f))
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

        float flicker = 0.05f + 0.004f * sin(m_animation_time * 4.0f + i * 1.3f);

        shader->set_mat4(
            "model",
            translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z))
            * scale(glm::mat4(1.0f), glm::vec3(flicker))
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

void MainController::update_event_chain() {
    auto platform =
        engine::core::Controller::get<engine::platform::PlatformController>();
    m_time_elapsed += platform->dt();

    if (m_event_stage == 0 && m_time_elapsed >= 3.0f) {
        m_event_stage = 1;
        spdlog::info("Event chain: reflektor se pojacava");
    }
    if (m_event_stage == 1 && m_time_elapsed >= 6.0f) {
        m_event_stage = 2;
        spdlog::info("Event chain: sveće dodatno zasjaje");
    }
}

void MainController::update_light_controls() {
    auto platform =
        engine::core::Controller::get<engine::platform::PlatformController>();
    float dt = platform->dt();

    if (platform->key(engine::platform::KEY_UP).state()
        == engine::platform::Key::State::Pressed) {
        m_lamp_intensity += 1.0f * dt;
        }
    if (platform->key(engine::platform::KEY_DOWN).state()
        == engine::platform::Key::State::Pressed) {
        m_lamp_intensity -= 1.0f * dt;
        }
    m_lamp_intensity = glm::clamp(m_lamp_intensity, 0.1f, 4.0f);

    if (platform->key(engine::platform::KEY_RIGHT).state()
        == engine::platform::Key::State::Pressed) {
        m_spot_intensity += 1.0f * dt;
        }
    if (platform->key(engine::platform::KEY_LEFT).state()
        == engine::platform::Key::State::Pressed) {
        m_spot_intensity -= 1.0f * dt;
        }
    m_spot_intensity = glm::clamp(m_spot_intensity, 0.0f, 4.0f);

}

void MainController::init_bloom_buffers() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    int width = platform->window()->width();
    int height = platform->window()->height();

    engine::graphics::OpenGL::init_hdr_framebuffer(width, height, m_hdr_fbo, m_scene_color_tex, m_bright_color_tex);
    engine::graphics::OpenGL::init_pingpong_framebuffers(width, height, m_pingpong_fbo, m_pingpong_tex);
}

} // namespace app