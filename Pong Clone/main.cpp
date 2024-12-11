/**
* Author: Jeffrey Lim
* Assignment: Pong Clone
* Date due: 2024-10-12, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>

enum AppStatus { RUNNING, TERMINATED };

constexpr float WINDOW_SIZE_MULT = 1.0f;

constexpr int WINDOW_WIDTH = 640 * WINDOW_SIZE_MULT,
WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;

constexpr float BG_RED = 0.9765625f,
BG_GREEN = 0.97265625f,
BG_BLUE = 0.9609375f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

// Images
constexpr char RAT2_SPRITE_FILEPATH[] = "rat2.png",
RAT1_SPRITE_FILEPATH[] = "rat1.png",
BALL_SPRITE_FILEPATH[] = "cheese.png",
FONT_SPRITE_FILEPATH[] = "font1.png";

constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;
constexpr glm::vec3 INIT_SCALE_PADDLES = glm::vec3(1.0f, 1.0f, 0.0f),

// STARTING POSITIONS/SCALES OF OBJECTS
INIT_POS_RAT1 = glm::vec3(0.0f, 0.0f, 0.0f),
INIT_POS_RAT2 = glm::vec3(0.0f, 0.0f, 0.0f),
INIT_POS_BALL = glm::vec3(0.0f, 0.0f, 0.0f),
INIT_SCALE_BALL = glm::vec3(0.5f, 0.5f, 0.0f);

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_rat2_matrix, g_projection_matrix, g_rat1_matrix;
glm::mat4 g_ball_matrix[3]; // Matrix for up to 3 balls

float g_previous_ticks = 0.0f;

GLuint g_rat2_texture_id;
GLuint g_rat1_texture_id;
GLuint g_ball_texture_id;
GLuint g_font_texture_id;

constexpr float RAT1_SPEED = 3.0f; // speed of rats
constexpr float BALL_SPEED = 2.0f; // speed of ball

glm::vec3 g_rat2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_rat2_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_rat1_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_rat1_movement = glm::vec3(0.0f, 0.0f, 0.0f);

// vector of balls for multiple ball useage
glm::vec3 g_ball_position[3] = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) };
glm::vec3 g_ball_movement[3] = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) };

int g_number_of_balls = 1; // Number of active balls (user can change between 1-3 balls)

// bool variables for game
bool gameOn = true;
bool singleplayer = false;
bool moveUp = true;
bool p1winner = false;
bool p2winner = false;

void draw_text();
bool collision();
void endGame();
void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

// Draw Text function from the lecture
constexpr int FONTBANK_SIZE = 16;

void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text,
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

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

//calculate positions of objects to search for collision
//from Box-Box collision Notes
bool collision(glm::vec3 x_coord_a, glm::vec3 x_coord_b, glm::vec3 width_a, glm::vec3 width_b) {
    float x_distance = fabs(x_coord_a.x - x_coord_b.x) - ((width_a.x + width_b.x) / 2.0f);
    float y_distance = fabs(x_coord_a.y - x_coord_b.y) - ((width_a.y + width_b.y) / 2.0f);

    if (x_distance < 0.0f && y_distance < 0.0f) {
        // Collision!
        return true;
    }
    return false;
}

// Function that stops movement of all objects when game is over
void endGame() {
    g_rat1_movement = { 0.0f, 0.0f, 0.0f };
    g_rat2_movement = { 0.0f, 0.0f, 0.0f };
    for (int i = 0; i < g_number_of_balls; i++) {
        g_ball_movement[i] = { 0.0f, 0.0f, 0.0f };
    }
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("User-Input and Collisions Exercise",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr) shutdown();

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_rat2_matrix = glm::mat4(1.0f);
    g_rat1_matrix = glm::mat4(1.0f);
    for (int i = 0; i < 3; i++) {
        g_ball_matrix[i] = glm::mat4(1.0f);
    }

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_rat2_texture_id = load_texture(RAT2_SPRITE_FILEPATH);
    g_rat1_texture_id = load_texture(RAT1_SPRITE_FILEPATH);
    g_ball_texture_id = load_texture(BALL_SPRITE_FILEPATH);
    g_font_texture_id = load_texture(FONT_SPRITE_FILEPATH);

    // enable blending
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
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_q: g_app_status = TERMINATED; break;
            case SDLK_t:
                singleplayer = true;
                break;
            case SDLK_1: g_number_of_balls = 1; 
                break; // Press '1' to select 1 ball
            case SDLK_2: g_number_of_balls = 2; 
                break; // Press '2' to select 2 balls
            case SDLK_3: g_number_of_balls = 3; 
                break; // Press '3' to select 3 balls
            default: break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    if (gameOn == true) {
        if (key_state[SDL_SCANCODE_W])
        {
            g_rat1_movement.y = 1.0f;

        }
        else if (key_state[SDL_SCANCODE_S])
        {
            g_rat1_movement.y = -1.0f;

        }
        else {
            g_rat1_movement.y = 0.0f;
        }
        if (singleplayer == false) {
            if (key_state[SDL_SCANCODE_UP])
            {
                g_rat2_movement.y = 1.0f;
            }
            else if (key_state[SDL_SCANCODE_DOWN])
            {
                g_rat2_movement.y = -1.0f;
            }
            else {
                g_rat2_movement.y = 0.0f;
            }
        }
        else {
            // singleplayer logic for movement
            if (g_rat2_position.y >= 3.2f || g_rat2_position.y <= -3.2f) {
                moveUp = !moveUp;
            }
            if (moveUp == true) {
                g_rat2_movement.y = 1.0f;
            }
            else {
                g_rat2_movement.y = -1.0f;
            }
        }
    }
}

void update()
{
    // --- DELTA TIME CALCULATIONS --- //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    // --- ACCUMULATOR LOGIC --- //
    g_rat1_position += g_rat1_movement * RAT1_SPEED * delta_time;

    g_rat2_position += g_rat2_movement * RAT1_SPEED * delta_time;

    for (int i = 0; i < g_number_of_balls; i++) {
        g_ball_position[i] += g_ball_movement[i] * BALL_SPEED * delta_time;
    }

    //set initial positions
    g_rat1_position.x = -4;
    g_rat2_position.x = 4;

    // --- TRANSLATION --- //
    g_rat2_matrix = glm::mat4(1.0f);
    g_rat2_matrix = glm::translate(g_rat2_matrix, g_rat2_position);

    g_rat1_matrix = glm::mat4(1.0f);
    g_rat1_matrix = glm::translate(g_rat1_matrix, g_rat1_position);

    // Apply all for all balls
    for (int i = 0; i < g_number_of_balls; i++) {
        g_ball_matrix[i] = glm::mat4(1.0f);
        g_ball_matrix[i] = glm::translate(g_ball_matrix[i], INIT_POS_BALL);
        g_ball_matrix[i] = glm::translate(g_ball_matrix[i], g_ball_position[i]);
        g_ball_matrix[i] = glm::scale(g_ball_matrix[i], INIT_SCALE_BALL);
    }

    // --- SCALING --- //
    g_rat2_matrix = glm::scale(g_rat2_matrix, INIT_SCALE_PADDLES);

    g_rat1_matrix = glm::scale(g_rat1_matrix, INIT_SCALE_PADDLES);

    // normalize
    if (glm::length(g_rat1_movement) > 1.0f)
    {
        g_rat1_movement = glm::normalize(g_rat1_movement);
    }
    if (glm::length(g_rat2_movement) > 1.0f)
    {
        g_rat2_movement = glm::normalize(g_rat2_movement);
    }

    // --- COLLISION LOGIC --- //
    for (int i = 0; i < g_number_of_balls; i++) {
        if (collision(g_ball_position[i], g_rat2_position, INIT_SCALE_BALL, INIT_SCALE_PADDLES)) {
            g_ball_movement[i].x = -g_ball_movement[i].x;
            g_ball_movement[i].y = g_ball_position[i].y - g_rat2_position.y;
        }

        else if (collision(g_ball_position[i], g_rat1_position, INIT_SCALE_BALL, INIT_SCALE_PADDLES)) {
            g_ball_movement[i].x = -g_ball_movement[i].x;
            g_ball_movement[i].y = g_ball_position[i].y - g_rat1_position.y;
        }

        if (g_ball_position[i].y > 3.5f) {
            g_ball_movement[i].y = -g_ball_movement[i].y;
        }
        else if (g_ball_position[i].y < -3.5f) {
            g_ball_movement[i].y = -g_ball_movement[i].y;
        }
    }

    //BORDER for paddles
    if (g_rat1_position.y >= 3.2f) {
        g_rat1_position.y = 3.2f;
    }
    else if (g_rat1_position.y <= -3.2f) {
        g_rat1_position.y = -3.2f;
    }
    if (g_rat2_position.y >= 3.2f) {
        g_rat2_position.y = 3.2f;
    }
    else if (g_rat2_position.y <= -3.2f) {
        g_rat2_position.y = -3.2f;
    }

    // Game Over LOGIC
    for (int i = 0; i < g_number_of_balls; i++) {
        if (g_ball_position[i].x <= -5.0f) {
            //player 2 wins
            g_ball_position[i].x = -5.0f;
            endGame();
            gameOn = false;
            p2winner = true;
        }
        else if (g_ball_position[i].x >= 5.0f) {
            //player 1 wins
            g_ball_position[i].x = 5.0f;
            endGame();
            gameOn = false;
            p1winner = true;
        }
    }
}

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
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

    // Bind texture
    draw_object(g_rat1_matrix, g_rat1_texture_id);
    draw_object(g_rat2_matrix, g_rat2_texture_id);

    for (int i = 0; i < g_number_of_balls; i++) {
        draw_object(g_ball_matrix[i], g_ball_texture_id);
    }

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // LOGIC for determining who is the winner
    if (gameOn == false) {
        if (p1winner == true) {
            draw_text(&g_shader_program, g_font_texture_id, "Player 1 wins!", 0.5f, 0.05f, glm::vec3(-3.5f, 2.0f, 0.0f));
        }
        else if (p2winner == true) {
            draw_text(&g_shader_program, g_font_texture_id, "Player 2 wins!", 0.5f, 0.05f, glm::vec3(-3.5f, 2.0f, 0.0f));
        }
        
    }
    

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
