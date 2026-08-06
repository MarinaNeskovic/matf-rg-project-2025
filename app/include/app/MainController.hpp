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

    void draw_backpack();
    void update_camera();

    float m_backpack_scale{1.0f};
};

} // namespace app

#endif//MATF_RG_PROJECT_MAINCONTROLLER_HPP
