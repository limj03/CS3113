#include "MainMenu.h"

const char FONT_SPRITE[] = "assets/font/font.png";


void MainMenu::initialise() {
	font_texture_id = Utility::load_texture(FONT_SPRITE);
}

void MainMenu::update(float delta_time) {}

void MainMenu::render(ShaderProgram* program) {
	Utility::draw_text(program, font_texture_id, "Hunter's Harvest", 1, 0.01, glm::vec3(-7.0, 1.0, 0.0));
	Utility::draw_text(program, font_texture_id, "Press enter to", 1, 0.01, glm::vec3(-6.0, -1.0, 0.0));
	Utility::draw_text(program, font_texture_id, "start", 1, 0.01, glm::vec3(-2.0, -2.0, 0.0));
}

MainMenu::~MainMenu()
{
}