#pragma once
#include "Scene.h"

class WinMenu : public Scene {
public:
    GLuint font_texture_id;
    // ————— CONSTRUCTOR ————— //
    ~WinMenu();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};