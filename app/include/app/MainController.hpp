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
    void update_camera();
    void set_light_uniforms(engine::resources::Shader* shader);

    void  draw_piano();
    float m_piano_scale{1.0f};
    void draw_candles();
    float m_candle_scale{1.0f};

    void draw_library();
};

} // namespace app

#endif//MATF_RG_PROJECT_MAINCONTROLLER_HPP
