/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/
#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION

#define PI 3.141596f
#define NUM_SLIMES 20
#ifndef PLANE_SIZE
#define PLANE_SIZE 30
#define HPL_SIZE (PLANE_SIZE / 2.f)
#endif
#define NUM_ENEMIES 5

#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "player.cpp"
#include "arm.cpp"
#include "slime.cpp"
#include "collisions.h"
#include "boundingplane.h"
#include "world.cpp"
#include "projectile.cpp"

#include "tiny_obj_loader.h"
#include "WindowManager.h"
#include "Shape.h"
#include "WorldCollision.h"
#include "MaterialLoader.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "enemy.cpp"

using namespace std;
using namespace glm;
shared_ptr<Shape> shape;
shared_ptr<Shape> worldOBJ;
shared_ptr<Shape> playerOBJ;
shared_ptr<Shape> projectileOBJ;
shared_ptr<Shape> armOBJ;



double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}

// Player movement

class Application : public EventCallbacks {

public:

    WindowManager *windowManager = nullptr;
    WorldCollision worldCollision = WorldCollision();
    MaterialLoader worldMaterialLoader = MaterialLoader("../resources/world-v4.mtl");
    MaterialLoader playerMaterialLoader = MaterialLoader("../resources/new-wizard-body.mtl");
    MaterialLoader armMaterialLoader = MaterialLoader("../resources/new-wizard-arm2.mtl");

    // Our shader program
    std::shared_ptr<Program> prog, psky, pslime, pworld, pplayer, pprojectile, parm;

    // Contains vertex information for OpenGL
    GLuint VertexArrayID;

    // Data necessary to give our box to OpenGL
    GLuint VertexBufferID, VertexNormDBox, VertexTexBox, IndexBufferIDBox;

    vector<Projectile> player_projectiles;
    vector<Projectile> enemy_projectiles;
    Player player;
    Arm arm;
    World world;
    vector<Enemy> enemies;
    vector<int> game_stats;
    bool gameDone = false;

    //texture data
    GLuint Texture;
    GLuint Texture2;
    GLuint TextureSlime;
    GLuint TextureSlime2;



    float totalTime = 0;

    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if(gameDone)
            glfwSetWindowShouldClose(window, GL_TRUE);
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        if (key == GLFW_KEY_W && action == GLFW_PRESS) {
            world.w = 1;
            player.w = 1;
            arm.w = 1;
        }
        if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
            world.w = 0;
            player.w = 0;
            arm.w = 0;
        }
        if (key == GLFW_KEY_S && action == GLFW_PRESS) {
            world.s = 1;
            player.s = 1;
            arm.s = 1;
        }
        if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
            world.s = 0;
            player.s = 0;
            arm.s = 0;
        }
        if (key == GLFW_KEY_A && action == GLFW_PRESS) {
            world.a = 1;
            player.a = 1;
            arm.a = 1;
        }
        if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
            world.a = 0;
            player.a = 0;
            arm.a = 0;
        }
        if (key == GLFW_KEY_D && action == GLFW_PRESS) {
            world.d = 1;
            player.d = 1;
            arm.d = 1;
        }
        if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
            world.d = 0;
            player.d = 0;
            arm.d = 0;
        }
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            player_projectiles.push_back(arm.spawnProjectile());
            player.space = 1;
            arm.space = 1;
        }
        if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
            player.space = 0;
            arm.space = 0;
        }
        if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

    }

    // callback for the mouse when clicked move the triangle when helper functions
    // written
    void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
       player.playerRotation(window, xpos, ypos);
       arm.armRotation(window, xpos, ypos);
       //need to know when click happens
    }

    //if the window is resized, capture the new size and reset the viewport
    void resizeCallback(GLFWwindow *window, int in_width, int in_height) {
        //get the window size - may be different then pixels for retina
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
    }

    void initPlane() {
        //generate the VAO
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        //generate vertex buffer to hand off to OGL
        glGenBuffers(1, &VertexBufferID);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

        GLfloat rect_vertices[] = {
                // front
                -1.0, -1.0, 1.0,//LD
                1.0, -1.0, 1.0,//RD
                1.0, 1.0, 1.0,//RU
                -1.0, 1.0, 1.0,//LU
        };
        //make it a bit smaller
        for (int i = 0; i < 12; i++)
            rect_vertices[i] *= 0.5;
        //actually memcopy the data - only do this once
        glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_DYNAMIC_DRAW);

        //we need to set up the vertex array
        glEnableVertexAttribArray(0);
        //key function to get up how many elements to pull out at a time (3)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

        //color
        GLfloat cube_norm[] = {
                // front colors
                0.0, 0.0, 1.0,
                0.0, 0.0, 1.0,
                0.0, 0.0, 1.0,
                0.0, 0.0, 1.0,

        };
        glGenBuffers(1, &VertexNormDBox);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VertexNormDBox);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_norm), cube_norm, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

        //color
        glm::vec2 cube_tex[] = {
                // front colors
                glm::vec2(0.0, 2.0),
                glm::vec2(2.0, 2.0),
                glm::vec2(2.0, 0.0),
                glm::vec2(0.0, 0.0),

        };
        glGenBuffers(1, &VertexTexBox);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VertexTexBox);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_tex), cube_tex, GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);

        glGenBuffers(1, &IndexBufferIDBox);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
        GLushort cube_elements[] = {

                // front
                0, 1, 2,
                2, 3, 0,
        };
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);
    }

    /*Note that any gl calls must always happen after a GL state is initialized */
    void initGeom() {
        initPlane();
        glBindVertexArray(0);

        string resourceDirectory = "../resources";
        // Initialize mesh.
        shape = make_shared<Shape>();
        //shape->loadMesh(resourceDirectory + "/t800.obj");
        shape->loadMesh(resourceDirectory + "/sphere.obj");
        shape->resize();
        shape->init();

        worldOBJ = make_shared<Shape>();
        worldOBJ->loadMesh(resourceDirectory + "/world-v4.obj", (string *) "../resources/");
        worldOBJ->resize();
        worldOBJ->init();
        worldCollision.get3dGrid(worldOBJ);


        // Load Materials
        worldMaterialLoader.readMaterialFile();
        playerMaterialLoader.readMaterialFile();
        armMaterialLoader.readMaterialFile();

        playerOBJ = make_shared<Shape>();
        playerOBJ->loadMesh(resourceDirectory + "/new-wizard-body.obj", (string *) "../resources/");
        playerOBJ->resize();
        playerOBJ->init();

        armOBJ = make_shared<Shape>();
        armOBJ->loadMesh(resourceDirectory + "/new-wizard-arm2.obj", (string *) "../resources/");
        armOBJ->resize();
        armOBJ->init();

        projectileOBJ = make_shared<Shape>();
        projectileOBJ->loadMesh(resourceDirectory + "/fireball.obj");
        projectileOBJ->resize();
        projectileOBJ->init();

        int width, height, channels;
        char filepath[1000];

        //texture 1
        string str = resourceDirectory + "/water.jpeg";
        strcpy(filepath, str.c_str());
        unsigned char *data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &Texture);
        //glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        //texture 2
        str = resourceDirectory + "/night.jpg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &Texture2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Texture2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        //texture slime
        str = resourceDirectory + "/slime.jpeg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &TextureSlime);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, TextureSlime);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        //texture slime when hit
        str = resourceDirectory + "/slimehit.jpeg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &TextureSlime2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, TextureSlime2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        //[TWOTEXTURES]
        //set the 2 textures to the correct samplers in the fragment shader:
        GLuint Tex1Location = glGetUniformLocation(prog->pid, "tex");//tex, tex2... sampler in the fragment shader
        GLuint Tex2Location = glGetUniformLocation(prog->pid, "tex2");
        // Then bind the uniform samplers to texture units:
        glUseProgram(prog->pid);
        glUniform1i(Tex1Location, 0);
        glUniform1i(Tex2Location, 1);

        //PLAYER
        Tex1Location = glGetUniformLocation(pplayer->pid, "tex");//tex, tex2... sampler in the fragment shader
        Tex2Location = glGetUniformLocation(pplayer->pid, "tex2");
        // Then bind the uniform samplers to texture units:
        glUseProgram(pplayer->pid);
        glUniform1i(Tex1Location, 0);
        glUniform1i(Tex2Location, 1);

        //ARM
        Tex1Location = glGetUniformLocation(parm->pid, "tex");//tex, tex2... sampler in the fragment shader
        Tex2Location = glGetUniformLocation(parm->pid, "tex2");
        // Then bind the uniform samplers to texture units:
        glUseProgram(parm->pid);
        glUniform1i(Tex1Location, 0);
        glUniform1i(Tex2Location, 1);

        //PROJECTILE
        Tex1Location = glGetUniformLocation(pprojectile->pid, "tex");//tex, tex2... sampler in the fragment shader
        Tex2Location = glGetUniformLocation(pprojectile->pid, "tex2");
        // Then bind the uniform samplers to texture units:
        glUseProgram(pprojectile->pid);
        glUniform1i(Tex1Location, 0);
        glUniform1i(Tex2Location, 1);

        Tex1Location = glGetUniformLocation(psky->pid, "tex");//tex, tex2... sampler in the fragment shader
        Tex2Location = glGetUniformLocation(psky->pid, "tex2");
        // Then bind the uniform samplers to texture units:
        glUseProgram(psky->pid);
        glUniform1i(Tex1Location, 0);
        glUniform1i(Tex2Location, 1);

        Tex1Location = glGetUniformLocation(pslime->pid, "tex");
        Tex2Location = glGetUniformLocation(pslime->pid, "tex2");
        glUseProgram(pslime->pid);
        glUniform1i(Tex1Location, 2);
        glUniform1i(Tex2Location, 3);
    }

    //General OGL initialization - set OGL state here
    void init(const std::string &resourceDirectory) {
        GLSL::checkVersion();

        // Set background color.
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // Enable z-buffer test.
        glEnable(GL_DEPTH_TEST);
        //glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        //glDisable(GL_DEPTH_TEST);
        // Initialize the GLSL program.
        prog = std::make_shared<Program>();
        prog->setVerbose(true);
        prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
        if (!prog->init()) {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        prog->addUniform("P");
        prog->addUniform("V");
        prog->addUniform("M");
        prog->addUniform("campos");
        prog->addUniform("MatAmb");
        prog->addUniform("MatDif");
        prog->addUniform("MatSpec");
        prog->addUniform("MatShine");
        prog->addAttribute("vertPos");
        prog->addAttribute("vertNor");
        prog->addAttribute("vertTex");
        prog->addUniform("Opacity");

        pworld = std::make_shared<Program>();
        pworld->setVerbose(true);
        pworld->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
        if (!pworld->init()) {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        pworld->addUniform("P");
        pworld->addUniform("V");
        pworld->addUniform("M");
        pworld->addUniform("campos");
        pworld->addAttribute("vertPos");
        pworld->addAttribute("vertNor");
        pworld->addAttribute("vertTex");

        pplayer = std::make_shared<Program>();
        pplayer->setVerbose(true);
        pplayer->setShaderNames(resourceDirectory + "/shader_vertex_player.glsl",
                                resourceDirectory + "/shader_fragment_player.glsl");
        if (!pplayer->init()) {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        pplayer->addUniform("P");
        pplayer->addUniform("V");
        pplayer->addUniform("M");
        pplayer->addUniform("campos");
        pplayer->addAttribute("vertPos");
        pplayer->addAttribute("vertNor");
        pplayer->addAttribute("vertTex");

        parm = std::make_shared<Program>();
        parm->setVerbose(true);
        parm->setShaderNames(resourceDirectory + "/shader_vertex_player.glsl",
                                resourceDirectory + "/shader_fragment_player.glsl");
        if (!parm->init()) {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        parm->addUniform("P");
        parm->addUniform("V");
        parm->addUniform("M");
        parm->addUniform("campos");
        parm->addAttribute("vertPos");
        parm->addAttribute("vertNor");
        parm->addAttribute("vertTex");

        pprojectile = std::make_shared<Program>();
        pprojectile->setVerbose(true);
        pprojectile->setShaderNames(resourceDirectory + "/shader_vertex.glsl",
                                resourceDirectory + "/shader_fragment.glsl");
        if (!pprojectile->init()) {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        pprojectile->addUniform("P");
        pprojectile->addUniform("V");
        pprojectile->addUniform("M");
        pprojectile->addUniform("campos");
        pprojectile->addAttribute("vertPos");
        pprojectile->addAttribute("vertNor");
        pprojectile->addAttribute("vertTex");

        psky = std::make_shared<Program>();
        psky->setVerbose(true);
        psky->setShaderNames(resourceDirectory + "/skyvertex.glsl", resourceDirectory + "/skyfrag.glsl");
        if (!psky->init()) {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        psky->addUniform("dn");
        psky->addUniform("P");
        psky->addUniform("V");
        psky->addUniform("M");
        psky->addUniform("campos");
        psky->addAttribute("vertPos");
        psky->addAttribute("vertNor");
        psky->addAttribute("vertTex");

        pslime = std::make_shared<Program>();
        pslime->setVerbose(true);
        pslime->setShaderNames(resourceDirectory + "/slimevertex.glsl", resourceDirectory + "/slimefrag.glsl");
        if (!pslime->init()) {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        pslime->addUniform("P");
        pslime->addUniform("V");
        pslime->addUniform("M");
        pslime->addAttribute("vertPos");
        pslime->addAttribute("vertNor");
        pslime->addAttribute("vertTex");
        pslime->addUniform("part");
        pslime->addUniform("hit");


        for(int i = 0; i < NUM_ENEMIES; i++) {
            float x = (float)(rand() % 200 - 100) / 100.0f;
            float y = (float)(rand() % 200 - 100) / 100.0f;
            float z = (float)(rand() % 200 - 100) / 100.0f;
            vec3 forward = normalize(vec3(x, y, z));
            enemies.emplace_back(forward);
        }

        game_stats.push_back(player.health);
        game_stats.push_back(enemies.size());
    }

    /****DRAW
    This is the most important function in your program - this is where you
    will actually issue the commands to draw any geometry you have set up to
    draw
    ********/
    void render() {

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        double frametime = get_last_elapsed_time();
        totalTime += frametime;

        // Get current frame buffer size.
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        float aspect = width / (float) height;
        glViewport(0, 0, width, height);

        // Clear framebuffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        player.updateLocation(frametime);
        arm.updateLocation(frametime);

        glm::mat4 V, M, P; //View, Model and Perspective matrix
        V = player.camera();
        P = glm::perspective((float) (3.14159 / 4.), (float) ((float) width / (float) height), 0.1f,
                             1000.0f); //so much type casting... GLM metods are quite funny ones

        prog->bind();
        glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniform3fv(prog->getUniform("campos"), 1, &player.pos[0]);

        glBindVertexArray(VertexArrayID);
        //actually draw from vertex 0, 3 vertices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);

        M = player.getModel();
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        playerOBJ->draw(prog, GL_FALSE, playerMaterialLoader.materials);


//        if(worldCollision.didPlayerCollide(worldOBJ, player.fwd, player.height)) {
//            cout << "Player Collided!!! " << "height: " << player.height << endl;
//        }

        // draw player projectiles
        vector<int> projectilesToDelete = vector<int>();
        vector<int> enemiesHit = vector<int>();
        for(int i = 0; i < player_projectiles.size(); i++) {
            Projectile *proj = &player_projectiles[i];
            // check if collided with player
            if(proj->graceTimeLeft == 0 && collisions::detectSphereSphere(proj->hitbox, player.hitbox)) {
                player.health -= 1;
                projectilesToDelete.push_back(i);
                printf("Player hit!\n");
                continue;
            }
            bool hitEnemy = false;
            for(int j = 0; j < enemies.size(); j++) {
                if(collisions::detectSphereSphere(proj->hitbox, enemies[j].hitbox)) {
                    enemiesHit.push_back(j);
                    projectilesToDelete.push_back(i);
                    hitEnemy=true;
                }
            }
            if(hitEnemy) {
                printf("Enemy hit!\n");
                continue;
            }

            if (proj->lifespan > 0) {
                proj->rotateProj(frametime);
                M = proj->getModel();
                glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
                projectileOBJ->draw(prog, GL_FALSE, {});
            } else player_projectiles.erase(player_projectiles.begin() + i);

        }
        for(int i = projectilesToDelete.size() - 1; i >= 0; i--) {
            player_projectiles.erase(player_projectiles.begin() + projectilesToDelete[i]);
        }
        for(int i = enemiesHit.size() - 1; i >= 0; i--) {
            enemies.erase(enemies.begin() + enemiesHit[i]);
        }

        // draw enemies
        for(int i = 0; i < enemies.size(); i++) {
            if(enemies[i].updateEnemy(player.fwd, frametime)) {
                enemy_projectiles.push_back(enemies[i].spawnProjectile());
            };
            M = enemies[i].getModel();
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            playerOBJ->draw(prog, GL_FALSE, playerMaterialLoader.materials);
        }

        // draw enemy projectiles
        projectilesToDelete.clear();
        for(int i = 0; i < enemy_projectiles.size(); i++) {
            Projectile *proj = &enemy_projectiles[i];
            if(collisions::detectSphereSphere(proj->hitbox, player.hitbox)) {
                player.health -= 1;
                projectilesToDelete.push_back(i);
                printf("Player hit!\n");
                continue;
            }
            if(proj->lifespan > 0) {
                proj->rotateProj(frametime);
                M = proj->getModel();
                glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
                projectileOBJ->draw(prog, GL_FALSE, playerMaterialLoader.materials);
            } else
                enemy_projectiles.erase(enemy_projectiles.begin() + i);
        }
        for(int i = projectilesToDelete.size() - 1; i >= 0; i--) {
            enemy_projectiles.erase(enemy_projectiles.begin() + projectilesToDelete[i]);
        }

        if(player.health != game_stats[0] || enemies.size() != game_stats[1]) {
            printf("Player health: %d | Enemies left: %d\n", player.health, enemies.size());
            game_stats[0] = player.health;
            game_stats[1] = enemies.size();
        }

        if(enemies.size() == 0) {
            printf("You win!\n");
        }
        if(player.health == 0) {
            printf("Game over!\n");
            gameDone = true;
        }

        glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniform3fv(prog->getUniform("campos"), 1, &arm.pos[0]);

        glBindVertexArray(VertexArrayID);
        //actually draw from vertex 0, 3 vertices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);

        M = arm.getModel();
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        armOBJ->draw(prog, GL_FALSE, armMaterialLoader.materials);

        // Draw the box using GLSL.

        //send the matrices to the shaders
        glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniform3fv(prog->getUniform("campos"), 1, &player.pos[0]);

        glBindVertexArray(VertexArrayID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);

        M = world.getModel();

        glUniform3f(prog->getUniform("campos"), 2 * player.pos[0], 2 * player.pos[1], 2 * player.pos[2]);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        worldOBJ->draw(prog, GL_FALSE, worldMaterialLoader.materials);

        prog->unbind();

        glBindVertexArray(0);  
    }
};

//******************************************************************************************
int main(int argc, char **argv)
{
    srand(time(NULL));
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	//windowManager->init(1920, 1080);
    windowManager->init(960, 540);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}