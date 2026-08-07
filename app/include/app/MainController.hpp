//
// Created by marina on 6. 8. 2026..
//

#ifndef MATF_RG_PROJECT_MAINCONTROLLER_HPP
#define MATF_RG_PROJECT_MAINCONTROLLER_HPP

#include <engine/core/Engine.hpp>

namespace app {

class MainController final : public engine::core::Controller {
public:
    std::string_view name() const override {
        return "app::MainController";
    }

private:
    void initialize() override;
    bool loop() override;
    void update() override;
    void begin_draw() override;
    void draw() override;
    void end_draw() override;

    void draw_lighthouse();
    void draw_rock();
    void draw_boat();
    void update_camera();
    void set_light_uniforms(engine::resources::Shader* shader);

    float m_lighthouse_scale{1.0f};
    float m_rock_scale{1.0f};
    float m_boat_scale{1.0f};
};

} // namespace app

#endif//MATF_RG_PROJECT_MAINCONTROLLER_HPP
