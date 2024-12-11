#pragma once
#include "Scene.h"

class MainMenu : public Scene {
public:
    GLuint font_texture_id;
    // ————— CONSTRUCTOR ————— //
    ~MainMenu();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};