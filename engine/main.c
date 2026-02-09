#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GLFW/glfw3.h>

#include "shader_manager.h"
#include "vec_mat_4x4.h"
#include "obj_importer.h"


#define WINDOW_DEFAULT_SIZE_X 		1024
#define WINDOW_DEFAULT_SIZE_Y 		768
#define M_PI           				3.14159265358979323846


//static sWindowManager wm;
static uint8_t idx_drawAs = 8;

void error_cb(int code, const char* desc)
{
	printf("ERROR %d, [%s]\n", code, desc);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	printf("Resized to w: %d\th: %d\n", width, height);
}

int init_gl(void)
{
	int res = 1;

	if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND))
	{
		glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
		glfwInitHint(GLFW_WAYLAND_LIBDECOR, GLFW_WAYLAND_DISABLE_LIBDECOR);
	}

	if (!glfwInit())
	{
		// handle init fail
		return 0;
	}

	/*
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("GLAD failed\n");
		return -1;
	}
	*/


	glfwSetErrorCallback(error_cb);


	return res;
}

void deinit_gl(void)
{
	glfwTerminate();
}

GLFWwindow* window_create(void)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	GLFWwindow* w = glfwCreateWindow(
		WINDOW_DEFAULT_SIZE_X,
		WINDOW_DEFAULT_SIZE_Y,
		"GLFW Window",
		NULL,
		NULL
	);

	glfwSetFramebufferSizeCallback(w, framebuffer_size_callback);

	if (w == NULL)
	{
		printf("Failed to create window");
	}
	return w;
}


typedef enum {
	DIR_LEFT 	= 0b00000001,
	DIR_RIGHT 	= 0b00000010,
	DIR_UP 		= 0b00000100,
	DIR_DOWN 	= 0b00001000,
} eDirection;

static uint8_t directional_move = 0;
static uint8_t a_pressed = 0;

#define GLFW_KEY_NONE

static uint8_t s_pressed = 0;
static uint8_t s_pressed_code = 0;
static e_axis rotationAxis = ax_z;

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
        glfwSetWindowShouldClose(window, 1);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (!s_pressed)
		{
			s_pressed = true;
			fprintf(stdout, "S pressed\n");
			switch (s_pressed_code)
			{
			case 0:
				rotationAxis = ax_x;
				break;
			case 1:
				rotationAxis = ax_y;
				break;
			case 2:
				rotationAxis = ax_z;
				break;
			default:
				rotationAxis = ax_x;
				break;
			}
			s_pressed_code = (s_pressed_code + 1) % 3;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
	{
		s_pressed = false;
	}

	// WATCH OUT: the glfw spams all the time a  key is pressed
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		directional_move |= DIR_LEFT;
	} 
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE)
	{
		directional_move &= ~DIR_LEFT;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		directional_move |= DIR_RIGHT;
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
	{
		directional_move &= ~DIR_RIGHT;
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		directional_move |= DIR_UP;
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)
	{
		directional_move &= ~DIR_UP;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		directional_move |= DIR_DOWN;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
	{
		directional_move &= ~DIR_DOWN;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		if (a_pressed == 0)
		{
			idx_drawAs = (idx_drawAs + 1) % 11;
			a_pressed = 1;
			printf("idx_drawAs: %d", idx_drawAs);
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
	{
		a_pressed = 0;
	}
}


GLuint LoadTextureFromFile(char* filename)
{
	int w;
	int h;
	int nrChannels;
	unsigned int t;

	stbi_set_flip_vertically_on_load(true);
	uint8_t * data = stbi_load(filename, &w, &h, &nrChannels, 0);
	if (data == NULL)
	{
		printf("Epic fail - no texture to load\n");
		return 0;
	}

	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	return t;
}


typedef struct {
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	gpu_mesh_data* gpu;
} s_drawable;



s_drawable* create_drawable()
{
    s_obj_parsed_buff data;

	s_drawable* d = (s_drawable*)malloc(sizeof(s_drawable));

    obj_parse_file(&data, "./assets/objects/basic_ship_textured.obj");
    //obj_parse_file(&data, "./assets/objects/basic_cube.obj");
    //obj_parse_file(&data, "./assets/objects/teapot.obj");

	d->gpu = obj_cook_mesh_data(&data, OBJ_MESH_VERT | OBJ_MESH_NORMAL | OBJ_MESH_TEXTURE);

    obj_destroy_data_buffer(&data);

	glGenVertexArrays(1, &d->VAO);
	glGenBuffers(1, &d->VBO);
	//glGenBuffers(1, &d->EBO);
	
	glBindVertexArray(d->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, d->VBO);
	glBufferData(GL_ARRAY_BUFFER, d->gpu->vert_count * d->gpu->stride, d->gpu->data, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, d->gpu->vert_count * sizeof(int), d->gpu->indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, d->gpu->stride, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, d->gpu->stride, (void*)(3* sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, d->gpu->stride, (void*)(6* sizeof(float)));
	glEnableVertexAttribArray(2);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return d;
}

void destroy_drawable(s_drawable* d)
{
	if (!d) return;
	glDeleteVertexArrays(1, &d->VAO);
	glDeleteBuffers(1, &d->VBO);
	//glDeleteBuffers(1, &d->EBO);

	if (d->gpu) destroy_mesh_data(d->gpu);
	free(d);
}

void draw(s_drawable* d, GLuint shaderProgram, uint64_t drawAs)
{
	glBindVertexArray(d->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, d->VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->EBO);
	//glDrawElements(drawAs, d->gpu->vert_count, GL_UNSIGNED_INT, 0);
	glDrawArrays(drawAs, 0, d->gpu->vert_count);
	glBindVertexArray(0);
}


static double sysTick = 0;

typedef struct {
	pthread_t thread_id;
	double*   time_ptr;
} s_thread_info;


static uint8_t shouldPrintTime = 0;
static void* pthread_1_second(void* arg)
{
	for (;;)
	{
		sleep(1);
		shouldPrintTime = 1;
	}
	return NULL;
}

void testCb(int k, int a);

void key_callback(
	GLFWwindow* w,
	int key,
	int scancode,
	int action,
	int mods)
{
	//fprintf(stdout, "key = %04X, scancode = %04X action = %04X mods = %04X\n",
	//key, scancode, action, mods);

	if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(w, true);
	if (action == GLFW_PRESS | action == GLFW_REPEAT)
	{
		testCb(key, action);
	}
}


static mat_4x4f model_matrix;
static mat_4x4f view_matrix;
static mat_4x4f projection_matrix;

void window_size_callback(GLFWwindow* win, int w, int h)
{
	/*
	ortho_projection_mat_4x4f(
		projection_matrix,
		0,
		w,
		0,
		h,
		0.1f,
		1000.0f
	);
	*/
	perspective_projection_mat_4x4f(
		projection_matrix, RADS(60), 0.1f, 500.0f, ((float)w / (float)h));
}
void init_callbacks(GLFWwindow* w)
{
	glfwSetKeyCallback(w, key_callback);
	glfwSetWindowSizeCallback(w, window_size_callback);
}

void print_gl_draw_mode(uint64_t m)
{
	char msg[32];
	switch (m)
	{
	case GL_POINTS:
		sprintf(msg, "%s", "GL_POINTS");
		break;
	case GL_LINE_STRIP:
		sprintf(msg, "%s","GL_LINE_STRIP");
		break;
	case GL_LINE_LOOP:
		sprintf(msg, "%s","GL_LINE_LOOP");
		break;
	case GL_LINES:
		sprintf(msg, "%s","GL_LINES");
		break;
	case GL_LINE_STRIP_ADJACENCY:
		sprintf(msg, "%s","GL_LINE_STRIP_ADJACENCY");
		break;
	case GL_LINES_ADJACENCY:
		sprintf(msg, "%s","GL_LINES_ADJACENCY");
		break;
	case GL_TRIANGLE_STRIP:
		sprintf(msg, "%s","GL_TRIANGLE_STRIP");
		break;
	case GL_TRIANGLE_FAN:
		sprintf(msg, "%s","GL_TRIANGLE_FAN");
		break;
	case GL_TRIANGLES:
		sprintf(msg, "%s","GL_TRIANGLES");
		break;
	case GL_TRIANGLE_STRIP_ADJACENCY:
		sprintf(msg, "%s","GL_TRIANGLE_STRIP_ADJACENCY");
		break;
	case GL_TRIANGLES_ADJACENCY:
		sprintf(msg, "%s","GL_TRIANGLES_ADJACENCY");
		break;
	}
	//printf("GL Draw Mode: [%s]\n", msg);
}

uint64_t drawAs[] = {
	GL_POINTS,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_LINES,
	GL_LINE_STRIP_ADJACENCY,
	GL_LINES_ADJACENCY,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP_ADJACENCY,
	GL_TRIANGLES_ADJACENCY
};


#define MOVE_AMOUNT 0.1f
void testCb(int k, int a)
{
	if (a == GLFW_PRESS)
	{
		switch (k)
		{
			case GLFW_KEY_R:
				rotationAxis = (rotationAxis + 1) % 3;
				break;
			case GLFW_KEY_W:
				translate_mat_4x4f(view_matrix, (vec_4f){0, -MOVE_AMOUNT, 0, 0});
				break;
			case GLFW_KEY_S:
				translate_mat_4x4f(view_matrix, (vec_4f){0, MOVE_AMOUNT, 0, 0});
				break;
			case GLFW_KEY_A:
				translate_mat_4x4f(view_matrix, (vec_4f){MOVE_AMOUNT, 0, 0, 0});
				break;
			case GLFW_KEY_D:
				translate_mat_4x4f(view_matrix, (vec_4f){-MOVE_AMOUNT, 0, 0, 0});
				break;
			case GLFW_KEY_Q:
				translate_mat_4x4f(view_matrix, (vec_4f){0, 0, -MOVE_AMOUNT, 0});
				break;
			case GLFW_KEY_E:
				translate_mat_4x4f(view_matrix, (vec_4f){0, 0, MOVE_AMOUNT, 0});
				break;
			default:
				break;
		}
	}
	else if (a == GLFW_REPEAT)
	{
		switch (k)
		{
			case GLFW_KEY_W:
				translate_mat_4x4f(view_matrix, (vec_4f){0, -MOVE_AMOUNT, 0, 0});
				break;
			case GLFW_KEY_S:
				translate_mat_4x4f(view_matrix, (vec_4f){0, MOVE_AMOUNT, 0, 0});
				break;
			case GLFW_KEY_A:
				translate_mat_4x4f(view_matrix, (vec_4f){MOVE_AMOUNT, 0, 0, 0});
				break;
			case GLFW_KEY_D:
				translate_mat_4x4f(view_matrix, (vec_4f){-MOVE_AMOUNT, 0, 0, 0});
				break;
			case GLFW_KEY_Q:
				translate_mat_4x4f(view_matrix, (vec_4f){0, 0, -MOVE_AMOUNT, 0});
				break;
			case GLFW_KEY_E:
				translate_mat_4x4f(view_matrix, (vec_4f){0, 0, MOVE_AMOUNT, 0});
				break;
			default:
				break;
		}
	}
}

int main(int argc, char** argv)
{
	pthread_t pt;
	pthread_create(&pt, NULL, &pthread_1_second, NULL);

	printf("Sizeof drawAs: %lu\n", sizeof(drawAs));
	int res_init = init_gl();
	printf("GL init: %d\n", res_init);

	uint64_t timerFreq = glfwGetTimerFrequency();
	printf("GLFW timer %lu MHz\n", (timerFreq / 1000000UL));

	identity_mat_4x4f(model_matrix);
	//translate_mat_4x4f(model_matrix, (vec_4f){WINDOW_DEFAULT_SIZE_X/2,WINDOW_DEFAULT_SIZE_Y/2,0, 1.0});
	//translate_mat_4x4f(model_matrix, (vec_4f){0.0f, 0.0f, -2.0f, 1.0f});

	identity_mat_4x4f(view_matrix);

	identity_mat_4x4f(projection_matrix);

	perspective_projection_mat_4x4f(
		projection_matrix,
		RADS(60),
		0.1f,
		500.0f,
		(float)WINDOW_DEFAULT_SIZE_X / (float)WINDOW_DEFAULT_SIZE_Y
	);

	GLFWwindow* w = window_create();
	//WManagerAddWindow(&wm, w);

	glfwMakeContextCurrent(w);
	//glfwSwapInterval(0);	// Disable V-Sync - uncomment this line

	init_callbacks(w);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF);


	GLuint shaderProgram = createShaderProgramFromFile(
		"./shaders/basic_vertex.glsl",
		"./shaders/basic_fragment.glsl"
	);
	printf("Shader program: [%d]\n", shaderProgram);


	GLuint myTex = LoadTextureFromFile("./assets/textures/ship_texture.png");
	glBindTexture(GL_TEXTURE_2D, myTex);

	GLint modelMatGLLoc = glGetUniformLocation(shaderProgram, "modelMat");
	GLint viewMatGLLoc = glGetUniformLocation(shaderProgram, "viewMat");
	GLint projMatGLLoc = glGetUniformLocation(shaderProgram, "projectionMat");

	glUseProgram(shaderProgram);

	s_drawable* drawable = create_drawable();

	while(!glfwWindowShouldClose(w))
	{
		sysTick = glfwGetTime();

		if(shouldPrintTime)
		{
			printf("sysTick = %lf\n", sysTick);
			shouldPrintTime = 0;
			print_gl_draw_mode(drawAs[idx_drawAs]);
		}

		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		rotate_mat_4x4f(model_matrix, RADS(1), rotationAxis);
		//rotate_mat_4x4f(model_matrix, RADS(-1), rotationAxis);
		glUniformMatrix4fv(modelMatGLLoc, 1, GL_TRUE, model_matrix);
		glUniformMatrix4fv(viewMatGLLoc, 1, GL_TRUE, view_matrix);
		glUniformMatrix4fv(projMatGLLoc, 1, GL_FALSE, projection_matrix);

		draw(drawable, shaderProgram, drawAs[idx_drawAs]);

		//glReadPixels(480,350,50,50,GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer);

		glFinish();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}

	destroy_drawable(drawable);
	glfwDestroyWindow(w);
	deinit_gl();
	return 0;
}
