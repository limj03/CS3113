class Platform {
private:
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::mat4 m_model_matrix;
    GLuint m_texture_id;

public:
    //default constructor
    Platform();
    //constructor
    Platform(glm::vec3 position, glm::vec3 scale, GLuint textid);


    //methods
    void update();
    void render(ShaderProgram* program);

    //getter functions
    glm::vec3 get_position() { return m_position; };
    glm::vec3 get_scale() { return m_scale; };

    //setter functions
    void set_position(glm::vec3 new_position) { m_position = new_position; };
    void set_scale(glm::vec3 new_scale) { m_scale = new_scale; };
    void set_textid(GLuint textid) { m_texture_id = textid; };


};