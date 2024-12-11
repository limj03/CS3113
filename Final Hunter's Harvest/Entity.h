#include "Map.h"

enum EntityType { PLAYER, ENEMY, POWERUP };
enum AIType { HUNTER, WALKER_X, WALKER_Y };
enum AIState { WALKING, IDLE, ATTACKING, SLEEPING, WAKING };

class Entity {
private:
	bool m_is_active = true;

	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_acceleration;

	int* m_animation_right = NULL;
	int* m_animation_left = NULL;
	int* m_animation_up = NULL;
	int* m_animation_down = NULL;

	float m_speed;
	glm::vec3 m_movement;
	glm::mat4 m_model_matrix;

	EntityType m_entity_type;
	AIType m_ai_type;
	AIState m_ai_state;

	string m_facing_direction;

	float m_width = 1;
	float m_height = 1;

public:
	static const int FRAMES_PER_SECOND = 4;
	static const int LEFT = 0,
		RIGHT = 1, UP = 2, DOWN = 3;

	int** m_walking = new int* [4] {
		m_animation_left,
			m_animation_right,
			m_animation_up,
			m_animation_down
	};

	int* m_sleeping = NULL;
	int* m_waking = NULL;

	int m_animation_frames = 0,
		m_animation_index = 0,
		m_animation_cols = 0,
		m_animation_rows = 0;

	int* m_animation_indices = NULL;
	float m_animation_time = 0.0f;

	bool hit;
	bool can_hit;

	//collisions
	bool m_collided_top = false;
	bool m_collided_bottom = false;
	bool m_collided_right = false;
	bool m_collided_left = false;
	bool m_collided_powerup;

	GLuint m_texture_id;

	int m_hp;
	float delta_time;

	float movement_timer = 0.0f;

	//"star power" in mario
	bool can_kill = false;

	//methods
	Entity();
	Entity(EntityType type, glm::vec3 pos, float speed, glm::vec3 accel, GLuint sprite,
		int* left_anim, int* right_anim, int frames, int index, float time, int cols, int rows, float heigh, float width, int health);
	~Entity();

	void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
	void update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map); // Now, update should check for both objects in the game AND the map
	void render(ShaderProgram* program);

	bool const check_collision(Entity* other) const;
	void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
	void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);

	// Overloading our methods to check for only the map
	void const check_collision_y(Map* map);
	void const check_collision_x(Map* map);

	void move_left() {
		m_movement.x = -1.0f;
		m_facing_direction = "left";
	};
	void move_right() {
		m_movement.x = 1.0f;
		m_facing_direction = "right";
	};
	void move_up() { m_movement.y = 1.0f; };
	void move_down() { m_movement.y = -1.0f; };

	void ai_activate(Entity* player, float delta_time);
	void ai_walk_x();
	void ai_walk_y();
	void ai_sleep(Entity* player);
	void ai_waking();
	void ai_attacking(Entity* player);

	void activate() { m_is_active = true; };
	void deactivate() { m_is_active = false; };

	// ————— GETTERS ————— //
	EntityType const get_entity_type()    const { return m_entity_type; };
	AIType     const get_ai_type()        const { return m_ai_type; };
	AIState    const get_ai_state()       const { return m_ai_state; };

	glm::vec3  const get_position()       const { return m_position; };
	glm::vec3  const get_movement()       const { return m_movement; };
	glm::vec3  const get_velocity()       const { return m_velocity; };
	glm::vec3  const get_acceleration()   const { return m_acceleration; };
	float      const get_speed()          const { return m_speed; };
	int        const get_width()          const { return m_width; };
	int        const get_height()         const { return m_height; };
	string	   const get_direction()	  const { return m_facing_direction; };
	bool	   const get_active()		  const { return m_is_active; };

	// ————— SETTERS ————— //
	void const set_m_sleeping(int* arr) { m_sleeping = arr; };
	void const set_active(bool b) { m_is_active = b; };
	void const set_m_waking(int* arr) { m_waking = arr; };
	void const set_entity_type(EntityType new_entity_type) { m_entity_type = new_entity_type; };
	void const set_ai_type(AIType new_ai_type) { m_ai_type = new_ai_type; };
	void const set_ai_state(AIState new_state) { m_ai_state = new_state; };
	void const set_position(glm::vec3 new_position) { m_position = new_position; };
	void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; };
	void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; };
	void const set_speed(float new_speed) { m_speed = new_speed; };
	void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; };
	void const set_width(float new_width) { m_width = new_width; };
	void const set_height(float new_height) { m_height = new_height; };
	void const set_direction(string direction) { m_facing_direction = direction; };

	void const set_up_animations(int* upAnimation) {
		m_animation_up = upAnimation;
		m_walking[UP] = m_animation_up;
	}
	void const set_down_animations(int* downAnimation) {
		m_animation_down = downAnimation;
		m_walking[DOWN] = m_animation_down;
	}
	void const kill() { m_is_active = false; };
};