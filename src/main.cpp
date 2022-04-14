/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/
#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION

#define PI 3.141596
#define NUM_SLIMES 20
#ifndef PLANE_SIZE
#define PLANE_SIZE 30
#define HPL_SIZE (PLANE_SIZE / 2.f)
#endif

#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "player.cpp"
#include "slime.cpp"
#include "collisions.h"
#include "boundingplane.h"

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
shared_ptr<Shape> shape;


double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}

// Player movement

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog,psky, pslime;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexNormDBox, VertexTexBox, IndexBufferIDBox;

    GLuint SlimeArrayId, SlimeBufferId, SlimeNormId, SlimeTexId, SlimeIndexId;

    Player player;

	//texture data
	GLuint Texture, TextureN;
	GLuint Texture2;
    GLuint TextureSlime;
    GLuint TextureSlime2;

    int numCollided = 0;
    boundingplane bpLeft = boundingplane(1.0f, 0.0f, 0.0f, -15.0f);
    boundingplane bpRight = boundingplane(-1.0f, 0.0f, 0.0f, -15.0f);
    boundingplane bpTop = boundingplane(0.0f, 0.0f, 1.0f, -15.0f);
    boundingplane bpDown = boundingplane(0.0f, 0.0f, -1.0f, -15.0f);

    vector<slime> slimes;
    bool done = false;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
        if(done) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
            player.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
            player.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
            player.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
            player.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
            player.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
            player.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
            player.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
            player.d = 0;
		}
        if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
            player.increaseSpeed();
        }
        if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
            player.decreaseSpeed();
        }
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow* window, double xpos, double ypos)
	{
        player.mouseCallback(window, xpos, ypos);
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
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
                -1.0, -1.0,  1.0,//LD
                1.0, -1.0,  1.0,//RD
                1.0,  1.0,  1.0,//RU
                -1.0,  1.0,  1.0,//LU
        };
        //make it a bit smaller
        for (int i = 0; i < 12; i++)
            rect_vertices[i] *= 0.5;
        //actually memcopy the data - only do this once
        glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_DYNAMIC_DRAW);

        //we need to set up the vertex array
        glEnableVertexAttribArray(0);
        //key function to get up how many elements to pull out at a time (3)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

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
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

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
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

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

    void initSlime() {
        //generate the VAO
        glGenVertexArrays(1, &SlimeArrayId);
        glBindVertexArray(SlimeArrayId);

        //generate vertex buffer to hand off to OGL
        glGenBuffers(1, &SlimeBufferId);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, SlimeBufferId);

        GLfloat cube_vertices[] = {
                // front
                -1.0, -1.0,  1.0,
                1.0, -1.0,  1.0,
                1.0,  1.0,  1.0,
                -1.0,  1.0,  1.0,
                // top
                -1.0, 1.0, 1.0,
                1.0, 1.0, 1.0,
                1.0, 1.0, -1.0,
                -1.0, 1.0, -1.0,
                // back
                1.0, -1.0, -1.0,
                -1.0, -1.0, -1.0,
                -1.0,  1.0, -1.0,
                1.0,  1.0, -1.0,
                // bottom
                -1.0, -1.0, -1.0,
                1.0, -1.0, -1.0,
                1.0, -1.0, 1.0,
                -1.0, -1.0, 1.0,
                // left
                -1.0, -1.0, -1.0,
                -1.0, -1.0, 1.0,
                -1.0, 1.0, 1.0,
                -1.0, 1.0, -1.0,
                // right
                1.0, -1.0, 1.0,
                1.0, -1.0, -1.0,
                1.0, 1.0, -1.0,
                1.0, 1.0, 1.0
        };
        //make it a bit smaller
        for (int i = 0; i < 72; i++)
            cube_vertices[i] *= 0.5;
        //actually memcopy the data - only do this once
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

        //we need to set up the vertex array
        glEnableVertexAttribArray(3);
        //key function to get up how many elements to pull out at a time (3)
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // cube norms
        GLfloat cube_norm[] = {
                // front norm
                0.0, 0.0, 1.0,
                0.0, 0.0, 1.0,
                0.0, 0.0, 1.0,
                0.0, 0.0, 1.0,
                // top norm
                0.0, 1.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 1.0, 0.0,
                // back norm
                0.0, 0.0, -1.0,
                0.0, 0.0, -1.0,
                0.0, 0.0, -1.0,
                0.0, 0.0, -1.0,
                //bottom norm
                0.0, -1.0, 0.0,
                0.0, -1.0, 0.0,
                0.0, -1.0, 0.0,
                0.0, -1.0, 0.0,
                // left norm
                -1.0, 0.0, 0.0,
                -1.0, 0.0, 0.0,
                -1.0, 0.0, 0.0,
                -1.0, 0.0, 0.0,
                //right norm
                1.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                1.0, 0.0, 0.0
        };

        glGenBuffers(1, &SlimeNormId);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, SlimeNormId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_norm), cube_norm, GL_STATIC_DRAW);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        //tex
        glm::vec2 cube_tex[] = {
                // front tex
                glm::vec2(0.0, 0.0),
                glm::vec2(1.0, 0.0),
                glm::vec2(1.0, 1.0),
                glm::vec2(0.0, 1.0),
                // top tex
                glm::vec2(0.0, 0.0),
                glm::vec2(1.0, 0.0),
                glm::vec2(1.0, 1.0),
                glm::vec2(0.0, 1.0),
                // back tex
                glm::vec2(1.0, 0.0),
                glm::vec2(0.0, 0.0),
                glm::vec2(0.0, 1.0),
                glm::vec2(1.0, 1.0),
                // bottom tex
                glm::vec2(0.0, 0.0),
                glm::vec2(1.0, 0.0),
                glm::vec2(1.0, 1.0),
                glm::vec2(0.0, 1.0),
                // left tex
                glm::vec2(0.0, 0.0),
                glm::vec2(1.0, 0.0),
                glm::vec2(1.0, 1.0),
                glm::vec2(0.0, 1.0),
                // right tex
                glm::vec2(0.0, 0.0),
                glm::vec2(1.0, 0.0),
                glm::vec2(1.0, 1.0),
                glm::vec2(0.0, 1.0),
        };

        glGenBuffers(1, &SlimeTexId);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, SlimeTexId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_tex), cube_tex, GL_STATIC_DRAW);
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glGenBuffers(1, &SlimeIndexId);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SlimeIndexId);
        GLushort cube_elements[] = {
                // front
                0, 1, 2,
                2, 3, 0,
                // top
                4, 5, 6,
                6, 7, 4,
                // back
                8, 9, 10,
                10, 11, 8,
                // bottom
                12, 13, 14,
                14, 15, 12,
                // left
                16, 17, 18,
                18, 19, 16,
                // right
                20, 21, 22,
                22, 23, 20,
        };
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
        initPlane();
        initSlime();
		glBindVertexArray(0);

		string resourceDirectory = "../resources" ;
		// Initialize mesh.
		shape = make_shared<Shape>();
		//shape->loadMesh(resourceDirectory + "/t800.obj");
		shape->loadMesh(resourceDirectory + "/sphere.obj");
		shape->resize();
		shape->init();

		int width, height, channels;
		char filepath[1000];

		//texture 1
		string str = resourceDirectory + "/grass.jpg";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
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
		str = resourceDirectory + "/sky.jpg";
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
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
        glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//glDisable(GL_DEPTH_TEST);
		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("campos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");


		psky = std::make_shared<Program>();
		psky->setVerbose(true);
		psky->setShaderNames(resourceDirectory + "/skyvertex.glsl", resourceDirectory + "/skyfrag.glsl");
		if (!psky->init())
		{
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
        if (!pslime->init())
        {
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


        // Initialize the slimes!!
        slimes = vector<slime>();
        for (int i = 0; i < NUM_SLIMES; i++) {
            slimes.emplace_back();
        }
        numCollided = 0;
        cout << "Remaining slimes: " << slimes.size() << endl;
        cout << "Collided with " << numCollided << " slimes" << endl;
    }


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		double frametime = get_last_elapsed_time();

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now

		static float playerX = 0.0;
		static float playerY = 0;
		static float playerZ = 0;

		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = player.process(frametime);
		M = glm::mat4(1);
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones
		
		float sangle = 3.1415926 / 2.;
		glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 camp = player.cameraPos;
		glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
		glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));

		M = TransSky * RotateXSky * SSky;


		psky->bind();		
		glUniformMatrix4fv(psky->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(psky->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(psky->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(psky->getUniform("campos"), 1, &player.pos[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);

		static float ttime = 0;
		ttime += frametime;
		float dn = sin(ttime)*0.5 +0.5;
		glUniform1f(psky->getUniform("dn"), dn);		
		glDisable(GL_DEPTH_TEST);
		shape->draw(psky, GL_FALSE);
		glEnable(GL_DEPTH_TEST);
		psky->unbind();


		glm::mat4 RotateX;
		glm::mat4 TransZ;
		glm::mat4 S;

        pslime->bind();
        glBindVertexArray(SlimeArrayId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SlimeIndexId);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, TextureSlime);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, TextureSlime2);

        glUniformMatrix4fv(pslime->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(pslime->getUniform("V"), 1, GL_FALSE, &V[0][0]);

        vector<int> delIndices = vector<int>();
        // update the slime, then draw it
        for(int i = 0; i < slimes.size(); i++) {
           slimes[i].update(frametime);
           // if the slime is dead, add it to the list of indices to delete
           if(drawSlime(&slimes[i])) {
               delIndices.push_back(i);
           }
        }
        for(int i = delIndices.size() - 1; i >= 0; i--) {
            slimes.erase(slimes.begin() + delIndices[i]);
            cout << "Remaining slimes: " << slimes.size() << endl;
            if(slimes.size() == 0) {
                cout << "You win!" << endl;
                done = true;
            }
        }

        for(int i = 0; i < slimes.size(); i++) {
            if(collisions::detectSphereSphere(slimes[i].sphere, player.sphere) && slimes[i].timeLeft == 0) {
                numCollided += 1;
                cout << "Collided with " << numCollided << " slimes" << endl;
                slimes[i].startTimer();
            }



            if(collisions::detectPlaneSphere(bpLeft, slimes[i].sphere)) {
                slimes[i].dir.x = -abs(slimes[i].dir.x);
            } else if(collisions::detectPlaneSphere(bpRight, slimes[i].sphere)) {
                slimes[i].dir.x = abs(slimes[i].dir.x);
            } else if(collisions::detectPlaneSphere(bpDown, slimes[i].sphere)) {
                slimes[i].dir.y = abs(slimes[i].dir.y);
            } else if(collisions::detectPlaneSphere(bpTop, slimes[i].sphere)) {
                slimes[i].dir.y = -abs(slimes[i].dir.y);
            }
        }

        for(int i = 0; i < slimes.size(); i++) {
            for (int j = i + 1; j < slimes.size(); j++) {
                if(collisions::detectSphereSphere(slimes[j].sphere, slimes[i].sphere)) {
                    vec2 dir = slimes[j].dir + slimes[i].dir;
                    if(dir.x == 0 && dir.y == 0) {
                        continue;
                    }
                    dir = normalize(dir);
                    slimes[i].dir = -dir;
                    slimes[j].dir = dir;
                }
            }
        }
        pslime->unbind();

		// Draw the box using GLSL.
		prog->bind();
		//send the matrices to the shaders
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
		
		TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.3f, 0.0));
		S = glm::scale(glm::mat4(1.0f), glm::vec3(PLANE_SIZE, PLANE_SIZE, 0.f));
		float angle = 3.1415926 / 2.0f;
        RotateX = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));

		M = TransZ *  RotateX * S;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
        prog->unbind();


        glBindVertexArray(0);
    }

    bool drawSlime(slime *s) {
        glUniform1i(pslime->getUniform("hit"), s->timeLeft != 0);
        mat4 M_body = s->getBody();
        mat4 M = M_body;
        glUniform1i(pslime->getUniform("part"), 0);
        glUniformMatrix4fv(pslime->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

        // draw the left eye
        M = s->getLeftEye();
        glUniform1i(pslime->getUniform("part"), 1);
        glUniformMatrix4fv(pslime->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

        M = s->getRightEye();
        glUniform1i(pslime->getUniform("part"), 1);
        glUniformMatrix4fv(pslime->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

        M = s->getSmile();
        glUniform1i(pslime->getUniform("part"), 2);
        glUniformMatrix4fv(pslime->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
        if(s->timeLeft == 0) {
            return false;
        }
        else if(s->timeLeft < 1.0f) {
            return true;
        }

        return false;
    }
};
//******************************************************************************************
int main(int argc, char **argv)
{
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
