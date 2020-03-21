#include <iostream>
#include <cmath>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL.h>
// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"

#include "ply_loader.h"

template <typename T>
void display(T mat, int rows, int cols)
{
	for (int i=0; i<rows; i++)
	{
		for (int j=0; j<cols; j++)
		{
			std::cout << mat[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
bool    keys[1024];

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

glm::vec3 model_position(-133.183f, -25.7757f, -25.1166f);


// 设定摄像机的内外参---------------
float Rotation[3][3] = {
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0
};
float Camera_Pos[3] = {
    0.0, 0.0, 5.0
};

float K[3][3] = {
    720.0, 0.0, 400.0,
    0.0, 720.0, 300.0,
    0.0, 0.0, 1.0
};

glm::mat4 gen_ViewingMatrix(float r[3][3], float c[3])
{
    float t[3] = {
        -1 * (r[0][0] * c[0] + r[0][1] * c[1] + r[0][2] * c[2]),
        -1 * (r[1][0] * c[0] + r[1][1] * c[1] + r[1][2] * c[2]),
        -1 * (r[2][0] * c[0] + r[2][1] * c[1] + r[2][2] * c[2])
    };
    float view_arr[16] = {
        r[0][0], r[1][0], r[2][0], 0.0,
        r[0][1], r[1][1], r[2][1], 0.0,
        r[0][2], r[1][2], r[2][2], 0.0,
        t[0], t[1], t[2], 1.0
    };
    glm::mat4 viewing_Matrix = glm::make_mat4(view_arr);
    return viewing_Matrix;
}

glm::mat4 gen_Perspective(float k[3][3], float n, float f)
{
    assert(k[0][0] == k[1][1]);

    float r = n * k[0][2] / k[0][0];
    float l = -1 * r;
    float t = n * k[1][2] / k[0][0];
    float b = -1 * t;

    return glm::transpose(glm::mat4(
        2*n/(r-l), 0.0, 0.0, 0.0,
        0.0, 2*n/(t-b), 0.0, 0.0,
        0.0, 0.0, (n+f)/(n-f), 2*n*f/(n-f),
        0.0, 0.0, -1.0, 0.0
    ));
}

// ----------------

// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // GLFW Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    // OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // Build and compile our shader program
    Shader objShader("model.vs", "model.frag");

    Ply_Loader* ply_model = new Ply_Loader();
    ply_model->load_model("theInn.ply");
    GLfloat* vertices = ply_model->get_vertices();
    GLint num_points = ply_model->get_num_vertex();
    GLint length_vertices = ply_model->get_len();

    // 顶点属性
    GLuint VBO, VAO;

    glGenVertexArrays(1, &VAO); // 申请一个顶点数组,分配ID
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 绑定一个数组缓冲
    glBufferData(GL_ARRAY_BUFFER, length_vertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // 纹理贴图
    GLuint diffuseMap;
    glGenTextures(1, &diffuseMap);
    int width, height;
    unsigned char* image;
    // 载入图片
    image = SOIL_load_image("diffuse.png", &width, &height, 0, SOIL_LOAD_RGB);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // // Set texture units
    objShader.Use();
    glUniform1i(glGetUniformLocation(objShader.Program, "diffuse_map"),  0);


    // Game loop
    while (!glfwWindowShouldClose(window))
    {

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        glm::mat4 model = glm::mat4(1.0);
        model = glm::translate(model, model_position);

        glm::mat4 view = glm::mat4(1.0);
        // view = camera.GetViewMatrix();
        view = gen_ViewingMatrix(Rotation, Camera_Pos);

        glm::mat4 projection = gen_Perspective(K, 0.1f, 100.0f);
        // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

        // std::cout << "model matrix" << std::endl;
        // display(model, 4, 4);

        // std::cout << "view matrix" << std::endl;
        // display(view, 4, 4);

        // std::cout << "projection matrix" << std::endl;
        // display(projection, 4, 4);

        // Clear the colorbuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(objShader.Program, "model");
        GLint viewLoc  = glGetUniformLocation(objShader.Program,  "view");
        GLint projLoc  = glGetUniformLocation(objShader.Program,  "projection");
        // Pass the matrices to the objShader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, num_points);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// // Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}
