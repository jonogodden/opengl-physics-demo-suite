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

// Reset button for Newton's Cradle
struct ResetButton
{
    float x, y, width, height;
    const char *label;
};

ResetButton resetButton = {-0.9f, 0.75f, 0.5f, 0.08f, "Reset"};

// Fluid flow parameters
struct FluidParticle
{
    float x, y;
    float vx, vy;
    float radius;
    glm::vec3 color;
    bool active;
};

// Speed control buttons for fluid demo
struct SpeedButton
{
    float x, y, width, height;
    float speed;
    const char *label;
};

SpeedButton speedButtons[] = {
    {-0.9f, 0.6f, 0.5f, 0.08f, 0.002f, "Slow"},
    {-0.2f, 0.6f, 0.5f, 0.08f, 0.005f, "Medium"},
    {0.5f, 0.6f, 0.5f, 0.08f, 0.01f, "Fast"}};
const int NUM_SPEED_BUTTONS = 3;

// Shape types for aerodynamics demo
enum class ObstacleShape
{
    BALL,
    TRIANGLE,
    AIRFOIL
};

// Shape selection buttons
struct ShapeButton
{
    float x, y, width, height;
    ObstacleShape shape;
    const char *label;
};

ShapeButton shapeButtons[] = {
    {-0.9f, 0.75f, 0.5f, 0.08f, ObstacleShape::BALL, "Ball"},
    {-0.2f, 0.75f, 0.5f, 0.08f, ObstacleShape::TRIANGLE, "Triangle"},
    {0.5f, 0.75f, 0.5f, 0.08f, ObstacleShape::AIRFOIL, "Airfoil"}};
const int NUM_SHAPE_BUTTONS = 3;

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

// Newton's Cradle structures
struct Pendulum
{
    float x, y;       // Current position
    float angle;      // Current angle (radians)
    float angularVel; // Angular velocity
    float length;     // Length of pendulum string
    float mass;       // Mass of the bob
    float radius;     // Radius of the bob
    glm::vec3 color;  // Color of the bob
    bool isDragging;  // Whether this pendulum is being dragged
};

// Newton's Cradle parameters
const int NUM_PENDULUMS = 5;
Pendulum pendulums[NUM_PENDULUMS];
const float PENDULUM_LENGTH = 0.8f;
const float PENDULUM_SPACING = 0.12f;
const float PENDULUM_RADIUS = 0.05f;
const float PENDULUM_MASS = 1.0f;
const float GRAVITY = 0.001f;
const float DAMPING = 0.999f;
const float COLLISION_DISTANCE = PENDULUM_RADIUS * 2.0f;

// Newton's Cradle initialization function prototype
void initNewtonsCradle();

// Newton's Cradle update function prototype
void updateNewtonsCradle();

// Newton's Cradle reset function prototype
void resetNewtonsCradle();

// Box boundaries
const float BOX_LEFT = -0.8f;
const float BOX_RIGHT = 0.8f;
const float BOX_TOP = 0.6f;
const float BOX_BOTTOM = -0.6f;

// Fluid demo parameters
const int MAX_FLUID_PARTICLES = 200;
FluidParticle fluidParticles[MAX_FLUID_PARTICLES];
int NUM_FLUID_PARTICLES = 0;
float streamSpeed = 0.005f;
float obstacleX = 0.0f;
float obstacleY = 0.0f;
float obstacleRadius = 0.15f;
ObstacleShape currentShape = ObstacleShape::BALL;

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
        else if (currentScreen == Screen::GREEN_DEMO)
        {
            // Check if reset button was clicked
            if (normalizedX >= resetButton.x && normalizedX <= resetButton.x + resetButton.width &&
                normalizedY >= resetButton.y && normalizedY <= resetButton.y + resetButton.height)
            {
                resetNewtonsCradle();
                return;
            }
            // Check if back button was clicked
            if (normalizedX >= backButton.x && normalizedX <= backButton.x + backButton.width &&
                normalizedY >= backButton.y && normalizedY <= backButton.y + backButton.height)
            {
                currentScreen = Screen::MAIN_MENU;
            }
            else
            {
                // Check if clicking on any pendulum to pull it and all balls to its left back
                for (int i = 0; i < NUM_PENDULUMS; i++)
                {
                    float bobX = pendulums[i].x + pendulums[i].length * sin(pendulums[i].angle);
                    float bobY = pendulums[i].y - pendulums[i].length * cos(pendulums[i].angle);

                    float clickDistance = sqrt((normalizedX - bobX) * (normalizedX - bobX) +
                                               (normalizedY - bobY) * (normalizedY - bobY));

                    if (clickDistance < pendulums[i].radius * 2.0f)
                    {
                        // Pull back this pendulum and all pendulums to its left
                        for (int j = 0; j <= i; j++)
                        {
                            pendulums[j].angle = -0.5f;     // Pull back about 30 degrees
                            pendulums[j].angularVel = 0.0f; // Reset velocity
                        }
                        break; // Exit the loop after finding the clicked ball
                    }
                }
            }
        }
        else if (currentScreen == Screen::YELLOW_DEMO)
        {
            // Check if a shape button was clicked
            for (int i = 0; i < NUM_SHAPE_BUTTONS; i++)
            {
                ShapeButton &b = shapeButtons[i];
                if (normalizedX >= b.x && normalizedX <= b.x + b.width &&
                    normalizedY >= b.y && normalizedY <= b.y + b.height)
                {
                    currentShape = b.shape;
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

// Create pendulum string vertex data
void createPendulumString(float anchorX, float anchorY, float bobX, float bobY, glm::vec3 color, float vertices[], int &vertexIndex)
{
    // Create a thin line for the string
    float thickness = 0.002f;

    // Calculate string direction
    float dx = bobX - anchorX;
    float dy = bobY - anchorY;
    float length = sqrt(dx * dx + dy * dy);

    if (length > 0.001f)
    {
        // Normalize direction
        dx /= length;
        dy /= length;

        // Perpendicular vector for thickness
        float perpX = -dy * thickness;
        float perpY = dx * thickness;

        // Create a thin rectangle for the string
        // Top-left
        vertices[vertexIndex++] = anchorX + perpX;
        vertices[vertexIndex++] = anchorY + perpY;
        vertices[vertexIndex++] = 0.0f;
        vertices[vertexIndex++] = color.r;
        vertices[vertexIndex++] = color.g;
        vertices[vertexIndex++] = color.b;

        // Top-right
        vertices[vertexIndex++] = anchorX - perpX;
        vertices[vertexIndex++] = anchorY - perpY;
        vertices[vertexIndex++] = 0.0f;
        vertices[vertexIndex++] = color.r;
        vertices[vertexIndex++] = color.g;
        vertices[vertexIndex++] = color.b;

        // Bottom-right
        vertices[vertexIndex++] = bobX - perpX;
        vertices[vertexIndex++] = bobY - perpY;
        vertices[vertexIndex++] = 0.0f;
        vertices[vertexIndex++] = color.r;
        vertices[vertexIndex++] = color.g;
        vertices[vertexIndex++] = color.b;

        // Top-left
        vertices[vertexIndex++] = anchorX + perpX;
        vertices[vertexIndex++] = anchorY + perpY;
        vertices[vertexIndex++] = 0.0f;
        vertices[vertexIndex++] = color.r;
        vertices[vertexIndex++] = color.g;
        vertices[vertexIndex++] = color.b;

        // Bottom-right
        vertices[vertexIndex++] = bobX - perpX;
        vertices[vertexIndex++] = bobY - perpY;
        vertices[vertexIndex++] = 0.0f;
        vertices[vertexIndex++] = color.r;
        vertices[vertexIndex++] = color.g;
        vertices[vertexIndex++] = color.b;

        // Bottom-left
        vertices[vertexIndex++] = bobX + perpX;
        vertices[vertexIndex++] = bobY + perpY;
        vertices[vertexIndex++] = 0.0f;
        vertices[vertexIndex++] = color.r;
        vertices[vertexIndex++] = color.g;
        vertices[vertexIndex++] = color.b;
    }
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

// Newton's Cradle initialization function
void initNewtonsCradle()
{
    float startX = -0.24f; // Center the 5 pendulums properly
    for (int i = 0; i < NUM_PENDULUMS; i++)
    {
        pendulums[i].x = startX + i * PENDULUM_SPACING;
        pendulums[i].y = BOX_TOP - 0.15f; // Anchor point inside the box, not at the very top
        pendulums[i].angle = 0.0f;
        pendulums[i].angularVel = 0.0f;
        pendulums[i].length = PENDULUM_LENGTH * 0.8f; // Adjusted scaling for longer strings
        pendulums[i].mass = PENDULUM_MASS;
        pendulums[i].radius = PENDULUM_RADIUS;
        pendulums[i].color = glm::vec3(0.0f, 1.0f, 0.0f); // Green color
        pendulums[i].isDragging = false;
    }
}

// Newton's Cradle update function
void updateNewtonsCradle()
{
    // Apply damping to all pendulums
    for (int i = 0; i < NUM_PENDULUMS; i++)
    {
        pendulums[i].angularVel *= DAMPING;
    }

    // Update pendulum physics
    for (int i = 0; i < NUM_PENDULUMS; i++)
    {
        // Simple pendulum physics
        pendulums[i].angularVel -= GRAVITY * sin(pendulums[i].angle) / pendulums[i].length;
        pendulums[i].angle += pendulums[i].angularVel;
    }

    // Check collisions between adjacent pendulums
    for (int i = 0; i < NUM_PENDULUMS - 1; i++)
    {
        float bob1X = pendulums[i].x + pendulums[i].length * sin(pendulums[i].angle);
        float bob1Y = pendulums[i].y - pendulums[i].length * cos(pendulums[i].angle);
        float bob2X = pendulums[i + 1].x + pendulums[i + 1].length * sin(pendulums[i + 1].angle);
        float bob2Y = pendulums[i + 1].y - pendulums[i + 1].length * cos(pendulums[i + 1].angle);

        float dx = bob2X - bob1X;
        float dy = bob2Y - bob1Y;
        float distance = sqrt(dx * dx + dy * dy);

        // Only handle collision if balls are overlapping and moving toward each other
        if (distance < COLLISION_DISTANCE && distance > 0.001f)
        {
            // Calculate velocities of bobs
            float vel1X = pendulums[i].angularVel * pendulums[i].length * cos(pendulums[i].angle);
            float vel1Y = -pendulums[i].angularVel * pendulums[i].length * sin(pendulums[i].angle);
            float vel2X = pendulums[i + 1].angularVel * pendulums[i + 1].length * cos(pendulums[i + 1].angle);
            float vel2Y = -pendulums[i + 1].angularVel * pendulums[i + 1].length * sin(pendulums[i + 1].angle);

            // Relative velocity along collision normal
            float normalX = dx / distance;
            float normalY = dy / distance;
            float relVelX = vel2X - vel1X;
            float relVelY = vel2Y - vel1Y;
            float relVelAlongNormal = relVelX * normalX + relVelY * normalY;

            // Only apply collision if balls are moving toward each other
            if (relVelAlongNormal < 0)
            {
                // Elastic collision - swap angular velocities
                float tempVel = pendulums[i].angularVel;
                pendulums[i].angularVel = pendulums[i + 1].angularVel;
                pendulums[i + 1].angularVel = tempVel;

                // Immediately separate balls by adjusting their positions
                // This prevents multiple collisions in the same frame
                float overlap = COLLISION_DISTANCE - distance;
                float separationX = overlap * normalX * 0.6f; // Slightly more separation
                float separationY = overlap * normalY * 0.6f;

                // Calculate new positions that maintain pendulum constraints
                float newBob1X = bob1X - separationX;
                float newBob1Y = bob1Y - separationY;
                float newBob2X = bob2X + separationX;
                float newBob2Y = bob2Y + separationY;

                // Convert back to angles while maintaining pendulum constraints
                float newAngle1 = asin((newBob1X - pendulums[i].x) / pendulums[i].length);
                float newAngle2 = asin((newBob2X - pendulums[i + 1].x) / pendulums[i + 1].length);

                // Apply the new angles
                pendulums[i].angle = newAngle1;
                pendulums[i + 1].angle = newAngle2;
            }
        }
    }
}

// Function to reset Newton's Cradle
void resetNewtonsCradle()
{
    for (int i = 0; i < NUM_PENDULUMS; i++)
    {
        pendulums[i].angle = 0.0f;
        pendulums[i].angularVel = 0.0f;
    }
}

// Fluid demo function prototypes
void initFluidDemo();
void updateFluidDemo();
void spawnFluidParticle();

// Shape collision detection function prototypes
bool checkBallCollision(float x, float y, float radius);
bool checkTriangleCollision(float x, float y, float radius);
bool checkAirfoilCollision(float x, float y, float radius);

// Fluid demo functions
void initFluidDemo()
{
    NUM_FLUID_PARTICLES = 0;
    streamSpeed = 0.01f; // Fast speed by default
    obstacleX = 0.0f;
    obstacleY = 0.0f;
    obstacleRadius = 0.15f;

    // Initialize all particles as inactive
    for (int i = 0; i < MAX_FLUID_PARTICLES; i++)
    {
        fluidParticles[i].active = false;
    }
}

void spawnFluidParticle()
{
    // Find an inactive particle
    for (int i = 0; i < MAX_FLUID_PARTICLES; i++)
    {
        if (!fluidParticles[i].active)
        {
            // Spawn on the left edge with some random vertical position
            fluidParticles[i].x = BOX_LEFT + 0.05f;
            fluidParticles[i].y = BOX_BOTTOM + 0.1f + (float)rand() / RAND_MAX * (BOX_TOP - BOX_BOTTOM - 0.2f);
            fluidParticles[i].vx = streamSpeed; // Always move right
            fluidParticles[i].vy = 0.0f;        // No vertical velocity initially
            fluidParticles[i].radius = 0.008f;
            fluidParticles[i].color = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow particles
            fluidParticles[i].active = true;
            NUM_FLUID_PARTICLES++;
            break;
        }
    }
}

void updateFluidDemo()
{
    // Continuously spawn new particles to keep the stream full
    int activeCount = 0;
    for (int i = 0; i < MAX_FLUID_PARTICLES; i++)
    {
        if (fluidParticles[i].active)
            activeCount++;
    }
    // Try to keep the stream full
    int particlesToSpawn = MAX_FLUID_PARTICLES - activeCount;
    for (int i = 0; i < particlesToSpawn; i++)
    {
        spawnFluidParticle();
    }

    // Update all active particles
    for (int i = 0; i < MAX_FLUID_PARTICLES; i++)
    {
        if (!fluidParticles[i].active)
            continue;
        FluidParticle &p = fluidParticles[i];
        // Update position
        p.x += p.vx;
        p.y += p.vy;
        // Check collision with obstacle based on current shape
        bool collision = false;
        switch (currentShape)
        {
        case ObstacleShape::BALL:
            collision = checkBallCollision(p.x, p.y, p.radius);
            break;
        case ObstacleShape::TRIANGLE:
            collision = checkTriangleCollision(p.x, p.y, p.radius);
            break;
        case ObstacleShape::AIRFOIL:
            collision = checkAirfoilCollision(p.x, p.y, p.radius);
            break;
        }
        if (collision)
        {
            float dx = p.x - obstacleX;
            float dy = p.y - obstacleY;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance > 0.001f)
            {
                float pushDistance = obstacleRadius + p.radius + 0.01f;
                p.x = obstacleX + (dx / distance) * pushDistance;
                p.y = obstacleY + (dy / distance) * pushDistance;
                float flowForce = streamSpeed * 0.5f;
                float normalX = dx / distance;
                float normalY = dy / distance;
                p.vx += normalY * flowForce;
                p.vy -= normalX * flowForce;
                if (p.vx < streamSpeed * 0.5f)
                {
                    p.vx = streamSpeed * 0.5f;
                }
            }
        }
        // Check collision with box walls - particles flow through, not bounce
        if (p.x - p.radius <= BOX_LEFT)
        {
            p.x = BOX_LEFT + p.radius;
            p.vx = streamSpeed;
        }
        if (p.y - p.radius <= BOX_BOTTOM)
        {
            p.y = BOX_BOTTOM + p.radius;
            p.vy = 0.0f;
        }
        if (p.y + p.radius >= BOX_TOP)
        {
            p.y = BOX_TOP - p.radius;
            p.vy = 0.0f;
        }
        // Remove particles that reach or pass the right edge
        if (p.x - p.radius >= BOX_RIGHT)
        {
            p.active = false;
            NUM_FLUID_PARTICLES--;
        }
        // Add small amount of damping to prevent excessive turbulence
        p.vx *= 0.998f;
        p.vy *= 0.998f;
        if (streamSpeed > 0.007f)
        {
            p.vx += ((float)rand() / RAND_MAX - 0.5f) * 0.0003f;
            p.vy += ((float)rand() / RAND_MAX - 0.5f) * 0.0003f;
        }
    }
}

// Shape collision detection functions
bool checkBallCollision(float x, float y, float radius)
{
    float dx = x - obstacleX;
    float dy = y - obstacleY;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < obstacleRadius + radius;
}

bool checkTriangleCollision(float x, float y, float radius)
{
    // Triangle vertices (equilateral triangle pointing right) - same as rendering
    float size = obstacleRadius * 2.0f; // Side length
    float h = size * sqrt(3.0f) / 2.0f; // Height
    float v1x = obstacleX - size / 2.0f, v1y = obstacleY - h / 3.0f;
    float v2x = obstacleX + size / 2.0f, v2y = obstacleY - h / 3.0f;
    float v3x = obstacleX, v3y = obstacleY + 2.0f * h / 3.0f;

    // First check if point is inside triangle (including radius)
    // Use barycentric coordinates
    float denominator = ((v2y - v3y) * (v1x - v3x) + (v3x - v2x) * (v1y - v3y));
    if (abs(denominator) < 0.0001f)
        return false; // Degenerate triangle

    float w1 = ((v2y - v3y) * (x - v3x) + (v3x - v2x) * (y - v3y)) / denominator;
    float w2 = ((v3y - v1y) * (x - v3x) + (v1x - v3x) * (y - v3y)) / denominator;
    float w3 = 1.0f - w1 - w2;

    // Check if point is inside triangle (with some tolerance for radius)
    if (w1 >= -0.1f && w2 >= -0.1f && w3 >= -0.1f)
    {
        return true;
    }

    // Also check distance to edges for particles near the boundary
    float minDist = 1000.0f;

    // Edge 1: v1 to v2
    float edge1x = v2x - v1x, edge1y = v2y - v1y;
    float edge1Len = sqrt(edge1x * edge1x + edge1y * edge1y);
    if (edge1Len > 0.0001f)
    {
        edge1x /= edge1Len;
        edge1y /= edge1Len;
        float proj1 = (x - v1x) * edge1x + (y - v1y) * edge1y;
        if (proj1 >= -radius && proj1 <= edge1Len + radius)
        {
            float dist1 = abs((x - v1x) * edge1y - (y - v1y) * edge1x);
            minDist = std::min(minDist, dist1);
        }
    }

    // Edge 2: v2 to v3
    float edge2x = v3x - v2x, edge2y = v3y - v2y;
    float edge2Len = sqrt(edge2x * edge2x + edge2y * edge2y);
    if (edge2Len > 0.0001f)
    {
        edge2x /= edge2Len;
        edge2y /= edge2Len;
        float proj2 = (x - v2x) * edge2x + (y - v2y) * edge2y;
        if (proj2 >= -radius && proj2 <= edge2Len + radius)
        {
            float dist2 = abs((x - v2x) * edge2y - (y - v2y) * edge2x);
            minDist = std::min(minDist, dist2);
        }
    }

    // Edge 3: v3 to v1
    float edge3x = v1x - v3x, edge3y = v1y - v3y;
    float edge3Len = sqrt(edge3x * edge3x + edge3y * edge3y);
    if (edge3Len > 0.0001f)
    {
        edge3x /= edge3Len;
        edge3y /= edge3Len;
        float proj3 = (x - v3x) * edge3x + (y - v3y) * edge3y;
        if (proj3 >= -radius && proj3 <= edge3Len + radius)
        {
            float dist3 = abs((x - v3x) * edge3y - (y - v3y) * edge3x);
            minDist = std::min(minDist, dist3);
        }
    }

    return minDist < radius;
}

bool checkAirfoilCollision(float x, float y, float radius)
{
    // Use the same NACA formula as the rendering
    float chord = obstacleRadius * 2.0f;
    float maxThickness = obstacleRadius * 0.8f;
    float dx = x - obstacleX;

    // Check if particle is within the airfoil's chord length
    if (dx >= -chord * 0.5f && dx <= chord * 0.5f)
    {
        // Convert to normalized coordinates (0 to 1)
        float xc = (dx + chord * 0.5f) / chord;

        // NACA 00xx thickness formula (same as rendering)
        float yt = 5.0f * maxThickness * (0.2969f * sqrt(xc) - 0.1260f * xc - 0.3516f * xc * xc + 0.2843f * xc * xc * xc - 0.1015f * xc * xc * xc * xc);

        // Check if particle is within the airfoil thickness (including radius)
        float dy = y - obstacleY;
        if (abs(dy) <= yt + radius)
        {
            return true;
        }
    }

    return false;
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

    // Create pendulum string vertex data
    float pendulumStringVertices[NUM_PENDULUMS * 6 * 6]; // NUM_PENDULUMS * 6 vertices * 6 floats per vertex
    int pendulumStringVertexIndex = 0;
    for (int i = 0; i < NUM_PENDULUMS; i++)
    {
        createPendulumString(pendulums[i].x, pendulums[i].y, pendulums[i].x + pendulums[i].length * sin(pendulums[i].angle), pendulums[i].y - pendulums[i].length * cos(pendulums[i].angle), pendulums[i].color, pendulumStringVertices, pendulumStringVertexIndex);
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

    // VBO/VAO for pendulum string
    unsigned int pendulumStringVBO, pendulumStringVAO;
    glGenVertexArrays(1, &pendulumStringVAO);
    glGenBuffers(1, &pendulumStringVBO);
    glBindVertexArray(pendulumStringVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pendulumStringVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pendulumStringVertices), pendulumStringVertices, GL_STATIC_DRAW);
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

    // Initialize Newton's Cradle
    initNewtonsCradle();

    // Initialize Fluid Demo
    initFluidDemo();

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
        else if (currentScreen == Screen::GREEN_DEMO)
        {
            updateNewtonsCradle();
        }
        else if (currentScreen == Screen::YELLOW_DEMO)
        {
            updateFluidDemo();
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
            // Declare all variables needed for GREEN_DEMO rendering here to avoid C++ jump-to-case errors
            float stringVertices[NUM_PENDULUMS * 2 * 6];
            int stringVertexIndex = 0;
            float pendulumBobVertices[NUM_PENDULUMS * 32 * 3 * 6];
            int pendulumBobVertexIndex = 0;
            unsigned int pbVBO = 0, pbVAO = 0, strVBO = 0, strVAO = 0;
            glm::mat4 projection;
            glm::mat4 model;

            switch (currentScreen)
            {
            case Screen::GREEN_DEMO:
            {
                // Newton's Cradle demo
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
                glClear(GL_COLOR_BUFFER_BIT);

                glUseProgram(shaderProgram);
                projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                model = glm::mat4(1.0f);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                // Draw box walls (reuse boxVAO)
                glBindVertexArray(boxVAO);
                glDrawArrays(GL_TRIANGLES, 0, 4 * 6); // 4 walls * 6 vertices

                // Draw pendulum strings as lines
                stringVertexIndex = 0;
                for (int i = 0; i < NUM_PENDULUMS; i++)
                {
                    float anchorX = pendulums[i].x;
                    float anchorY = pendulums[i].y;
                    float bobX = pendulums[i].x + pendulums[i].length * sin(pendulums[i].angle);
                    float bobY = pendulums[i].y - pendulums[i].length * cos(pendulums[i].angle);
                    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f); // White string
                    // Anchor point
                    stringVertices[stringVertexIndex++] = anchorX;
                    stringVertices[stringVertexIndex++] = anchorY;
                    stringVertices[stringVertexIndex++] = 0.0f;
                    stringVertices[stringVertexIndex++] = color.r;
                    stringVertices[stringVertexIndex++] = color.g;
                    stringVertices[stringVertexIndex++] = color.b;
                    // Bob point
                    stringVertices[stringVertexIndex++] = bobX;
                    stringVertices[stringVertexIndex++] = bobY;
                    stringVertices[stringVertexIndex++] = 0.0f;
                    stringVertices[stringVertexIndex++] = color.r;
                    stringVertices[stringVertexIndex++] = color.g;
                    stringVertices[stringVertexIndex++] = color.b;
                }
                glGenVertexArrays(1, &strVAO);
                glGenBuffers(1, &strVBO);
                glBindVertexArray(strVAO);
                glBindBuffer(GL_ARRAY_BUFFER, strVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(stringVertices), stringVertices, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(strVAO);
                glDrawArrays(GL_LINES, 0, NUM_PENDULUMS * 2);
                glDeleteVertexArrays(1, &strVAO);
                glDeleteBuffers(1, &strVBO);

                // Draw pendulum bobs as green balls
                pendulumBobVertexIndex = 0;
                for (int i = 0; i < NUM_PENDULUMS; i++)
                {
                    float bobX = pendulums[i].x + pendulums[i].length * sin(pendulums[i].angle);
                    float bobY = pendulums[i].y - pendulums[i].length * cos(pendulums[i].angle);
                    createCircle(bobX, bobY, pendulums[i].radius, glm::vec3(0.0f, 1.0f, 0.0f), pendulumBobVertices, pendulumBobVertexIndex);
                }
                glGenVertexArrays(1, &pbVAO);
                glGenBuffers(1, &pbVBO);
                glBindVertexArray(pbVAO);
                glBindBuffer(GL_ARRAY_BUFFER, pbVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(pendulumBobVertices), pendulumBobVertices, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(pbVAO);
                glDrawArrays(GL_TRIANGLES, 0, NUM_PENDULUMS * 32 * 3);
                glDeleteVertexArrays(1, &pbVAO);
                glDeleteBuffers(1, &pbVBO);

                // Draw back button
                glBindVertexArray(backVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);

                // Draw reset button
                float resetButtonVertices[6 * 6];
                int resetVertexIndex = 0;
                createRectangle(resetButton.x, resetButton.y, resetButton.width, resetButton.height, glm::vec3(0.3f, 0.3f, 0.3f), resetButtonVertices, resetVertexIndex);
                unsigned int resetVBO, resetVAO;
                glGenVertexArrays(1, &resetVAO);
                glGenBuffers(1, &resetVBO);
                glBindVertexArray(resetVAO);
                glBindBuffer(GL_ARRAY_BUFFER, resetVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(resetButtonVertices), resetButtonVertices, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(resetVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glDeleteVertexArrays(1, &resetVAO);
                glDeleteBuffers(1, &resetVBO);
                break;
            }
            case Screen::YELLOW_DEMO:
            {
                // Fluid Flow demo
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
                glClear(GL_COLOR_BUFFER_BIT);

                glUseProgram(shaderProgram);
                glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                glm::mat4 model = glm::mat4(1.0f);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                // Draw box walls (reuse boxVAO)
                glBindVertexArray(boxVAO);
                glDrawArrays(GL_TRIANGLES, 0, 4 * 6); // 4 walls * 6 vertices

                // Draw shape buttons
                float shapeButtonVertices[NUM_SHAPE_BUTTONS * 6 * 6];
                int shbVertexIndex = 0;
                for (int i = 0; i < NUM_SHAPE_BUTTONS; i++)
                {
                    glm::vec3 color = glm::vec3(0.4f, 0.4f, 0.4f);
                    createRectangle(shapeButtons[i].x, shapeButtons[i].y, shapeButtons[i].width, shapeButtons[i].height, color, shapeButtonVertices, shbVertexIndex);
                }
                unsigned int shbVBO, shbVAO;
                glGenVertexArrays(1, &shbVAO);
                glGenBuffers(1, &shbVBO);
                glBindVertexArray(shbVAO);
                glBindBuffer(GL_ARRAY_BUFFER, shbVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(shapeButtonVertices), shapeButtonVertices, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(shbVAO);
                glDrawArrays(GL_TRIANGLES, 0, NUM_SHAPE_BUTTONS * 6);
                glDeleteVertexArrays(1, &shbVAO);
                glDeleteBuffers(1, &shbVBO);

                // Draw obstacle based on current shape
                switch (currentShape)
                {
                case ObstacleShape::BALL:
                {
                    // Draw circle
                    float obstacleVertices[32 * 3 * 6];
                    int obstacleVertexIndex = 0;
                    createCircle(obstacleX, obstacleY, obstacleRadius, glm::vec3(0.8f, 0.8f, 0.8f), obstacleVertices, obstacleVertexIndex);
                    unsigned int obsVBO, obsVAO;
                    glGenVertexArrays(1, &obsVAO);
                    glGenBuffers(1, &obsVBO);
                    glBindVertexArray(obsVAO);
                    glBindBuffer(GL_ARRAY_BUFFER, obsVBO);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(obstacleVertices), obstacleVertices, GL_STATIC_DRAW);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
                    glEnableVertexAttribArray(1);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(obsVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 32 * 3);
                    glDeleteVertexArrays(1, &obsVAO);
                    glDeleteBuffers(1, &obsVBO);
                    break;
                }
                case ObstacleShape::TRIANGLE:
                {
                    // Draw equilateral triangle sized to fit in a circle of radius obstacleRadius
                    float size = obstacleRadius * 2.0f; // Side length
                    float h = size * sqrt(3.0f) / 2.0f; // Height
                    float v1x = obstacleX - size / 2.0f, v1y = obstacleY - h / 3.0f;
                    float v2x = obstacleX + size / 2.0f, v2y = obstacleY - h / 3.0f;
                    float v3x = obstacleX, v3y = obstacleY + 2.0f * h / 3.0f;
                    float triangleVertices[3 * 6];
                    int triVertexIndex = 0;
                    // Draw as a filled triangle
                    // Vertex 1
                    triangleVertices[triVertexIndex++] = v1x;
                    triangleVertices[triVertexIndex++] = v1y;
                    triangleVertices[triVertexIndex++] = 0.0f;
                    triangleVertices[triVertexIndex++] = 0.8f;
                    triangleVertices[triVertexIndex++] = 0.8f;
                    triangleVertices[triVertexIndex++] = 0.8f;
                    // Vertex 2
                    triangleVertices[triVertexIndex++] = v2x;
                    triangleVertices[triVertexIndex++] = v2y;
                    triangleVertices[triVertexIndex++] = 0.0f;
                    triangleVertices[triVertexIndex++] = 0.8f;
                    triangleVertices[triVertexIndex++] = 0.8f;
                    triangleVertices[triVertexIndex++] = 0.8f;
                    // Vertex 3
                    triangleVertices[triVertexIndex++] = v3x;
                    triangleVertices[triVertexIndex++] = v3y;
                    triangleVertices[triVertexIndex++] = 0.0f;
                    triangleVertices[triVertexIndex++] = 0.8f;
                    triangleVertices[triVertexIndex++] = 0.8f;
                    triangleVertices[triVertexIndex++] = 0.8f;
                    unsigned int triVBO, triVAO;
                    glGenVertexArrays(1, &triVAO);
                    glGenBuffers(1, &triVBO);
                    glBindVertexArray(triVAO);
                    glBindBuffer(GL_ARRAY_BUFFER, triVBO);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
                    glEnableVertexAttribArray(1);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(triVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 3);
                    glDeleteVertexArrays(1, &triVAO);
                    glDeleteBuffers(1, &triVBO);
                    break;
                }
                case ObstacleShape::AIRFOIL:
                {
                    // Draw a smooth, centered airfoil (NACA 00xx symmetric)
                    const int N = 40; // Number of points per surface
                    float chord = obstacleRadius * 2.0f;
                    float maxThickness = obstacleRadius * 0.8f;
                    float airfoilVertices[(N * 2) * 6];
                    int airfoilVertexIndex = 0;
                    // Generate upper surface (x from -0.5 to 0.5)
                    for (int i = 0; i < N; ++i)
                    {
                        float t = (float)i / (N - 1);
                        float x = (t - 0.5f) * chord;
                        float xc = t; // 0 to 1
                        // NACA 00xx thickness formula
                        float yt = 5.0f * maxThickness * (0.2969f * sqrt(xc) - 0.1260f * xc - 0.3516f * xc * xc + 0.2843f * xc * xc * xc - 0.1015f * xc * xc * xc * xc);
                        float vx = obstacleX + x;
                        float vy = obstacleY + yt;
                        airfoilVertices[airfoilVertexIndex++] = vx;
                        airfoilVertices[airfoilVertexIndex++] = vy;
                        airfoilVertices[airfoilVertexIndex++] = 0.0f;
                        airfoilVertices[airfoilVertexIndex++] = 0.8f;
                        airfoilVertices[airfoilVertexIndex++] = 0.8f;
                        airfoilVertices[airfoilVertexIndex++] = 0.8f;
                    }
                    // Generate lower surface (x from 0.5 to -0.5)
                    for (int i = N - 1; i >= 0; --i)
                    {
                        float t = (float)i / (N - 1);
                        float x = (t - 0.5f) * chord;
                        float xc = t;
                        float yt = 5.0f * maxThickness * (0.2969f * sqrt(xc) - 0.1260f * xc - 0.3516f * xc * xc + 0.2843f * xc * xc * xc - 0.1015f * xc * xc * xc * xc);
                        float vx = obstacleX + x;
                        float vy = obstacleY - yt;
                        airfoilVertices[airfoilVertexIndex++] = vx;
                        airfoilVertices[airfoilVertexIndex++] = vy;
                        airfoilVertices[airfoilVertexIndex++] = 0.0f;
                        airfoilVertices[airfoilVertexIndex++] = 0.8f;
                        airfoilVertices[airfoilVertexIndex++] = 0.8f;
                        airfoilVertices[airfoilVertexIndex++] = 0.8f;
                    }
                    // Draw as a triangle fan
                    unsigned int airVBO, airVAO;
                    glGenVertexArrays(1, &airVAO);
                    glGenBuffers(1, &airVBO);
                    glBindVertexArray(airVAO);
                    glBindBuffer(GL_ARRAY_BUFFER, airVBO);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(airfoilVertices), airfoilVertices, GL_STATIC_DRAW);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
                    glEnableVertexAttribArray(1);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(airVAO);
                    glDrawArrays(GL_TRIANGLE_FAN, 0, N * 2);
                    glDeleteVertexArrays(1, &airVAO);
                    glDeleteBuffers(1, &airVBO);
                    break;
                }
                }

                // Draw fluid particles
                float fluidParticleVertices[MAX_FLUID_PARTICLES * 32 * 3 * 6];
                int fluidVertexIndex = 0;
                for (int i = 0; i < MAX_FLUID_PARTICLES; i++)
                {
                    if (fluidParticles[i].active)
                    {
                        // Color particles by speed (laminar = blue, turbulent = red)
                        glm::vec3 particleColor;
                        float speed = sqrt(fluidParticles[i].vx * fluidParticles[i].vx + fluidParticles[i].vy * fluidParticles[i].vy);
                        if (speed < streamSpeed * 1.5f)
                        {
                            particleColor = glm::vec3(0.0f, 0.5f, 1.0f); // Blue for laminar
                        }
                        else
                        {
                            particleColor = glm::vec3(1.0f, 0.3f, 0.0f); // Orange/red for turbulent
                        }
                        createCircle(fluidParticles[i].x, fluidParticles[i].y, fluidParticles[i].radius, particleColor, fluidParticleVertices, fluidVertexIndex);
                    }
                }
                unsigned int fpVBO, fpVAO;
                glGenVertexArrays(1, &fpVAO);
                glGenBuffers(1, &fpVBO);
                glBindVertexArray(fpVAO);
                glBindBuffer(GL_ARRAY_BUFFER, fpVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(fluidParticleVertices), fluidParticleVertices, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(fpVAO);
                glDrawArrays(GL_TRIANGLES, 0, fluidVertexIndex / 6);
                glDeleteVertexArrays(1, &fpVAO);
                glDeleteBuffers(1, &fpVBO);

                // Draw back button
                glBindVertexArray(backVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                break;
            }
            default:
                demoColor = glm::vec3(0.5f, 0.5f, 0.5f);
                demoName = "Unknown Demo";
                break;
            }

            // Handle other demo screens (default only)
            if (currentScreen != Screen::GREEN_DEMO && currentScreen != Screen::YELLOW_DEMO)
            {
                // Set clear color to demo color
                glClearColor(demoColor.r, demoColor.g, demoColor.b, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                // Draw the back button
                glUseProgram(shaderProgram);
                projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                model = glm::mat4(1.0f);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glBindVertexArray(backVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
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
    glDeleteVertexArrays(1, &pendulumStringVAO);
    glDeleteBuffers(1, &pendulumStringVBO);
    glDeleteProgram(shaderProgram);

    // Clean up
    glfwTerminate();
    return 0;
}