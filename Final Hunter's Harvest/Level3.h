#pragma once
#include "Scene.h"

class Level3 : public Scene {
public:

    GLuint font_texture;
    bool g_game_is_over = false;
    // ————— CONSTRUCTOR ————— //
    ~Level3();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};