#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Screen states
enum class Screen
{
    MAIN_MENU,
    RED_DEMO,
    BLUE_DEMO,
    GREEN_DEMO,
    YELLOW_DEMO
};

Screen currentScreen = Screen::MAIN_MENU;

// Button structure
struct Button
{
    float x, y, width, height;
    glm::vec3 color;
    const char *text;
};

// Button definitions for main menu
Button buttons[] = {
    {-0.3f, 0.3f, 0.6f, 0.15f, glm::vec3(1.0f, 0.0f, 0.0f), "Red Demo"},     // Red
    {-0.3f, 0.05f, 0.6f, 0.15f, glm::vec3(0.0f, 0.0f, 1.0f), "Blue Demo"},   // Blue
    {-0.3f, -0.2f, 0.6f, 0.15f, glm::vec3(0.0f, 1.0f, 0.0f), "Green Demo"},  // Green
    {-0.3f, -0.45f, 0.6f, 0.15f, glm::vec3(1.0f, 1.0f, 0.0f), "Yellow Demo"} // Yellow
};

const int NUM_BUTTONS = 4;

// Back button for demo screens
Button backButton = {-0.95f, 0.8f, 0.3f, 0.12f, glm::vec3(0.2f, 0.2f, 0.2f), "Back"};

// Window dimensions
int windowWidth = 800;
int windowHeight = 600;

// Mouse position
double mouseX = 0.0;
double mouseY = 0.0;

// Vertex Shader source code
const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    
    uniform mat4 model;
    uniform mat4 projection;
    
    out vec3 ourColor;
    
    void main()
    {
        gl_Position = projection * model * vec4(aPos, 1.0);
        ourColor = aColor;
    }
)";

// Fragment Shader source code
const char *fragmentShaderSource = R"(
    #version 330 core
    in vec3 ourColor;
    out vec4 FragColor;
    
    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
)";

// Error callback for GLFW
void errorCallback(int error, const char *description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

// Window resize callback
void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

// Mouse position callback
void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    mouseX = xpos;
    mouseY = ypos;
}

// Mouse button callback
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Convert mouse coordinates to normalized device coordinates
        float normalizedX = (2.0f * mouseX) / windowWidth - 1.0f;
        float normalizedY = 1.0f - (2.0f * mouseY) / windowHeight;

        if (currentScreen == Screen::MAIN_MENU)
        {
            // Check if any main menu button was clicked
            for (int i = 0; i < NUM_BUTTONS; i++)
            {
                if (normalizedX >= buttons[i].x && normalizedX <= buttons[i].x + buttons[i].width &&
                    normalizedY >= buttons[i].y && normalizedY <= buttons[i].y + buttons[i].height)
                {
                    // Navigate to corresponding demo
                    switch (i)
                    {
                    case 0:
                        currentScreen = Screen::RED_DEMO;
                        break;
                    case 1:
                        currentScreen = Screen::BLUE_DEMO;
                        break;
                    case 2:
                        currentScreen = Screen::GREEN_DEMO;
                        break;
                    case 3:
                        currentScreen = Screen::YELLOW_DEMO;
                        break;
                    }
                    break;
                }
            }
        }
        else
        {
            // Check if back button was clicked
            if (normalizedX >= backButton.x && normalizedX <= backButton.x + backButton.width &&
                normalizedY >= backButton.y && normalizedY <= backButton.y + backButton.height)
            {
                currentScreen = Screen::MAIN_MENU;
            }
        }
    }
}

// Process input
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        if (currentScreen != Screen::MAIN_MENU)
        {
            currentScreen = Screen::MAIN_MENU;
        }
        else
        {
            glfwSetWindowShouldClose(window, true);
        }
    }
}

// Create a rectangle vertex data
void createRectangle(float x, float y, float width, float height, glm::vec3 color, float vertices[], int &vertexIndex)
{
    // Top-left
    vertices[vertexIndex++] = x;
    vertices[vertexIndex++] = y + height;
    vertices[vertexIndex++] = 0.0f;
    vertices[vertexIndex++] = color.r;
    vertices[vertexIndex++] = color.g;
    vertices[vertexIndex++] = color.b;

    // Top-right
    vertices[vertexIndex++] = x + width;
    vertices[vertexIndex++] = y + height;
    vertices[vertexIndex++] = 0.0f;
    vertices[vertexIndex++] = color.r;
    vertices[vertexIndex++] = color.g;
    vertices[vertexIndex++] = color.b;

    // Bottom-right
    vertices[vertexIndex++] = x + width;
    vertices[vertexIndex++] = y;
    vertices[vertexIndex++] = 0.0f;
    vertices[vertexIndex++] = color.r;
    vertices[vertexIndex++] = color.g;
    vertices[vertexIndex++] = color.b;

    // Top-left
    vertices[vertexIndex++] = x;
    vertices[vertexIndex++] = y + height;
    vertices[vertexIndex++] = 0.0f;
    vertices[vertexIndex++] = color.r;
    vertices[vertexIndex++] = color.g;
    vertices[vertexIndex++] = color.b;

    // Bottom-right
    vertices[vertexIndex++] = x + width;
    vertices[vertexIndex++] = y;
    vertices[vertexIndex++] = 0.0f;
    vertices[vertexIndex++] = color.r;
    vertices[vertexIndex++] = color.g;
    vertices[vertexIndex++] = color.b;

    // Bottom-left
    vertices[vertexIndex++] = x;
    vertices[vertexIndex++] = y;
    vertices[vertexIndex++] = 0.0f;
    vertices[vertexIndex++] = color.r;
    vertices[vertexIndex++] = color.g;
    vertices[vertexIndex++] = color.b;
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set GLFW error callback
    glfwSetErrorCallback(errorCallback);

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "Physics Demo Suite", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set initial viewport
    glViewport(0, 0, windowWidth, windowHeight);

    // Build and compile our shader program
    // Vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for vertex shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // Fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for fragment shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // Link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    // Delete shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create vertex data for buttons (6 vertices per button, 6 floats per vertex)
    float buttonVertices[NUM_BUTTONS * 6 * 6]; // 6 vertices * 6 floats per vertex * num buttons
    int vertexIndex = 0;

    for (int i = 0; i < NUM_BUTTONS; i++)
    {
        createRectangle(buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height, buttons[i].color, buttonVertices, vertexIndex);
    }

    // Back button vertex data
    float backButtonVertices[6 * 6];
    int backVertexIndex = 0;
    createRectangle(backButton.x, backButton.y, backButton.width, backButton.height, backButton.color, backButtonVertices, backVertexIndex);

    // Vertex Buffer Object (VBO) and Vertex Array Object (VAO) for main menu buttons
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buttonVertices), buttonVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // VBO/VAO for back button
    unsigned int backVBO, backVAO;
    glGenVertexArrays(1, &backVAO);
    glGenBuffers(1, &backVBO);
    glBindVertexArray(backVAO);
    glBindBuffer(GL_ARRAY_BUFFER, backVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backButtonVertices), backButtonVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Get uniform locations
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Render
        if (currentScreen == Screen::MAIN_MENU)
        {
            // Set clear color (black background)
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Draw the buttons
            glUseProgram(shaderProgram);
            glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glm::mat4 model = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, NUM_BUTTONS * 6); // 6 vertices per button
        }
        else
        {
            // Demo screens
            glm::vec3 demoColor;
            const char *demoName;

            switch (currentScreen)
            {
            case Screen::RED_DEMO:
                demoColor = glm::vec3(1.0f, 0.0f, 0.0f);
                demoName = "Red Demo";
                break;
            case Screen::BLUE_DEMO:
                demoColor = glm::vec3(0.0f, 0.0f, 1.0f);
                demoName = "Blue Demo";
                break;
            case Screen::GREEN_DEMO:
                demoColor = glm::vec3(0.0f, 1.0f, 0.0f);
                demoName = "Green Demo";
                break;
            case Screen::YELLOW_DEMO:
                demoColor = glm::vec3(1.0f, 1.0f, 0.0f);
                demoName = "Yellow Demo";
                break;
            default:
                demoColor = glm::vec3(0.5f, 0.5f, 0.5f);
                demoName = "Unknown Demo";
                break;
            }

            // Set clear color to demo color
            glClearColor(demoColor.r, demoColor.g, demoColor.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Draw the back button
            glUseProgram(shaderProgram);
            glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glm::mat4 model = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(backVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Optional: De-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &backVAO);
    glDeleteBuffers(1, &backVBO);
    glDeleteProgram(shaderProgram);

    // Clean up
    glfwTerminate();
    return 0;
}