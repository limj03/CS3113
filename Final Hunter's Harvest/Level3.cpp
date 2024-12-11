#include "Level3.h"
#include "Utility.h"

#define LEVEL_WIDTH 30
#define LEVEL_HEIGHT 30
#define ENEMY_COUNT 6

const char PLAYER_SPRITE[] = "assets/characters/npc30.png";
const char TILEMAP_SPRITE[] = "assets/map/new.png";
const char ENEMY_SPRITE[] = "assets/characters/enemies.png";
const char FONT_SPRITE[] = "assets/font/font.png";
const char POWERUP_SPRITE[] = "assets/characters/Apple.png";



unsigned int LEVEL3_DATA[] =
{
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,56,58,58,58,58,58,58,58,58,58,58,58,59,-1,
-1,-1,-1,56,58,58,58,58,58,58,58,58,58,58,58,58,62,62,62,62,62,62,62,62,62,62,62,62,63,-1,
-1,-1,-1,60,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,63,-1,
-1,-1,-1,60,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,63,-1,
-1,-1,-1,60,62,62,62,62,62,62,62,62,62,62,62,62,62,70,70,70,70,70,62,62,62,62,70,70,71,-1,
-1,-1,-1,60,62,62,62,70,70,70,70,70,70,70,62,62,71,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,-1,-1,60,62,62,63,-1,-1,-1,-1,-1,-1,-1,60,63,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,-1,-1,60,62,62,63,-1,-1,-1,-1,-1,-1,-1,60,63,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,-1,-1,60,62,62,63,-1,-1,-1,-1,-1,-1,-1,60,63,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,56,58,62,62,62,62,58,59,-1,56,58,58,58,60,63,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,60,62,62,62,62,62,62,63,-1,60,62,62,62,62,63,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,60,62,62,62,62,62,62,63,-1,60,62,62,62,62,63,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,60,62,62,62,62,62,62,63,-1,60,62,62,62,62,63,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,60,62,62,62,62,62,62,63,-1,68,70,70,70,70,71,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,60,62,62,62,62,62,62,63,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,68,70,70,70,70,70,70,71,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,56,58,58,58,58,58,58,58,58,58,62,62,62,63,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,60,62,62,62,62,62,62,62,62,62,62,62,62,63,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,60,62,62,62,70,70,70,70,70,70,62,62,62,63,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,68,70,70,71,-1,-1,-1,-1,-1,-1,60,62,62,63,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,68,70,70,71,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
};

void Level3::initialise() {



    font_texture = Utility::load_texture(FONT_SPRITE);

    //player setup
    g_state.player = new Entity(PLAYER, glm::vec3(15.0f, -25.0f, 0.0f), 3.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(PLAYER_SPRITE),
        new int[3] { 3, 4, 5 }, new int[3] { 6, 7, 8 }, 3, 0, 0.05f, 3, 4, 1.3f, 1.0f, 1);
    g_state.player->set_up_animations(new int[3] { 9, 10, 11 });
    g_state.player->set_down_animations(new int[3] { 0, 1, 2 });
    g_state.player->set_direction("right");
    g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->RIGHT];

    //ai setup
    g_state.enemies = new Entity[ENEMY_COUNT];
    g_state.enemies[0] = Entity(ENEMY, glm::vec3(25.0f, -28.0f, 0.0f), 2.8f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(ENEMY_SPRITE),
        new int[2] { 0, 1 }, new int[2] { 0, 1 }, 2, 0, 0.5f, 10, 1, 1.0f, 1.0f, 1);
    g_state.enemies[0].set_ai_type(HUNTER);
    g_state.enemies[0].set_ai_state(SLEEPING);

    g_state.enemies[1] = Entity(ENEMY, glm::vec3(3.0f, -12.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(ENEMY_SPRITE),
        new int[2] { 0, 1 }, new int[2] { 0, 1 }, 2, 0, 0.5f, 10, 1, 1.0f, 1.0f, 1);
    g_state.enemies[1].set_ai_type(WALKER_X);
    g_state.enemies[1].set_direction("right");

    g_state.enemies[2] = Entity(ENEMY, glm::vec3(7.0f, -12.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(ENEMY_SPRITE),
        new int[2] { 0, 1 }, new int[2] { 0, 1 }, 2, 0, 0.5f, 10, 1, 1.0f, 1.0f, 1);
    g_state.enemies[2].set_ai_type(WALKER_X);
    g_state.enemies[2].set_direction("right");

    g_state.enemies[3] = Entity(ENEMY, glm::vec3(23.0f, -5.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(ENEMY_SPRITE),
        new int[2] { 0, 1 }, new int[2] { 0, 1 }, 2, 0, 0.5f, 10, 1, 1.0f, 1.0f, 1);
    g_state.enemies[3].set_ai_type(WALKER_X);
    g_state.enemies[3].set_direction("right");

    g_state.enemies[4] = Entity(ENEMY, glm::vec3(25.0f, -6.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(ENEMY_SPRITE),
        new int[2] { 0, 1 }, new int[2] { 0, 1 }, 2, 0, 0.5f, 10, 1, 1.0f, 1.0f, 1);
    g_state.enemies[4].set_ai_type(WALKER_X);
    g_state.enemies[4].set_direction("right");

    g_state.enemies[5] = Entity(ENEMY, glm::vec3(26.0f, -2.0f, 0.0f), 2.6f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(ENEMY_SPRITE),
        new int[2] { 0, 1 }, new int[2] { 0, 1 }, 2, 0, 0.5f, 10, 1, 1.0f, 1.0f, 1);
    g_state.enemies[5].set_ai_type(HUNTER);
    g_state.enemies[5].set_ai_state(SLEEPING);

    //powerup
    g_state.powerup = new Entity(POWERUP, glm::vec3(7.0f, -16.0f, 0.0f), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), Utility::load_texture(POWERUP_SPRITE),
        new int[1] { 0 }, new int[1] { 0 }, 1, 0, 0.5f, 17, 1, 1.0f, 1.0f, 1);

    //map setup
    GLuint map_texture_id = Utility::load_texture(TILEMAP_SPRITE);
    g_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL3_DATA, map_texture_id, 1.0f, 4, 9);
}

void Level3::update(float delta_time) {
    if (!g_game_is_over) {
        m_enemies_alive = 0;
        g_state.player->update(delta_time, g_state.player, g_state.enemies, ENEMY_COUNT, g_state.map);
        g_state.powerup->update(delta_time, g_state.player, g_state.player, ENEMY_COUNT, g_state.map);
        for (int i = 0; i < ENEMY_COUNT; i++) {
            g_state.enemies[i].update(delta_time, g_state.player, g_state.enemies, ENEMY_COUNT, g_state.map);
            if (g_state.enemies[i].get_active()) {
                m_enemies_alive += 1;
            }
        }
        
    }
}

void Level3::render(ShaderProgram* program)
{
    g_state.map->render(program);
    g_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        g_state.enemies[i].render(program);
    }
    g_state.powerup->render(program);
    
}

Level3::~Level3()
{
    delete    g_state.player;
    delete    g_state.map;
}