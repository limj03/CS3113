/**
* Author: Jeffrey Lim
* Assignment: Lunar Lander
* Date due: 2024-10-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <vector>
#include <string>
#include "Platform.h"
#include "Rocket.h"
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
float g_time_accumulator = 0.0f;

using namespace std;

const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

SDL_Window* displayWindow;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

#define LOG(argument) std::cout << argument << '\n'

//shader variables
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

//background
constexpr float BG_RED = 0.4665625f,
BG_GREEN = 0.97265625f,
BG_BLUE = 0.9609375f,
BG_OPACITY = 1.0f;

//speeds
float ROCKET_SPEED = 1.0f;

//player position
glm::vec3 g_player_position = glm::vec3(-15.0f, 15.0f, 0.0f);

//lava positions
glm::vec3 lava_platform_positions[5] = {
	glm::vec3(-8.0f, -10.0f, 0.0f),
	glm::vec3(-13.0f, -6.0f, 0.0f),
	glm::vec3(0.0f, -4.0f, 0.0f),
	glm::vec3(12.0f, -8.0f, 0.0f),
	glm::vec3(18.0f, -3.0f, 0.0f)
};

//water positions
glm::vec3 water_platform_positions[4] = {
	glm::vec3(-12.0f, -8.0f, 0.0f),
	glm::vec3(-4.0f, -7.0f, 0.0f),
	glm::vec3(16.0f, -9.0f, 0.0f),
	glm::vec3(5.0f, -9.0f, 0.0f),
};

//scaling
glm::vec3 g_player_scale = glm::vec3(1.0f, 1.0f, 0.0f);
glm::vec3 g_platform_scale = glm::vec3(1.0f, 1.0f, 0.0f);

//booleans
bool gameIsRunning;
bool gameOn = true;

//texture
const char PLAYER_SPRITE_FILEPATH[] = "fireball.png";
const char WATER_PLATFORM_SPRITE_FILEPATH[] = "water.png";
const char LAVA_PLATFORM_SPRITE_FILEPATH[] = "lava.png";
const char FONT_SPRITE_FILEPATH[] = "font1.png";

GLuint g_player_texture_id;
GLuint g_water_platform_texture_id;
GLuint g_lava_platform_texture_id;
GLuint g_font_texture_id;

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;

//variables to use delta time
float g_previous_ticks = 0.0f;
const float MILLISECONDS_IN_SECOND = 1000.0f;

struct GameState {
	Rocket* player;
	Platform* water_platforms;
	Platform* lava_platforms;
};

GameState g_game_state;
int WATER_PLATFORM_COUNT = 4;
int LAVA_PLATFORM_COUNT = 5;
bool winner = false;

//load textures into gl
GLuint load_texture(const char* filepath) {
	int width, height, number_of_components;
	unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

	if (image == NULL) {
		LOG("Unable to load image. Make sure path is correct.");
		assert(false);
	}

	GLuint textureID;
	glGenTextures(NUMBER_OF_TEXTURES, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);

	return textureID;
}

// Draw Text function from the lecture
constexpr int FONTBANK_SIZE = 16;

void draw_text(ShaderProgram* program, GLuint g_font_texture_id, std::string text,
	float font_size, float spacing, glm::vec3 position)
{
	// Scale the size of the fontbank in the UV-plane
	// We will use this for spacing and positioning
	float width = 1.0f / FONTBANK_SIZE;
	float height = 1.0f / FONTBANK_SIZE;

	// Instead of having a single pair of arrays, we'll have a series of pairs—one for
	// each character. Don't forget to include <vector>!
	std::vector<float> vertices;
	std::vector<float> texture_coordinates;

	// For every character...
	for (int i = 0; i < text.size(); i++) {
		// 1. Get their index in the spritesheet, as well as their offset (i.e. their
		//    position relative to the whole sentence)
		int spritesheet_index = (int)text[i];  // ascii value of character
		float offset = (font_size + spacing) * i;

		// 2. Using the spritesheet index, we can calculate our U- and V-coordinates
		float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
		float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

		// 3. Inset the current pair in both vectors
		vertices.insert(vertices.end(), {
			offset + (-0.5f * font_size), 0.5f * font_size,
			offset + (-0.5f * font_size), -0.5f * font_size,
			offset + (0.5f * font_size), 0.5f * font_size,
			offset + (0.5f * font_size), -0.5f * font_size,
			offset + (0.5f * font_size), 0.5f * font_size,
			offset + (-0.5f * font_size), -0.5f * font_size,
			});

		texture_coordinates.insert(texture_coordinates.end(), {
			u_coordinate, v_coordinate,
			u_coordinate, v_coordinate + height,
			u_coordinate + width, v_coordinate,
			u_coordinate + width, v_coordinate + height,
			u_coordinate + width, v_coordinate,
			u_coordinate, v_coordinate + height,
			});
	}

	// 4. And render all of them using the pairs
	glm::mat4 model_matrix = glm::mat4(1.0f);
	model_matrix = glm::translate(model_matrix, position);

	program->set_model_matrix(model_matrix);
	glUseProgram(program->get_program_id());

	glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0,
		vertices.data());
	glEnableVertexAttribArray(program->get_position_attribute());
	glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
		texture_coordinates.data());
	glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

	glBindTexture(GL_TEXTURE_2D, g_font_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

	glDisableVertexAttribArray(program->get_position_attribute());
	glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}



void initialise() {
	//initalise window
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

	//initialise context
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	gameIsRunning = true;

#ifdef _WINDOWS
	glewInit();
#endif

	//initialise viewport
	glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

	g_view_matrix = glm::mat4(1.0f);
	g_projection_matrix = glm::ortho(-20.0f, 20.0f, -15.0f, 15.0f, -1.0f, 1.0f);

	g_shader_program.set_view_matrix(g_view_matrix);
	g_shader_program.set_projection_matrix(g_projection_matrix);
	glUseProgram(g_shader_program.get_program_id());

	g_shader_program.set_colour(1, 0, 0, 1);

	//set background
	glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

	//initialise gamestate
	g_player_texture_id = load_texture(PLAYER_SPRITE_FILEPATH);
	g_water_platform_texture_id = load_texture(WATER_PLATFORM_SPRITE_FILEPATH);
	g_lava_platform_texture_id = load_texture(LAVA_PLATFORM_SPRITE_FILEPATH);
	g_font_texture_id = load_texture(FONT_SPRITE_FILEPATH);
	g_game_state.player = new Rocket(g_player_position, g_player_scale, ROCKET_SPEED, g_player_texture_id);

	g_game_state.water_platforms = new Platform[WATER_PLATFORM_COUNT];
	for (int i = 0; i < WATER_PLATFORM_COUNT; ++i) {
		g_game_state.water_platforms[i].set_textid(g_water_platform_texture_id);
		g_game_state.water_platforms[i].set_position(water_platform_positions[i]);
		g_game_state.water_platforms[i].set_scale(g_platform_scale);
	}

	g_game_state.lava_platforms = new Platform[LAVA_PLATFORM_COUNT];
	for (int i = 0; i < LAVA_PLATFORM_COUNT; ++i) {
		g_game_state.lava_platforms[i].set_textid(g_lava_platform_texture_id);
		g_game_state.lava_platforms[i].set_position(lava_platform_positions[i]);
		g_game_state.lava_platforms[i].set_scale(g_platform_scale);
	}

	//enable blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void process_input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// End game
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;
		}
	}

	const Uint8* key_state = SDL_GetKeyboardState(NULL);
	if (gameOn == true) {
		if (key_state[SDL_SCANCODE_W])
		{
			g_game_state.player->inc_y();

		}
		else if (key_state[SDL_SCANCODE_S])
		{
			g_game_state.player->dec_y();

		}
		else if (key_state[SDL_SCANCODE_A])
		{
			g_game_state.player->dec_x();

		}
		else if (key_state[SDL_SCANCODE_D])
		{
			g_game_state.player->inc_x();

		}

	}
}

void update()
{
	// --- DELTA TIME CALCULATIONS --- //
	float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
	float delta_time = ticks - g_previous_ticks;
	g_previous_ticks = ticks;

	//if rocket hits either left or right
	if (g_game_state.player->get_position().x >= 20.0f) {
		gameOn = false;
		winner = false;
	}
	if (g_game_state.player->get_position().x <= -20.0f) {
		gameOn = false;
		winner = false;
	}

	// Game Over LOGIC
	//for water platforms
	for (int i = 0; i < WATER_PLATFORM_COUNT; ++i) {
		g_game_state.water_platforms[i].update();
		//if collision is detected
		if (g_game_state.player->collision(&g_game_state.water_platforms[i])) {
			gameOn = false;
			//rocket "lands" on platform (aka position of rocket is above of the collided platform)
			if (g_game_state.player->get_position().y > g_game_state.water_platforms[i].get_position().y) {
				winner = true;
			}
			//rocket "crashes" into platform
			else {
				winner = false;
			}
		}
	}
	//for lava platforms, instant lose
	for (int i = 0; i < LAVA_PLATFORM_COUNT; ++i) {
		g_game_state.lava_platforms[i].update();
		//if collision is detected, lose
		if (g_game_state.player->collision(&g_game_state.lava_platforms[i])) {
			gameOn = false;
			winner = false;
		}
	}

	//if rocket hits bottom of the screen, lose
	if (g_game_state.player->get_position().y < -15.0f) {
		gameOn = false;
		winner = false;
	}

	// ————— FIXED TIMESTEP ————— //
	// STEP 1: Keep track of how much time has passed since last step
	delta_time += g_time_accumulator;

	// STEP 2: Accumulate the ammount of time passed while we're under our fixed timestep
	if (delta_time < FIXED_TIMESTEP)
	{
		g_time_accumulator = delta_time;
		return;
	}

	// STEP 3: Once we exceed our fixed timestep, apply that elapsed time into the objects' update function invocation
	while (delta_time >= FIXED_TIMESTEP)
	{
		// Notice that we're using FIXED_TIMESTEP as our delta time
		if (gameOn) {
			g_game_state.player->update(FIXED_TIMESTEP);
		}
		
		delta_time -= FIXED_TIMESTEP;
	}

	g_time_accumulator = delta_time;

}


void render() {
	glClear(GL_COLOR_BUFFER_BIT);

	// Vertices
	float vertices[] = {
		-0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
		-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
	};

	// Textures
	float texture_coordinates[] = {
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
	};

	glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(g_shader_program.get_position_attribute());

	glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
	glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

	// We disable two attribute arrays now
	glDisableVertexAttribArray(g_shader_program.get_position_attribute());
	glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

	g_game_state.player->render(&g_shader_program);
	for (int i = 0; i < WATER_PLATFORM_COUNT; ++i) {
		g_game_state.water_platforms[i].render(&g_shader_program);
	}
	for (int i = 0; i < LAVA_PLATFORM_COUNT; ++i) {
		g_game_state.lava_platforms[i].render(&g_shader_program);
	}

	//Fuel display
	std::string fuel_text = "Fuel: " + std::to_string((int)g_game_state.player->get_fuel());
	draw_text(&g_shader_program, g_font_texture_id, fuel_text, 2.0f, 0.1f, glm::vec3(-9.0f, 8.5f, 0.0f));

	// LOGIC for determining who is the winner
	if (gameOn == false) {
		if (winner == true) {
			draw_text(&g_shader_program, g_font_texture_id, "Mission Accomplished!", 1.3f, 0.1f, glm::vec3(-10.5f, 1.0f, 0.0f));
		}
		else if (winner == false) {
			draw_text(&g_shader_program, g_font_texture_id, "Mission Failed!", 2.0f, 0.1f, glm::vec3(-10.5f, 1.0f, 0.0f));
		}

	}

	SDL_GL_SwapWindow(displayWindow);
}


void shutdown() { SDL_Quit(); }

int main(int argc, char* argv[]) {
	initialise();

	while (gameIsRunning) {
		process_input();

		update();

		render();
	}


	shutdown();
	return 0;
}



