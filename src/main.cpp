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
#define NUM_ENEMIES 3

// #include "stb_image.h"
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
#include "camera.cpp"
// #include "particleSys.h"
#include "Texture.h"


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
shared_ptr<Shape> enemyOBJ;
shared_ptr<Shape> projectileOBJ;
shared_ptr<Shape> armOBJ;
shared_ptr<Shape> asteroidOBJ;
shared_ptr<Shape> hpbarOBJ;
shared_ptr<Shape> coneOBJ;



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
    MaterialLoader playerMaterialLoader = MaterialLoader("../resources/player-character.mtl");
    MaterialLoader armMaterialLoader = MaterialLoader("../resources/new-wizard-arm2.mtl");
    MaterialLoader asteroidMaterialLoader = MaterialLoader("../resources/asteroid.mtl");
    MaterialLoader fireballMaterialLoader = MaterialLoader("../resources/fireball.mtl");
    MaterialLoader hpbarMaterialLoader = MaterialLoader("../resources/hpbar3.mtl");
    MaterialLoader enemyMaterialLoader = MaterialLoader("../resources/enemy-character.mtl");
    MaterialLoader coneMaterialLoader = MaterialLoader("../resources/player-aim.mtl");

    // Our shader program
    std::shared_ptr<Program> prog, hud;
    MaterialLoader enemyMaterialLoader1 = MaterialLoader("../resources/enemy1.mtl");
    MaterialLoader enemyMaterialLoader2 = MaterialLoader("../resources/enemy2.mtl");


    // Our shader program for particles
	std::shared_ptr<Program> partProg;
    //the partricle system
	particleSys *thePartSystem;

    //PARTICLE SYSTEM
	// OpenGL handle to texture data used in particle
	std::shared_ptr<Texture> texture;
	bool keyToggles[256] = { false };
	//some particle variables

	float t = 0.0f; //reset in init
	float h = 0.01f;

    // Contains vertex information for OpenGL
    GLuint VertexArrayID;

    // Data necessary to give our box to OpenGL
    GLuint VertexBufferID, VertexNormDBox, VertexTexBox, IndexBufferIDBox;

    vector<Projectile> player_projectiles;
    vector<Projectile> enemy_projectiles;

    Player player;
    Arm arm;
    World world;
    Camera camera;
    vector<Enemy> enemies;
    vector<Enemy> enemies2;
    vector<int> game_stats;
    bool gameDone = false;

    //texture data
    GLuint Texture1;
    GLuint Texture2;
    GLuint TextureSlime;
    GLuint TextureSlime2;

    float totalTime = 0;
    int isMainView = 1;

    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if(gameDone)
            glfwSetWindowShouldClose(window, GL_TRUE);
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        if (key == GLFW_KEY_W && action == GLFW_PRESS) {
            camera.w = 1;
            world.w = 1;
            player.w = 1;
            arm.w = 1;
        }
        if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
            camera.w = 0;
            world.w = 0;
            player.w = 0;
            arm.w = 0;
        }
        if (key == GLFW_KEY_S && action == GLFW_PRESS) {
            camera.s = 1;
            world.s = 1;
            player.s = 1;
            arm.s = 1;
        }
        if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
            camera.s = 0;
            world.s = 0;
            player.s = 0;
            arm.s = 0;
        }
        if (key == GLFW_KEY_A && action == GLFW_PRESS) {
            camera.a = 1;
            world.a = 1;
            player.a = 1;
            arm.a = 1;
        }
        if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
            camera.a = 0;
            world.a = 0;
            player.a = 0;
            arm.a = 0;
        }
        if (key == GLFW_KEY_D && action == GLFW_PRESS) {
            camera.d = 1;
            world.d = 1;
            player.d = 1;
            arm.d = 1;
        }
        if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
            camera.d = 0;
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
        if (key == GLFW_KEY_U && action == GLFW_PRESS) {
            player.health -= 1;
        }
        if (key == GLFW_KEY_I && action == GLFW_PRESS) {
            player.health += 1;
        }
        if (key == GLFW_KEY_T && action == GLFW_PRESS) {
            isMainView = isMainView ? 0 : 1;
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
        asteroidMaterialLoader.readMaterialFile();
        hpbarMaterialLoader.readMaterialFile();
        fireballMaterialLoader.readMaterialFile();
        enemyMaterialLoader1.readMaterialFile();
        enemyMaterialLoader2.readMaterialFile();
        enemyMaterialLoader.readMaterialFile();
        coneMaterialLoader.readMaterialFile();

        playerOBJ = make_shared<Shape>();
        playerOBJ->loadMesh(resourceDirectory + "/player-character.obj", (string *) "../resources/");
        playerOBJ->resize();
        playerOBJ->init();

        coneOBJ = make_shared<Shape>();
        coneOBJ->loadMesh(resourceDirectory + "/player-aim.obj", (string *) "../resources/");
        coneOBJ->resize();
        coneOBJ->init();

        hpbarOBJ = make_shared<Shape>();
        hpbarOBJ->loadMesh(resourceDirectory + "/hpbar3.obj", (string *) "../resources/");
        hpbarOBJ->resize();
        hpbarOBJ->init();


        armOBJ = make_shared<Shape>();
        armOBJ->loadMesh(resourceDirectory + "/new-wizard-arm2.obj", (string *) "../resources/");
        armOBJ->resize();
        armOBJ->init();

        enemyOBJ = make_shared<Shape>();
        enemyOBJ->loadMesh(resourceDirectory + "/enemy-character.obj", (string *) "../resources/");
        enemyOBJ->resize();
        enemyOBJ->init();

        enemyOBJ = make_shared<Shape>();
        enemyOBJ->loadMesh(resourceDirectory + "/enemy.obj", (string *) "../resources/");
        enemyOBJ->resize();
        enemyOBJ->init();

        projectileOBJ = make_shared<Shape>();
        projectileOBJ->loadMesh(resourceDirectory + "/fireball.obj", (string *) "../resources/");
        projectileOBJ->resize();
        projectileOBJ->init();

        asteroidOBJ = make_shared<Shape>();
        asteroidOBJ->loadMesh(resourceDirectory + "/asteroid.obj", (string *) "../resources/");
        asteroidOBJ->resize();
        asteroidOBJ->init();
    }
    void SetMaterial(shared_ptr<Program> curS, int i) {

    	switch (i) {
    		case 0:
    			glUniform3f(curS->getUniform("MatAmb"), 0.089, 0.035, 0.013);
    			glUniform3f(curS->getUniform("MatDif"), 0.89, 0.35, 0.13);
    			glUniform3f(curS->getUniform("MatSpec"), 0.45, 0.17, 0.06);
    			glUniform1f(curS->getUniform("MatShine"), 20.0);
    		break;
    		case 1:
    			glUniform3f(curS->getUniform("MatAmb"), 0.063, 0.75, 0.8);
    			glUniform3f(curS->getUniform("MatDif"), 0.2, 0.8, 0.9);
    			glUniform3f(curS->getUniform("MatSpec"), 0.1, 0.4, 0.45);
    			glUniform1f(curS->getUniform("MatShine"), 400.0);
    		break;
    		case 2:
    			glUniform3f(curS->getUniform("MatAmb"), 0.05, 0.01, 0.01);
    			glUniform3f(curS->getUniform("MatDif"), 0.9, 0.3, 0.1);
    			glUniform3f(curS->getUniform("MatSpec"), 0.45, 0.15, 0.05);
    			glUniform1f(curS->getUniform("MatShine"), 27.9);
    		break;
			case 3:
    			glUniform3f(curS->getUniform("MatAmb"), 0.04, 0.04, 0.04);
    			glUniform3f(curS->getUniform("MatDif"), 0.4, 0.4, 0.4);
    			glUniform3f(curS->getUniform("MatSpec"), 0.2, 0.2, 0.2);
    			glUniform1f(curS->getUniform("MatShine"), 27.9);
    		break;
			case 4:
    			glUniform3f(curS->getUniform("MatAmb"), 0.02, 0.02, 0.02);
    			glUniform3f(curS->getUniform("MatDif"), 0.2, 0.2, 0.2);
    			glUniform3f(curS->getUniform("MatSpec"), 0.1, 0.1, 0.1);
    			glUniform1f(curS->getUniform("MatShine"), 27.9);
    		break;
			case 5:
    			glUniform3f(curS->getUniform("MatAmb"), 0.01, 0.01, 0.01);
    			glUniform3f(curS->getUniform("MatDif"), 0.1, 0.1, 0.1);
    			glUniform3f(curS->getUniform("MatSpec"), 0.05, 0.05, 0.05);
    			glUniform1f(curS->getUniform("MatShine"), 227.9);
    		break;
			case 6:
    			glUniform3f(curS->getUniform("MatAmb"), 0.07, 0.07, 0.07);
    			glUniform3f(curS->getUniform("MatDif"), 0.7, 0.7, 0.7);
    			glUniform3f(curS->getUniform("MatSpec"), 0.035, 0.035, 0.035);
    			glUniform1f(curS->getUniform("MatShine"), 500.9);
    		break;
			case 7:
    			glUniform3f(curS->getUniform("MatAmb"), 0.095, 0.075, 0.041);
    			glUniform3f(curS->getUniform("MatDif"), 0.95, 0.75, 0.41);
    			glUniform3f(curS->getUniform("MatSpec"), 0.47, 0.37, 0.21);
    			glUniform1f(curS->getUniform("MatShine"), 500.9);
    		break;
			case 8:
    			glUniform3f(curS->getUniform("MatAmb"), 0.095, 0.095, 0.095);
    			glUniform3f(curS->getUniform("MatDif"), 0.95, 0.95, 0.95);
    			glUniform3f(curS->getUniform("MatSpec"), 0.5, 0.5, 0.5);
    			glUniform1f(curS->getUniform("MatShine"), 500.9);
    		break;
  		}
    }

    //General OGL initialization - set OGL state here
    void init(const std::string &resourceDirectory) {
        GLSL::checkVersion();

        CHECKED_GL_CALL(glEnable(GL_DEPTH_TEST));
		CHECKED_GL_CALL(glEnable(GL_BLEND));
		CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		CHECKED_GL_CALL(glPointSize(5.0f));

        // Set background color.
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // Enable z-buffer test.
        glEnable(GL_DEPTH_TEST);
//        glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
        prog->addUniform("Height");
        prog->addUniform("Aim");

        hud = std::make_shared<Program>();
        hud->setVerbose(true);
        hud->setShaderNames(resourceDirectory + "/shader_vertex_hud.glsl", resourceDirectory + "/shader_fragment_hud.glsl");
        if (!hud->init()) {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        hud->addUniform("P");
        hud->addUniform("V");
        hud->addUniform("M");
        hud->addUniform("campos");
        hud->addUniform("MatAmb");
        hud->addUniform("MatDif");
        hud->addUniform("MatSpec");
        hud->addUniform("MatShine");
        hud->addAttribute("vertPos");
        hud->addAttribute("vertNor");
        hud->addAttribute("vertTex");
        hud->addUniform("Opacity");
        hud->addUniform("Height");

        texture = std::make_shared<Texture>();
		texture->setFilename(resourceDirectory + "/alpha.bmp");
		texture->init();
		texture->setUnit(0);
		texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);


		// Initialize PARTICLE the GLSL program.
		partProg = make_shared<Program>();
		partProg->setVerbose(true);
		partProg->setShaderNames(
			resourceDirectory + "/lab10_vert.glsl",
			resourceDirectory + "/lab10_frag.glsl");
		if (! partProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		partProg->addUniform("P");
		partProg->addUniform("M");
		partProg->addUniform("V");
		partProg->addUniform("pColor");
		partProg->addUniform("alphaTexture");
		partProg->addAttribute("vertPos");
		partProg->addAttribute("vertCol");

		// thePartSystem = new particleSys(vec3(0, 0, 0));
		// thePartSystem->gpuSetup();


        for(int i = 0; i < NUM_ENEMIES; i++) {
            float x = (float)(rand() % 200 - 100) / 100.0f;
            float y = (float)(rand() % 200 - 100) / 100.0f;
            float z = (float)(rand() % 200 - 100) / 100.0f;
            vec3 forward = normalize(vec3(x, y, z));
            enemies.emplace_back(forward);
        }
        for(int i = 0; i < NUM_ENEMIES; i++) {
            float x = (float)(rand() % 200 - 100) / 100.0f;
            float y = (float)(rand() % 200 - 100) / 100.0f;
            float z = (float)(rand() % 200 - 100) / 100.0f;
            vec3 forward = normalize(vec3(x, y, z));
            enemies2.emplace_back(forward);
        }

        game_stats.push_back(player.health);
        game_stats.push_back(enemies.size() * 2);
    }

    void drawHUD() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        double frametime = get_last_elapsed_time();
        totalTime += frametime;

        // Get current frame buffer size.
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        float aspect = width / (float) height;


//        vector<float> playerstate = player.getState();
        camera.update(frametime, player.pos, player.up, player.right, player.fwd);

        glm::mat4 V, M, P; //View, Model and Perspective matrix
        // player cam
            V = lookAt(vec3(0, 0, -5), vec3(0, 0, 0), vec3(0, 1, 0));

            // top down
            P = glm::perspective((float) (3.14159 / 4.f), (float) ((float) width / (float) height), 0.1f,
                                 1000.0f); //so much type casting... GLM metods are quite funny ones
//        P = glm::perspective((float) (3.14159 / 4.f), (float) ((float) width / (float) height), 0.1f,1000.0f); //so much type casting... GLM metods are quite funny ones
            P = glm::ortho(-1, 1, -1, 1);

        hud->bind();
        glUniformMatrix4fv(hud->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(hud->getUniform("V"), 1, GL_FALSE, &V[0][0]);

        float offset = 2.5f;
        for (int i = 0; i < player.health; i++) {
            M = translate(mat4(1.0f), vec3(0, 0, 0))  * scale(mat4(1.0f), vec3(1, 1, 1));
            glUniformMatrix4fv(hud->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            hpbarOBJ->draw(hud, GL_FALSE, hpbarMaterialLoader.materials);
        }

        hud->unbind();
        glBindVertexArray(0);

    }

    void drawScene(bool camState) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        double frametime = get_last_elapsed_time();
        totalTime += frametime;

        // Get current frame buffer size.
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        float aspect = width / (float) height;


//        vector<float> playerstate = player.getState();
        player.updateLocation(frametime);
        camera.update(frametime, player.pos, player.up, player.right, player.fwd);
        arm.updateLocation(frametime);

        glm::mat4 V, M, P; //View, Model and Perspective matrix
        // player cam
        if(camState)
            V = camera.playerCam(player.pos);
        // top down
        else
            V = camera.topCam();

        // player cam
        if(camState)
            P = glm::perspective((float) (3.14159 / 5.5f), (float) ((float) width / (float) height), 0.01f,
                             100.0f); //so much type casting... GLM metods are quite funny ones
         // top down
         else
            P = glm::perspective((float) (3.14159 / 4.f), (float) ((float) width / (float) height), 0.1f,
                                 1000.0f); //so much type casting... GLM metods are quite funny ones
//        P = glm::perspective((float) (3.14159 / 4.f), (float) ((float) width / (float) height), 0.1f,1000.0f); //so much type casting... GLM metods are quite funny ones
        M = glm::mat4(1.f);

        prog->bind();
        glUniform1f(prog->getUniform("Aim"), 0);
        glUniform1f(prog->getUniform("Height"), -1.f);
        glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniform3fv(prog->getUniform("campos"), 1, &player.pos[0]);

        glBindVertexArray(VertexArrayID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, Texture);

        M = player.getModel();
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        playerOBJ->draw(prog, GL_FALSE, playerMaterialLoader.materials);

        M = player.getModelAim();
        glUniform1f(prog->getUniform("Aim"), 1);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        coneOBJ->draw(prog, GL_FALSE, coneMaterialLoader.materials);
        glUniform1f(prog->getUniform("Aim"), 0);


    //    if(worldCollision.didPlayerCollide(worldOBJ, player.fwd, player.height)) {
    //        cout << "Player Collided!!! " << "height: " << player.height << endl;
    //    }
       SetMaterial(prog, 1);

        // draw player projectiles
        vector<int> projectilesToDelete = vector<int>();
        vector<int> enemiesHit = vector<int>();
        vector<int> enemiesHit2 = vector<int>();

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
            for(int j = 0; j < enemies2.size(); j++) {
                if(collisions::detectSphereSphere(proj->hitbox, enemies2[j].hitbox)) {
                    enemiesHit2.push_back(j);
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
                projectileOBJ->draw(prog, GL_FALSE, fireballMaterialLoader.materials);
            } else player_projectiles.erase(player_projectiles.begin() + i);

        }

        for(int i = projectilesToDelete.size() - 1; i >= 0; i--) {
            player_projectiles.erase(player_projectiles.begin() + projectilesToDelete[i]);
        }

        for(int i = enemiesHit.size() - 1; i >= 0; i--) {
            enemies.erase(enemies.begin() + enemiesHit[i]);
        }
        for(int i = enemiesHit2.size() - 1; i >= 0; i--) {
            enemies2.erase(enemies2.begin() + enemiesHit2[i]);
        }

        // draw enemies orange
        for(int i = 0; i < enemies.size(); i++) {
            if(enemies[i].updateEnemy(player.fwd, frametime)) {
                enemy_projectiles.push_back(enemies[i].spawnProjectile());
            };
            M = enemies[i].getModel();
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            enemyOBJ->draw(prog, GL_FALSE, enemyMaterialLoader1.materials);
        }

        for(int i = 0; i < enemies2.size(); i++) {
            if(enemies2[i].updateEnemy(player.fwd, frametime)) {
                enemy_projectiles.push_back(enemies2[i].spawnProjectile());
            };
            M = enemies2[i].getModel();
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            enemyOBJ->draw(prog, GL_FALSE, enemyMaterialLoader2.materials);
//            enemyOBJ->draw(prog, GL_FALSE, enemyMaterialLoader.materials);
        }

        SetMaterial(prog, 2);
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
                projectileOBJ->draw(prog, GL_FALSE, fireballMaterialLoader.materials);
            } else
                enemy_projectiles.erase(enemy_projectiles.begin() + i);
        }

        for(int i = projectilesToDelete.size() - 1; i >= 0; i--) {
            enemy_projectiles.erase(enemy_projectiles.begin() + projectilesToDelete[i]);
        }

        if(player.health != game_stats[0] || enemies.size() + enemies2.size() != game_stats[1]) {
            printf("Player health: %d | Enemies left: %d\n", player.health, enemies.size() + enemies2.size());
            game_stats[0] = player.health;
            game_stats[1] = enemies.size() + enemies2.size();
        }

        if(enemies.size() + enemies2.size() == 0) {
            printf("You win!\n");
            gameDone = true;
        }
        if(player.health == 0) {
            printf("Game over!\n");
            gameDone = true;
        }

        // glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        // glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        // glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        // glUniform3fv(prog->getUniform("campos"), 1, &arm.pos[0]);

        // glBindVertexArray(VertexArrayID);
        // //actually draw from vertex 0, 3 vertices
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, Texture);

//        M = arm.getModel();
//        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
//        armOBJ->draw(prog, GL_FALSE, armMaterialLoader.materials);

        // Draw the box using GLSL.

        // //send the matrices to the shaders
        // glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        // glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        // glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        // glUniform3fv(prog->getUniform("campos"), 1, &player.pos[0]);

        // glBindVertexArray(VertexArrayID);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, Texture);

        M = world.getModel();

        glUniform1f(prog->getUniform("Height"), worldCollision.getHeight(player.fwd));
        glUniform3f(prog->getUniform("campos"), 2 * player.pos[0], 2 * player.pos[1], 2 * player.pos[2]);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);

        worldOBJ->draw(prog, GL_FALSE, worldMaterialLoader.materials);


        // glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &P[0][0]);


        M = player.getModelHealth();
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        hpbarOBJ->draw(prog, GL_FALSE, hpbarMaterialLoader.materials);

        prog->unbind();

        M = world.getModel();

         // Draw PARTICLES
		partProg->bind();
		texture->bind(partProg->getUniform("alphaTexture"));
        CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("P"), 1, GL_FALSE, &P[0][0]));
        CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("V"), 1, GL_FALSE, &V[0][0]));
		CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("M"), 1, GL_FALSE, &M[0][0]));
		CHECKED_GL_CALL(glUniform3f(partProg->getUniform("pColor"), 0.9, 0.8, 0.4));

		// thePartSystem->drawMe(partProg);
		// thePartSystem->update();

        for(int i = 0; i < player_projectiles.size(); i++) {
            Projectile *proj = &player_projectiles[i];
            proj->renderParticleSys(V, partProg);
        }
        // CHECKED_GL_CALL(glUniform3f(partProg->getUniform("pColor"), 0.2, 0.2, 0.9));

        for(int i = 0; i < enemy_projectiles.size(); i++) {
            Projectile *proj = &enemy_projectiles[i];
            proj->renderParticleSys(V, partProg);
        }

		partProg->unbind();

        glBindVertexArray(0);
    }

    /****DRAW
    This is the most important function in your program - this is where you
    will actually issue the commands to draw any geometry you have set up to
    draw
    ********/
    void render() {

        // Get current frame buffer size.
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);


        // player cam
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, width, height);
        drawScene(isMainView ? false : true);

        // top down cam
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(0, height - height / 6, width / 6, height / 6);
        // Clear framebuffer.
        drawScene(isMainView ? true : false);


//        glClear(GL_DEPTH_BUFFER_BIT);
//        glViewport(width - (width / 6) - 64, height - height / 15, width / 6, height / 15);
//        drawHUD();
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
	windowManager->init(1920, 1080);
//    windowManager->init(960, 540);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
    // application->initTex(resourceDir);
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