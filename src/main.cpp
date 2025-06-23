#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <ctime>

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
Button backButton = {-0.95f, 0.85f, 0.3f, 0.12f, glm::vec3(0.2f, 0.2f, 0.2f), "Back"};

// Window dimensions
int windowWidth = 800;
int windowHeight = 600;

// Mouse position
double mouseX = 0.0;
double mouseY = 0.0;

// Ball physics properties
struct Ball
{
    float x, y;
    float vx, vy;
    float radius;
    glm::vec3 color;
};

// Ball initialization function prototype
void initBalls(int count);

// Square mass initialization function prototype
void initSquareMasses(float massRatio);

// Reset blue squares to initial state
void resetSquares();

// Ball count selection buttons for red demo
struct BallCountButton
{
    float x, y, width, height;
    int count;
    const char *label;
};

BallCountButton ballCountButtons[] = {
    {-0.9f, 0.75f, 0.5f, 0.08f, 5, "5 Balls"},
    {-0.2f, 0.75f, 0.5f, 0.08f, 10, "10 Balls"},
    {0.5f, 0.75f, 0.5f, 0.08f, 50, "50 Balls"}};
const int NUM_BALL_COUNT_BUTTONS = 3;

// Mass selection buttons for blue demo
struct MassButton
{
    float x, y, width, height;
    float massRatio;
    const char *label;
};

MassButton massButtons[] = {
    {-0.9f, 0.6f, 0.5f, 0.08f, 1.0f, "Equal Mass"},
    {-0.2f, 0.6f, 0.5f, 0.08f, 2.0f, "2x Mass"},
    {0.5f, 0.6f, 0.5f, 0.08f, 10.0f, "10x Mass"}};
const int NUM_MASS_BUTTONS = 3;

// Ball array and count
const int MAX_BALLS = 50;
Ball balls[MAX_BALLS];
int NUM_BALLS = 5;

// Square physics properties
struct Square
{
    float x, y;
    float vx, vy;
    float size;
    float mass;
    glm::vec3 color;
};

Square square = {0.0f, 0.0f, 0.004f, 0.0f, 0.1f, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f)};
Square square2 = {0.3f, 0.0f, -0.003f, 0.0f, 0.1f, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f)};

const int NUM_SQUARES = 2;
Square squares[NUM_SQUARES] = {square, square2};

// Box boundaries
const float BOX_LEFT = -0.8f;
const float BOX_RIGHT = 0.8f;
const float BOX_TOP = 0.6f;
const float BOX_BOTTOM = -0.6f;

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
        else if (currentScreen == Screen::RED_DEMO)
        {
            // Check if a ball count button was clicked
            for (int i = 0; i < NUM_BALL_COUNT_BUTTONS; i++)
            {
                BallCountButton &b = ballCountButtons[i];
                if (normalizedX >= b.x && normalizedX <= b.x + b.width &&
                    normalizedY >= b.y && normalizedY <= b.y + b.height)
                {
                    initBalls(b.count);
                    return;
                }
            }
            // Check if back button was clicked
            if (normalizedX >= backButton.x && normalizedX <= backButton.x + backButton.width &&
                normalizedY >= backButton.y && normalizedY <= backButton.y + backButton.height)
            {
                currentScreen = Screen::MAIN_MENU;
            }
        }
        else if (currentScreen == Screen::BLUE_DEMO)
        {
            // Check if a mass button was clicked
            for (int i = 0; i < NUM_MASS_BUTTONS; i++)
            {
                MassButton &b = massButtons[i];
                if (normalizedX >= b.x && normalizedX <= b.x + b.width &&
                    normalizedY >= b.y && normalizedY <= b.y + b.height)
                {
                    initSquareMasses(b.massRatio);
                    resetSquares();
                    return;
                }
            }
            // Check if back button was clicked
            if (normalizedX >= backButton.x && normalizedX <= backButton.x + backButton.width &&
                normalizedY >= backButton.y && normalizedY <= backButton.y + backButton.height)
            {
                currentScreen = Screen::MAIN_MENU;
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

// Create a circle vertex data (approximated with triangles)
void createCircle(float centerX, float centerY, float radius, glm::vec3 color, float vertices[], int &vertexIndex, int segments = 32)
{
    for (int i = 0; i < segments; i++)
    {
        float angle1 = 2.0f * 3.14159f * i / segments;
        float angle2 = 2.0f * 3.14159f * (i + 1) / segments;

        // Center
        vertices[vertexIndex++] = centerX;
        vertices[vertexIndex++] = centerY;
        vertices[vertexIndex++] = 0.0f;
        vertices[vertexIndex++] = color.r;
        vertices[vertexIndex++] = color.g;
        vertices[vertexIndex++] = color.b;

        // First point
        vertices[vertexIndex++] = centerX + radius * cos(angle1);
        vertices[vertexIndex++] = centerY + radius * sin(angle1);
        vertices[vertexIndex++] = 0.0f;
        vertices[vertexIndex++] = color.r;
        vertices[vertexIndex++] = color.g;
        vertices[vertexIndex++] = color.b;

        // Second point
        vertices[vertexIndex++] = centerX + radius * cos(angle2);
        vertices[vertexIndex++] = centerY + radius * sin(angle2);
        vertices[vertexIndex++] = 0.0f;
        vertices[vertexIndex++] = color.r;
        vertices[vertexIndex++] = color.g;
        vertices[vertexIndex++] = color.b;
    }
}

// Ball initialization function
void initBalls(int count)
{
    NUM_BALLS = count;
    float radius = (count <= 5) ? 0.05f : (count <= 10 ? 0.035f : 0.018f);
    srand((unsigned int)glfwGetTime());
    for (int i = 0; i < count; i++)
    {
        float angle = 2.0f * 3.14159f * i / count;
        float r = 0.5f * ((float)rand() / RAND_MAX) + 0.2f;
        balls[i].x = r * cos(angle) * 0.7f;
        balls[i].y = r * sin(angle) * 0.7f;
        float speed = (count <= 5) ? 0.003f : (count <= 10 ? 0.0025f : 0.0015f);
        float theta = 2.0f * 3.14159f * ((float)rand() / RAND_MAX);
        balls[i].vx = speed * cos(theta);
        balls[i].vy = speed * sin(theta);
        balls[i].radius = radius;
        balls[i].color = glm::vec3(1.0f, 0.0f, 0.0f);
    }
}

// Update ball physics
void updateBall()
{
    // Update position for all balls
    for (int i = 0; i < NUM_BALLS; i++)
    {
        balls[i].x += balls[i].vx;
        balls[i].y += balls[i].vy;

        // Check collision with walls
        if (balls[i].x - balls[i].radius <= BOX_LEFT || balls[i].x + balls[i].radius >= BOX_RIGHT)
        {
            balls[i].vx = -balls[i].vx;
            // Clamp position to prevent sticking
            if (balls[i].x - balls[i].radius <= BOX_LEFT)
                balls[i].x = BOX_LEFT + balls[i].radius;
            if (balls[i].x + balls[i].radius >= BOX_RIGHT)
                balls[i].x = BOX_RIGHT - balls[i].radius;
        }

        if (balls[i].y - balls[i].radius <= BOX_BOTTOM || balls[i].y + balls[i].radius >= BOX_TOP)
        {
            balls[i].vy = -balls[i].vy;
            // Clamp position to prevent sticking
            if (balls[i].y - balls[i].radius <= BOX_BOTTOM)
                balls[i].y = BOX_BOTTOM + balls[i].radius;
            if (balls[i].y + balls[i].radius >= BOX_TOP)
                balls[i].y = BOX_TOP - balls[i].radius;
        }
    }

    // Check ball-to-ball collisions
    for (int i = 0; i < NUM_BALLS; i++)
    {
        for (int j = i + 1; j < NUM_BALLS; j++)
        {
            float dx = balls[j].x - balls[i].x;
            float dy = balls[j].y - balls[i].y;
            float distance = sqrt(dx * dx + dy * dy);
            float minDistance = balls[i].radius + balls[j].radius;

            if (distance < minDistance)
            {
                // Collision detected - separate balls
                float overlap = minDistance - distance;
                float separationX = (dx / distance) * overlap * 0.5f;
                float separationY = (dy / distance) * overlap * 0.5f;

                balls[i].x -= separationX;
                balls[i].y -= separationY;
                balls[j].x += separationX;
                balls[j].y += separationY;

                // Calculate collision response (elastic collision for equal masses)
                float nx = dx / distance;
                float ny = dy / distance;

                // For equal masses, elastic collision simply swaps velocities along the normal
                float v1n = balls[i].vx * nx + balls[i].vy * ny;
                float v2n = balls[j].vx * nx + balls[j].vy * ny;

                // Swap normal velocities
                balls[i].vx = balls[i].vx + (v2n - v1n) * nx;
                balls[i].vy = balls[i].vy + (v2n - v1n) * ny;
                balls[j].vx = balls[j].vx + (v1n - v2n) * nx;
                balls[j].vy = balls[j].vy + (v1n - v2n) * ny;
            }
        }
    }
}

// Create a square vertex data
void createSquareVertices(float centerX, float centerY, float size, glm::vec3 color, float vertices[], int &vertexIndex)
{
    float halfSize = size * 0.5f;

    // Top-left
    vertices[vertexIndex++] = centerX - halfSize;
    vertices[vertexIndex++] = centerY + halfSize;
    vertices[vertexIndex++] = 0.0f;
    vertices[vertexIndex++] = color.r;
    vertices[vertexIndex++] = color.g;
    vertices[vertexIndex++] = color.b;

    // Top-right
    vertices[vertexIndex++] = centerX + halfSize;
    vertices[vertexIndex++] = centerY + halfSize;
    vertices[vertexIndex++] = 0.0f;
    vertices[vertexIndex++] = color.r;
    vertices[vertexIndex++] = color.g;
    vertices[vertexIndex++] = color.b;

    // Bottom-right
    vertices[vertexIndex++] = centerX + halfSize;
    vertices[vertexIndex++] = centerY - halfSize;
    vertices[vertexIndex++] = 0.0f;
    vertices[vertexIndex++] = color.r;
    vertices[vertexIndex++] = color.g;
    vertices[vertexIndex++] = color.b;

    // Top-left
    vertices[vertexIndex++] = centerX - halfSize;
    vertices[vertexIndex++] = centerY + halfSize;
    vertices[vertexIndex++] = 0.0f;
    vertices[vertexIndex++] = color.r;
    vertices[vertexIndex++] = color.g;
    vertices[vertexIndex++] = color.b;

    // Bottom-right
    vertices[vertexIndex++] = centerX + halfSize;
    vertices[vertexIndex++] = centerY - halfSize;
    vertices[vertexIndex++] = 0.0f;
    vertices[vertexIndex++] = color.r;
    vertices[vertexIndex++] = color.g;
    vertices[vertexIndex++] = color.b;

    // Bottom-left
    vertices[vertexIndex++] = centerX - halfSize;
    vertices[vertexIndex++] = centerY - halfSize;
    vertices[vertexIndex++] = 0.0f;
    vertices[vertexIndex++] = color.r;
    vertices[vertexIndex++] = color.g;
    vertices[vertexIndex++] = color.b;
}

// Square mass initialization function
void initSquareMasses(float massRatio)
{
    squares[0].mass = 1.0f;
    squares[1].mass = massRatio;
}

// Reset blue squares to initial state
void resetSquares()
{
    squares[0].x = 0.0f;
    squares[0].y = 0.0f;
    squares[0].vx = 0.004f;
    squares[0].vy = 0.0f;
    squares[0].size = 0.1f;
    squares[0].color = glm::vec3(0.0f, 0.0f, 1.0f);

    squares[1].x = 0.3f;
    squares[1].y = 0.0f;
    squares[1].vx = -0.003f;
    squares[1].vy = 0.0f;
    squares[1].size = 0.1f;
    squares[1].color = glm::vec3(0.0f, 0.0f, 1.0f);
}

// Update square physics
void updateSquare()
{
    // Update position for all squares
    for (int i = 0; i < NUM_SQUARES; i++)
    {
        // Apply small damping to prevent energy buildup
        squares[i].vx *= 1.0f; // No damping for now
        squares[i].vy *= 1.0f;

        squares[i].x += squares[i].vx;
        squares[i].y += squares[i].vy;

        // Check collision with walls
        if (squares[i].x - squares[i].size * 0.5f <= BOX_LEFT || squares[i].x + squares[i].size * 0.5f >= BOX_RIGHT)
        {
            squares[i].vx = -squares[i].vx;
            // Clamp position to prevent sticking
            if (squares[i].x - squares[i].size * 0.5f <= BOX_LEFT)
                squares[i].x = BOX_LEFT + squares[i].size * 0.5f;
            if (squares[i].x + squares[i].size * 0.5f >= BOX_RIGHT)
                squares[i].x = BOX_RIGHT - squares[i].size * 0.5f;
        }

        if (squares[i].y - squares[i].size * 0.5f <= BOX_BOTTOM || squares[i].y + squares[i].size * 0.5f >= BOX_TOP)
        {
            squares[i].vy = -squares[i].vy;
            // Clamp position to prevent sticking
            if (squares[i].y - squares[i].size * 0.5f <= BOX_BOTTOM)
                squares[i].y = BOX_BOTTOM + squares[i].size * 0.5f;
            if (squares[i].y + squares[i].size * 0.5f >= BOX_TOP)
                squares[i].y = BOX_TOP - squares[i].size * 0.5f;
        }
    }

    // Check square-to-square collisions (1D horizontal only)
    for (int i = 0; i < NUM_SQUARES; i++)
    {
        for (int j = i + 1; j < NUM_SQUARES; j++)
        {
            float dx = squares[j].x - squares[i].x;
            float distance = fabs(dx);
            float minDistance = squares[i].size * 0.5f + squares[j].size * 0.5f;

            if (distance < minDistance && distance > 0.001f)
            {
                // Only handle if moving toward each other (1D)
                if ((squares[j].vx - squares[i].vx) * (squares[j].x - squares[i].x) >= 0)
                    continue;

                // Separate squares
                float overlap = minDistance - distance;
                float separation = overlap * 0.5f * (dx > 0 ? 1.0f : -1.0f);
                squares[i].x -= separation;
                squares[j].x += separation;

                // 1D elastic collision equations for vx
                float m1 = squares[i].mass;
                float m2 = squares[j].mass;
                float v1 = squares[i].vx;
                float v2 = squares[j].vx;
                float v1p = ((m1 - m2) * v1 + 2 * m2 * v2) / (m1 + m2);
                float v2p = ((m2 - m1) * v2 + 2 * m1 * v1) / (m1 + m2);
                squares[i].vx = v1p;
                squares[j].vx = v2p;
            }
        }
    }
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

    // Box walls vertex data (4 walls)
    float boxVertices[4 * 6 * 6]; // 4 walls * 6 vertices * 6 floats per vertex
    int boxVertexIndex = 0;

    // Top wall
    createRectangle(BOX_LEFT, BOX_TOP - 0.02f, BOX_RIGHT - BOX_LEFT, 0.02f, glm::vec3(1.0f, 1.0f, 1.0f), boxVertices, boxVertexIndex);
    // Bottom wall
    createRectangle(BOX_LEFT, BOX_BOTTOM, BOX_RIGHT - BOX_LEFT, 0.02f, glm::vec3(1.0f, 1.0f, 1.0f), boxVertices, boxVertexIndex);
    // Left wall
    createRectangle(BOX_LEFT, BOX_BOTTOM, 0.02f, BOX_TOP - BOX_BOTTOM, glm::vec3(1.0f, 1.0f, 1.0f), boxVertices, boxVertexIndex);
    // Right wall
    createRectangle(BOX_RIGHT - 0.02f, BOX_BOTTOM, 0.02f, BOX_TOP - BOX_BOTTOM, glm::vec3(1.0f, 1.0f, 1.0f), boxVertices, boxVertexIndex);

    // Ball vertex data (will be updated each frame)
    float ballVertices[MAX_BALLS * 32 * 3 * 6]; // MAX_BALLS * 32 segments * 3 vertices * 6 floats per vertex
    int ballVertexIndex = 0;
    for (int i = 0; i < NUM_BALLS; i++)
    {
        createCircle(balls[i].x, balls[i].y, balls[i].radius, balls[i].color, ballVertices, ballVertexIndex);
    }

    // Square vertex data (will be updated each frame)
    float squareVertices[NUM_SQUARES * 6 * 6]; // NUM_SQUARES * 6 vertices * 6 floats per vertex
    int squareVertexIndex = 0;
    for (int i = 0; i < NUM_SQUARES; i++)
    {
        createSquareVertices(squares[i].x, squares[i].y, squares[i].size, squares[i].color, squareVertices, squareVertexIndex);
    }

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

    // VBO/VAO for box walls
    unsigned int boxVBO, boxVAO;
    glGenVertexArrays(1, &boxVAO);
    glGenBuffers(1, &boxVBO);
    glBindVertexArray(boxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // VBO/VAO for ball (dynamic)
    unsigned int ballVBO, ballVAO;
    glGenVertexArrays(1, &ballVAO);
    glGenBuffers(1, &ballVBO);
    glBindVertexArray(ballVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ballVertices), ballVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // VBO/VAO for square (dynamic)
    unsigned int squareVBO, squareVAO;
    glGenVertexArrays(1, &squareVAO);
    glGenBuffers(1, &squareVBO);
    glBindVertexArray(squareVAO);
    glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Get uniform locations
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    // Initialize balls
    initBalls(5);

    // Initialize squares with equal mass
    initSquareMasses(1.0f);
    resetSquares();

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Update physics
        if (currentScreen == Screen::RED_DEMO)
        {
            updateBall();
        }
        else if (currentScreen == Screen::BLUE_DEMO)
        {
            updateSquare();
        }

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
        else if (currentScreen == Screen::RED_DEMO)
        {
            // Red demo - bouncing balls
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(shaderProgram);
            glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glm::mat4 model = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // Draw box walls
            glBindVertexArray(boxVAO);
            glDrawArrays(GL_TRIANGLES, 0, 4 * 6); // 4 walls * 6 vertices

            // Draw ball count buttons
            float ballCountButtonVertices[NUM_BALL_COUNT_BUTTONS * 6 * 6];
            int bcVertexIndex = 0;
            for (int i = 0; i < NUM_BALL_COUNT_BUTTONS; i++)
            {
                glm::vec3 color = glm::vec3(0.3f, 0.3f, 0.3f);
                createRectangle(ballCountButtons[i].x, ballCountButtons[i].y, ballCountButtons[i].width, ballCountButtons[i].height, color, ballCountButtonVertices, bcVertexIndex);
            }
            unsigned int bcVBO, bcVAO;
            glGenVertexArrays(1, &bcVAO);
            glGenBuffers(1, &bcVBO);
            glBindVertexArray(bcVAO);
            glBindBuffer(GL_ARRAY_BUFFER, bcVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(ballCountButtonVertices), ballCountButtonVertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(bcVAO);
            glDrawArrays(GL_TRIANGLES, 0, NUM_BALL_COUNT_BUTTONS * 6);
            glDeleteVertexArrays(1, &bcVAO);
            glDeleteBuffers(1, &bcVBO);

            // Update and draw all balls
            ballVertexIndex = 0;
            for (int i = 0; i < NUM_BALLS; i++)
            {
                createCircle(balls[i].x, balls[i].y, balls[i].radius, balls[i].color, ballVertices, ballVertexIndex);
            }
            glBindVertexArray(ballVAO);
            glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ballVertices), ballVertices);
            glDrawArrays(GL_TRIANGLES, 0, NUM_BALLS * 32 * 3); // NUM_BALLS * 32 segments * 3 vertices

            // Draw back button
            glBindVertexArray(backVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        else if (currentScreen == Screen::BLUE_DEMO)
        {
            // Blue demo - bouncing squares
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(shaderProgram);
            glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glm::mat4 model = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // Draw box walls
            glBindVertexArray(boxVAO);
            glDrawArrays(GL_TRIANGLES, 0, 4 * 6); // 4 walls * 6 vertices

            // Draw mass buttons
            float massButtonVertices[NUM_MASS_BUTTONS * 6 * 6];
            int mbVertexIndex = 0;
            for (int i = 0; i < NUM_MASS_BUTTONS; i++)
            {
                glm::vec3 color = glm::vec3(0.3f, 0.3f, 0.3f);
                createRectangle(massButtons[i].x, massButtons[i].y, massButtons[i].width, massButtons[i].height, color, massButtonVertices, mbVertexIndex);
            }
            unsigned int mbVBO, mbVAO;
            glGenVertexArrays(1, &mbVAO);
            glGenBuffers(1, &mbVBO);
            glBindVertexArray(mbVAO);
            glBindBuffer(GL_ARRAY_BUFFER, mbVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(massButtonVertices), massButtonVertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(mbVAO);
            glDrawArrays(GL_TRIANGLES, 0, NUM_MASS_BUTTONS * 6);
            glDeleteVertexArrays(1, &mbVAO);
            glDeleteBuffers(1, &mbVBO);

            // Update and draw all squares
            squareVertexIndex = 0;
            for (int i = 0; i < NUM_SQUARES; i++)
            {
                createSquareVertices(squares[i].x, squares[i].y, squares[i].size, squares[i].color, squareVertices, squareVertexIndex);
            }
            glBindVertexArray(squareVAO);
            glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(squareVertices), squareVertices);
            glDrawArrays(GL_TRIANGLES, 0, NUM_SQUARES * 6); // NUM_SQUARES * 6 vertices for squares

            // Draw back button
            glBindVertexArray(backVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        else
        {
            // Other demo screens
            glm::vec3 demoColor;
            const char *demoName;
            switch (currentScreen)
            {
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
    glDeleteVertexArrays(1, &boxVAO);
    glDeleteBuffers(1, &boxVBO);
    glDeleteVertexArrays(1, &ballVAO);
    glDeleteBuffers(1, &ballVBO);
    glDeleteVertexArrays(1, &squareVAO);
    glDeleteBuffers(1, &squareVBO);
    glDeleteProgram(shaderProgram);

    // Clean up
    glfwTerminate();
    return 0;
}