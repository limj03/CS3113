/**
* Author: Jeffrey Lim
* Assignment: Rise of the AI
* Date due: 2024-11-9, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include <Windows.h>

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define ENEMY_COUNT 3
#define LEVEL1_WIDTH 30
#define LEVEL1_HEIGHT 10
#define LOG(argument) std::cout << argument << '\n'
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"

using namespace std;

const char GAME_WINDOW_NAME[] = "Rise of the AI";
const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

float BG_RED = 0.85f,
BG_BLUE = 0.65f,
BG_GREEN = 0.35f,
BG_OPACITY = 1.0f;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
const char PLAYER_SPRITE[] = "assets/characters/npc30.png";
const char TILEMAP_SPRITE[] = "assets/map/Mine_Blocks.png";
const char ENEMY_SPRITE[] = "assets/characters/enemies.png";
const char FONT_SPRITE[] = "assets/font/font.png";

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;
const int FONTBANK_SIZE = 16.0f;

unsigned int LEVEL_1_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 2, 2, 2, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    0, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 6,
    6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 6, 6,
    6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, //
    6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 0, 0, 0, 0, 1, 2, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
};

//delta time
float previous_ticks = 0.0f;
const float MILLISECONDS_IN_SECOND = 1000.0f;
float delta_time;
float g_accumulator = 0.0f;

//speed
float g_player_speed = 1.0f;

//position
glm::vec3 g_player_position = glm::vec3(0.0f, 0.0f, 0.0f);

//scaling
glm::vec3 g_player_scale = glm::vec3(1.0f, 1.0f, 0.0f);

SDL_Window* g_display_window;
bool g_game_is_running = true;
bool g_game_is_over = false;
bool winner;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

GLuint font_texture_id;
GLuint player_texture_id;
GLuint enemy_texture_id;

struct GameState {
	Entity* player;
	Entity* enemies;
	Map* map;
};

GameState g_game_state;

void draw_text(ShaderProgram* program, GLuint g_font_texture_id, std::string text,
    float font_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairsone for
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

GLuint load_texture(const char* filepath)
{
	int width, height, number_of_components;
	unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

	if (image == NULL)
	{
		LOG("Unable to load image. Make sure the path is correct.");
		assert(false);
	}

	GLuint texture_id;
	glGenTextures(NUMBER_OF_TEXTURES, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(image);

	return texture_id;
}


void initialise() {
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow(GAME_WINDOW_NAME,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-10.0f, 10.0f, -7.0f, 7.0f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
    
    //initialise texture_ids
    font_texture_id = load_texture(FONT_SPRITE);
    player_texture_id = load_texture(PLAYER_SPRITE);
    enemy_texture_id = load_texture(ENEMY_SPRITE);


    //create player
    g_game_state.player = new Entity(PLAYER, glm::vec3(3.0f, -3.0f, 0.0f), 2.5f, glm::vec3(0.0f, -20.0f, 0.0f), player_texture_id,
        new int[3] { 3, 4, 5 }, new int[3] { 6, 7, 8 }, 3, 0, 0.1f, 3, 4, 0.8f, 0.8f);
    g_game_state.player->set_direction("right");
    g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->RIGHT];

    g_game_state.enemies = new Entity[ENEMY_COUNT];

    //create AIs
    //walker
    g_game_state.enemies[0] = Entity(ENEMY, glm::vec3(11.0f, 0.0f, 0.0f), 2.0f, glm::vec3(0.0f, -20.0f, 0.0f), enemy_texture_id,
        new int[1] { 1 }, new int[2] { 0, 1 }, 2, 0, 0.1f, 10, 1, 0.8f, 0.8f);
    g_game_state.enemies[0].set_direction("right");
    g_game_state.enemies[0].set_ai_type(WALKER);
    
    //jumper
    g_game_state.enemies[1] = Entity(ENEMY, glm::vec3(8.0f, -4.0f, 0.0f), 2.0f, glm::vec3(0.0f, -20.0f, 0.0f), enemy_texture_id,
        new int[1] { 1 }, new int[2] { 0, 1 }, 2, 0, 0.1f, 10, 1, 0.8f, 0.8f);
    g_game_state.enemies[1].set_direction("right");
    g_game_state.enemies[1].set_ai_type(JUMPER);
    g_game_state.enemies[1].set_jumping_power(6.0f);

    //jumper-guard
    g_game_state.enemies[2] = Entity(ENEMY, glm::vec3(25.0f, -1.0f, 0.0f), 2.0f, glm::vec3(0.0f, -20.0f, 0.0f), enemy_texture_id,
        new int[1] { 1 }, new int[2] { 0, 1 }, 2, 0, 0.1f, 10, 1, 0.8f, 0.8f);
    g_game_state.enemies[2].set_direction("left");
    g_game_state.enemies[2].set_ai_type(GUARD);
    g_game_state.enemies[2].set_jumping_power(10.0f);
    g_game_state.enemies[2].set_ai_state(IDLE);
    
    //jump power of the player
    g_game_state.player->m_jumping_power = 8.0f;

    // ----- MAP SET-UP ----- //
    GLuint map_texture_id = load_texture(TILEMAP_SPRITE);
    g_game_state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 4, 3);
    
    // ----- BLENDING ----- //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_game_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_game_is_running = false;
                break;

            case SDLK_SPACE:
                //jump (double jump included)
                if (g_game_state.player->m_jumps >= 1)
                {
                    g_game_state.player->m_is_jumping = true;
                }
                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_game_state.player->move_left();
        g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_game_state.player->move_right();
        g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->RIGHT];
    }
    
    if (glm::length(g_game_state.player->get_movement()) > 1.0f)
    {
        g_game_state.player->set_movement(glm::normalize(g_game_state.player->get_movement()));
    }
}

void update()
{

    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - previous_ticks;
    previous_ticks = ticks;

    //decrement number of enemies alive
    int enemies_alive = ENEMY_COUNT;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (!g_game_state.enemies[i].get_active()) {
            enemies_alive -= 1;
        }
    }
    //if all enemies died, game is over
    if (enemies_alive == 0) {
        g_game_is_over = true;
        winner = true;
    }
    //if player is dead, game is over as well
    if (!g_game_state.player->get_active()) {
       g_game_is_over = true;
       winner = false;
    }

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    //update player and enemies
    while (delta_time >= FIXED_TIMESTEP)
    {
        if (!g_game_is_over) {
            g_game_state.player->update(FIXED_TIMESTEP, g_game_state.player, g_game_state.enemies, ENEMY_COUNT, g_game_state.map);
            for (int i = 0; i < ENEMY_COUNT; i++) {
                g_game_state.enemies[i].update(FIXED_TIMESTEP, g_game_state.player, NULL, 0, g_game_state.map);
            }
        }
        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_game_state.player->get_position().x, -g_game_state.player->get_position().y, 0.0f));
    
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);
    
    //render player and enemies
    g_game_state.player->render(&g_shader_program);
    g_game_state.map->render(&g_shader_program);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        g_game_state.enemies[i].render(&g_shader_program);
    }

    //render game over message
    if (g_game_is_over && winner) {
        draw_text(&g_shader_program, font_texture_id, "YOU WIN!", 1, 0.1, g_game_state.player->get_position() + glm::vec3(-5.0f, 2.0f, 0.0f));
    }
    else if (g_game_is_over && !winner) {
        draw_text(&g_shader_program, font_texture_id, "YOU LOSE!", 1, 0.1, g_game_state.player->get_position() + glm::vec3(-5.0f, 2.0f, 0.0f));
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    delete g_game_state.player;
    delete g_game_state.map;
}

//  GAME LOOP  //
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}