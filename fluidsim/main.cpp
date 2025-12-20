#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#include <glad.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include "imguidz.h"
#include "implot.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;

#define WIDTH 1920
#define HEIGHT 1080

using u32 = uint32_t;

constexpr float test = 100.0f;

inline ImVec2 worldToNDC(float x, float y) {
  return ImVec2((x / WIDTH) * 2.0f - 1.0f, (y / HEIGHT) * 2.0f - 1.0f);
}

inline ImVec2 NDCToScreen(ImVec2 ndc) {
  return ImVec2((ndc.x + 1.0f) * 0.5f * WIDTH, (1.0f - ndc.y) * 0.5f * HEIGHT);
}

GLFWwindow *window;

void reset();

float lastTime = glfwGetTime();
float currentTime;
float timeScale = 1.0f;
bool paused = false;

float gravity = 0.0;
constexpr float GravitationalConstant = 0.000000000000000001f;
float separationForce = 2000.0;
constexpr float radius = 2.0f;
// constexpr float ParticleMass = 1e5;
constexpr float ParticleMass = 0.00000000000000001f;
constexpr float ParticleMass2 = 95e20;
// constexpr float ParticleMass2 = 1e5;

float startingX = 5.0, startingY = 710.0;
float startingVX = 0.0, startingVY = 0.0;
float VX = startingVX, VY = startingVY;
float startingColorR = 0.31;
float startingColorG = 0.62;
float startingColorB = 0.62;

float AirResistance;

bool drawForces = false;
float forceDrawScale = 0.001f;

// GLFWmousebuttonfun old_callback = nullptr;
// GLFWmousebuttonfun new_callback = nullptr;

bool mb1pressed;
bool mb2pressed;

ImFont *TitleFont;
ImFont *mainFont;
ImFont *SubtitleFont;

// bool getmouse = false;

double GLFWmX, GLFWmY;
double ParticlemX, ParticlemY;

float mouseFRadius = 200.0;

bool resetSimButton;

bool show_demo_window = false;
bool show_implot_demo_window = false;

float particleSize = 3.0f;

int particle_ammount;

static float col1[3] = {1.0f, 0.0f, 0.2f};

constexpr float cellSize = radius * radius;
constexpr int cellsX = (WIDTH / cellSize);
constexpr int cellsY = (HEIGHT / cellSize);

bool SpatialGravityEnabled = false;

// OpenGL variables
GLuint particleVAO;
GLuint particleVBO;
GLuint particleShaderProgram;

class Particle {
public:
  // position in world space (currently pixel space)
  float x, y;   // 8bytes
  float vx, vy; // 8bytes
  float mass;
  // x and y position in openGL normalized screen space [-1, 1]
  float nx, ny; // 8bytes
  // 32bytes
};
class ParticleColor {
public:
  float r, g, b; // 12bytes
  float a;       // ignored for now (padding)
};

std::vector<Particle> particles;
std::vector<ParticleColor> particleColors;
std::vector<int> grid[cellsX][cellsY];

void applyGravity(Particle &a, Particle &b, float GravitationalConstant,
                  float dt);

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void setupDrawing() {
  const char *vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        uniform float particleSize;
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            gl_PointSize = particleSize;
        }
    )";

  const char *fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec3 particleColor;
        void main() {
            FragColor = vec4(particleColor, 1.0);
        }
    )";

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "I'm sowwy mister there was an ewwow :3\n"
              << infoLog << std::endl;
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "EROR!! PANIK!!\n" << infoLog << std::endl;
  }

  particleShaderProgram = glCreateProgram();
  glAttachShader(particleShaderProgram, vertexShader);
  glAttachShader(particleShaderProgram, fragmentShader);
  glLinkProgram(particleShaderProgram);

  glGetProgramiv(particleShaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(particleShaderProgram, 512, NULL, infoLog);
    std::cout << "I'm sowwy mister there was an ewwow :3\n"
              << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glGenVertexArrays(1, &particleVAO);
  glGenBuffers(1, &particleVBO);

  glBindVertexArray(particleVAO);
  glBindBuffer(GL_ARRAY_BUFFER, particleVBO);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void drawParticle() {
  glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(particleShaderProgram);

  int sizeLoc = glGetUniformLocation(particleShaderProgram, "particleSize");
  glUniform1f(sizeLoc, particleSize);

  GLint colorLoc = glGetUniformLocation(particleShaderProgram, "particleColor");
  glUniform3f(colorLoc, col1[0], col1[1], col1[2]);

  std::vector<float> particlePositions;
  particlePositions.reserve(particles.size() * 2);
  for (const auto &p : particles) {
    particlePositions.push_back(p.nx);
    particlePositions.push_back(p.ny);
  }

  glBindVertexArray(particleVAO);
  glBindBuffer(GL_ARRAY_BUFFER, particleVBO);

  glBufferData(GL_ARRAY_BUFFER, particlePositions.size() * sizeof(float),
               particlePositions.data(), GL_DYNAMIC_DRAW);

  glDrawArrays(GL_POINTS, 0, particles.size());

  glBindVertexArray(0);
}

void updateParticle(float GravitationalConstant, float dt) {

  if (SpatialGravityEnabled == true) {
    for (int d = 0; d < particles.size(); d++) {
      for (int k = 0; k < particles.size(); k++) {
        applyGravity(particles[d], particles[k], GravitationalConstant, dt);
      }
    }
  }

  glfwGetCursorPos(window, &GLFWmX, &GLFWmY);

  ParticlemX = GLFWmX;
  ParticlemY = HEIGHT - GLFWmY;

  for (u32 i = 0; i < particles.size(); i++) {
    auto &p = particles[i];
    p.vy -= gravity * dt;
    p.y += p.vy * dt;
    p.x += p.vx * dt;
    p.nx = p.x / WIDTH * 2 - 1;
    p.ny = p.y / HEIGHT * 2 - 1;

    /*
    if (mb1pressed == true) {
        float dx = p.x - ParticlemX;
        float dy = p.y - ParticlemY;
        auto distance = sqrt(((dx) * (dx)) + ((dy) * (dy)));

        if (distance < mouseFRadius) {
            float mouseCForce = 5000.0;

            p.vx += mouseCForce * dt / dx;
            p.vy += mouseCForce * dt / dy;

            p.r = 0;
            p.g = 1;
            p.b = 0.31;
        }
    } */
  }

  if (resetSimButton) {
    reset();
  }
}

void applyGravity(Particle &a, Particle &b, float GravitationalConstant,
                  float dt) {
  // std::cout << "UHAEOUIHSF" << std::endl;
  float dx = a.x - b.x;
  float dy = a.y - b.y;

  float distSq = dx * dx + dy * dy + 0.01f; // 0.01f for a little softening
  float dist = std::sqrt(distSq);

  // normalize direction
  float nx = dx / dist;
  float ny = dy / dist;

  float force = GravitationalConstant * a.mass * b.mass / distSq;

  // accelertion
  float ax = force / a.mass;
  float ay = force / a.mass;

  float bx = force / b.mass;
  float by = force / b.mass;

  a.vx += nx * ax * dt;
  a.vy += ny * ay * dt;

  b.vx += nx * bx * dt;
  b.vy += ny * by * dt;
}

void clearGrid() {
  for (int x = 0; x < cellsX; x++) {
    for (int y = 0; y < cellsY; y++) {
      grid[x][y].clear();
    }
  }
}

void SpatialGrid() {
  for (u32 i = 0; i < particles.size(); i++) {
    auto &p = particles[i];
    int cellX = std::floor(p.x / cellSize);
    int cellY = std::floor(p.y / cellSize);

    if (cellX < 0) {
      cellX = 0;
    }
    if (cellX >= cellsX) {
      cellX = cellsX - 1;
    }
    if (cellY < 0) {
      cellY = 0;
    }
    if (cellY >= cellsY) {
      cellY = cellsY - 1;
    }

    grid[cellX][cellY].push_back(i);
  }
}

void CollisionHandler(float dt) {
  for (u32 i = 0; i < particles.size(); i++) {
    auto &a = particles[i];
    int cellX = std::floor(a.x / cellSize);
    int cellY = std::floor(a.y / cellSize);

    for (int ddx = -1; ddx < 1; ddx++) {
      for (int ddy = -1; ddy < 1; ddy++) {
        int Nx = cellX + ddx;
        int Ny = cellY + ddy;

        if (Nx < 0 || Nx >= cellsX) {
          continue;
        } else if (Ny < 0 || Ny >= cellsY) {
          continue;
        }
        for (int idx : grid[Nx][Ny]) {
          auto &b = particles[idx];
          float dx = a.x - b.x;
          float dy = a.y - b.y;
          auto distance =
              std::sqrt(((dx) * (dx)) +
                        ((dy) * (dy))); // at 60fps 750M distance checks / sec

          if (distance < cellSize) {
            // a.vx -= separationForce * dt;
            // a.vy -= separationForce * dt;
            // b.vx += separationForce * dt;
            // b.vy += separationForce * dt;

            a.vx += dx * separationForce * dt;
            a.vx *= AirResistance;
            a.vy += dy * separationForce * dt;
            a.vy *= AirResistance;
            b.vx -= dx * separationForce * dt;
            b.vx *= AirResistance;
            b.vy -= dy * separationForce * dt;
            b.vy *= AirResistance;
            // *= -dampeningFactor
          }
        }
      }
    }
    // bounce code here
    if (a.y < 3) {
      a.y = 3;
      a.vy *= -0.6;
    }
    if (a.y > 1080) {
      a.y = 1080;
      a.vy *= -0.6;
    }

    if (a.x > WIDTH) {
      a.x = WIDTH;
      a.vx *= -0.6;
    }
    if (a.x < 0) {
      a.x = 0;
      a.vx *= -0.6;
    }
    if (a.y <= 3 && a.vy <= 0) {
      a.vx *= -0.6;
    }
  }
}
void SpatialGRAVparticleCreation() {
  for (int n = 0; n < 1; n++) {
    particleColors.push_back(
        ParticleColor{startingColorR, startingColorG, startingColorB, 1.0});
    particles.push_back(
        Particle{startingX + 100, startingY - 100, 0.05, 0.005, ParticleMass});
    // startingX += 5;
  }
  for (int u = 0; u < 1; u++) {
    particleColors.push_back(
        ParticleColor{startingColorR, startingColorG, startingColorB, 1.0});
    particles.push_back(
        Particle{startingX + 1100, startingY - 100, -0.0, -0.0, ParticleMass2});
    // startingX += 5;
  }
}
void particleCreation() {
  for (int n = 0; n < particle_ammount; n++) {
    particleColors.push_back(
        ParticleColor{startingColorR, startingColorG, startingColorB, 1.0});
    particles.push_back(
        Particle{startingX, startingY, VX, VY, ParticleMass, 0, 0});
    // startingX += 5;
    startingX++;
    startingY++;
  }
}

/* TODO: fix colors */

void changeColor() {
  for (u32 i = 0; i < particles.size(); i++) {
    auto &p = particles[i];
    auto &pc = particleColors[i];

    if (p.vx > 300.0 || p.vy > 300.0) {
      pc.r = col1[0];
      pc.g = col1[1];
      pc.b = col1[2];
    }
    if (p.vx > 300.0 && p.vy > 300.0) {
      pc.r = col1[0];
      pc.g = col1[1];
      pc.b = col1[2];
    }
    if (p.vx < 300.0 || p.vy < 300.0) {
      col1[0] = startingColorR;
      col1[1] = startingColorG;
      col1[2] = startingColorB;
    }
  }
}

void ImguiWindow(ImGuiIO &io = ImGui::GetIO()) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (show_demo_window) {
    ImGui::ShowDemoWindow(&show_demo_window);
  }
  if (show_implot_demo_window) {
    ImPlot::ShowDemoWindow();
  }

  ImGui::SetNextWindowSize(ImVec2(420, 550), ImGuiCond_Always);
  // ImGui::SetNextWindowPos(ImVec2(1490, 520), ImGuiCond_Always);
  ImGui::Begin("##particlesim", NULL,
               ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoResize); // Create a window called

  SetupImGuiStyle();
  ImGui::PushFont(TitleFont);
  ImGui::Text("Particle Simulator");
  ImGui::PopFont();
  ImGui::PushFont(SubtitleFont);
  ImGui::SeparatorText("Simulation");
  ImGui::PopFont();
  ImGui::PushFont(mainFont);
  ImGui::SliderInt("Particle Ammount", &particle_ammount, 0, 50000, "%d",
                   ImGuiSliderFlags_Logarithmic);
  // ImGui::SliderInt("Ammount of particles", &particle_ammount, 0, 10000);
  ImGui::SliderFloat("Gravity", &gravity, 0.0f, 5000.0f);
  ImGui::SliderFloat("Simulation Speed", &timeScale, 0.25, 1000.0f);
  ImGui::SliderFloat("Particle Size", &particleSize, 0.0f, 10.0f);
  ImGui::SliderFloat("Separation Force", &separationForce, 0.0, 5000.0f);
  ImGui::PopFont();
  ImGui::PushFont(SubtitleFont);
  ImGui::SeparatorText("Forces");
  ImGui::PopFont();
  ImGui::PushFont(mainFont);
  ImGui::SliderFloat("Air Resistance", &AirResistance, 1.0, 5.0f);
  ImGui::SliderFloat("Starting Velocity X", &startingVX, -5000.0, 5000.0f);
  ImGui::SliderFloat("Starting Velocity Y", &startingVY, -5000.0, 5000.0f);
  ImGui::PopFont();
  ImGui::PushFont(SubtitleFont);
  ImGui::SeparatorText("Render");
  ImGui::PopFont();
  ImGui::PushFont(mainFont);
  ImGui::ColorEdit3("Particle Color", col1); // Edit 3 floats representing a
  ImGui::PopFont();
  ImGui::PushFont(SubtitleFont);
  ImGui::SeparatorText("Debug");
  ImGui::PopFont();
  ImGui::PushFont(mainFont);
  ImGui::Checkbox("Draw Force Vectors", &drawForces);
  ImGui::SliderFloat("Force Vector Scale", &forceDrawScale, 0.0001f, 1.0f,
                     "%.5f");

  if (ImGui::Button("reset")) {
    resetSimButton = true;
  };

  ImGui::Checkbox("Spatial Gravity", &SpatialGravityEnabled);
  ImGui::SameLine();
  ImGui::Checkbox("Pause", &paused);
  ImGui::SameLine();
  ImGui::Checkbox("imgui demo window", &show_demo_window);
  ImGui::SameLine();
  ImGui::Checkbox("implot demo window", &show_implot_demo_window);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / io.Framerate, io.Framerate);
  ImGui::PopFont();
  ImGui::End();
}
/*
void mousecallback(GLFWwindow* window, int button, int action, int mods) {
    if (getmouse) return;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            mb1pressed = true;
            std::cout << "yes" << '\n';
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            mb1pressed = false;
    }

}*/

void SpatialGravRESET() {
  lastTime = glfwGetTime();
  currentTime = 0;
  startingX = 5.0;
  startingY = 710.0;
  VX = startingVX;
  VY = startingVY;
  startingColorR = 0.31;
  startingColorG = 0.62;
  startingColorB = 0.62;

  while (!particles.empty()) {
    particles.pop_back();
    particleColors.pop_back();
  }

  SpatialGRAVparticleCreation();
  resetSimButton = false;
}

void reset() {
  if (SpatialGravityEnabled == true) {
    SpatialGravRESET();
    return;
  }

  lastTime = glfwGetTime();
  currentTime = 0;
  startingX = 5.0;
  startingY = 710.0;
  VX = startingVX;
  VY = startingVY;
  startingColorR = 0.31;
  startingColorG = 0.62;
  startingColorB = 0.62;

  while (!particles.empty()) {
    particles.pop_back();
    particleColors.pop_back();
  }

  particleCreation();
  resetSimButton = false;
}

void DrawForces(const std::vector<Particle> &particles) {
  ImDrawList *dl = ImGui::GetBackgroundDrawList();

  for (int i = 0; i < particles.size(); i++) {
    const Particle &a = particles[i];

    for (int j = i + 1; j < particles.size(); j++) {
      const Particle &b = particles[j];

      float dx = b.x - a.x;
      float dy = b.y - a.y;

      float distSq = dx * dx + dy * dy + 0.0001f;
      float dist = std::sqrtf(distSq);

      float nx = dx / dist;
      float ny = dy / dist;

      float force = (a.mass * b.mass) / distSq;

      float fx = nx * force * forceDrawScale;
      float fy = ny * force * forceDrawScale;

      ImVec2 start = NDCToScreen(worldToNDC(a.x, a.y));
      ImVec2 end = NDCToScreen(worldToNDC(a.x + fx, a.y + fy));

      dl->AddLine(start, end, IM_COL32(255, 100, 100, 100), 1.0f);
    }
  }
}

void DrawAxes() {
  ImDrawList *dl = ImGui::GetBackgroundDrawList();

  ImVec2 origin = NDCToScreen(worldToNDC(0, 0));
  ImVec2 xAxis = NDCToScreen(worldToNDC(200, 0));
  ImVec2 yAxis = NDCToScreen(worldToNDC(0, 200));

  dl->AddLine(origin, xAxis, IM_COL32(255, 0, 0, 255), 2.0f); // X
  dl->AddLine(origin, yAxis, IM_COL32(0, 255, 0, 255), 2.0f); // Y
}

/*
void combinedMouseCallback(GLFWwindow* window, int button, int action, int mods)
{ if (old_callback) { mousecallback(window, button, action, mods);
    }
    if (new_callback) {
        IMGUIcallback(window, button, action, mods);
    }
}*/
/*
template <std::size_t X, std::size_t Y>
void PrintVectorGridContents(const std::vector<int> (&vgrid)[X][Y])
{
    int t = 0;
    for (std::size_t i = 0; i < X; ++i)
    {
        for (std::size_t j = 0; j < Y; ++j)
        {
            std::cout << "[" << i << "," << j << "] {";
            const auto& cell = vgrid[i][j];
            for (std::size_t k = 0; k < cell.size(); ++k)
            {
                if (k != 0) std::cout << ", ";
                std::cout << cell[k];
            }
            std::cout << "}\n";
            t++;
            if (t > 100) {
                return;
            }
        }
    }
}
*/

int main() {
  /*
  std::cout << "CellsX: " << cellsX << '\n';
  std::cout << "CellsY: " << cellsY << '\n';
  std::cout << "Expected Grid: " << cellsX * cellsY * sizeof(int) << '\n';
  std::cout << "| grid | : " << sizeof(grid) << '\n';
  size_t grid_cell_size = (sizeof(grid) / cellsX) / cellsY;
  std::cout << "grid cell size: " << grid_cell_size;
  std::cout << '\n';
  PrintVectorGridContents(grid);
  */
  // initialize glfw
  if (!glfwInit()) {
    return -1;
  }
  glfwSetErrorCallback(glfw_error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create window and context
  window = glfwCreateWindow(WIDTH, HEIGHT, "particle sim", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "I'm sowwy mister there was an ewwow and idk why dont fwoget "
                 "to buy me a coffi tho :3"
              << '\n';
    return -1;
  }

  // Initialize OpenGL for particle drawing
  setupDrawing();

  //    glfwSetMouseButtonCallback(window, mousecallback);

  // glfwSwapInterval(1); // Disable vsync

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100 (WebGL 1.0)
  const char *glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
  // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
  const char *glsl_version = "#version 300 es";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char *glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  ImPlot::CreateContext();

  // clang-format off
  io.Fonts->AddFontDefault();
  mainFont = io.Fonts->AddFontFromFileTTF("../assets/Pixel_Letters.ttf", 15.0f);
  TitleFont = io.Fonts->AddFontFromFileTTF("../assets/Pixel_Letters.ttf", 25.0f);
  SubtitleFont = io.Fonts->AddFontFromFileTTF("../assets/Pixel_Letters.ttf", 20.0f);
  // clang-format on
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // ImGui::StyleColorsLight();
  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
  ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
  ImGui_ImplOpenGL3_Init(glsl_version);
  /*
  IMGUIcallback = glfwSetMouseButtonCallback(window, NULL);
  glfwSetMouseButtonCallback(window, mousecallback);
  */
  ImGui_ImplOpenGL3_CreateFontsTexture();
  // Initial particle creation
  particleCreation();

  // main loop
  while (!glfwWindowShouldClose(window)) {

    currentTime = glfwGetTime();
    float dt = currentTime - lastTime;
    lastTime = currentTime;
    if (paused == true) {
      dt = 0.0f;
    } else {
      dt *= timeScale;
    }
    //
    // if (SpatialGravityEnabled) {
    //   for (int i = 0; i < particles.size(); i++) {
    //     auto &a = particles[i];
    //     for (int j = 0; j < particles.size(); j++) {
    //       auto &b = particles[j];
    //       applyGravity(a, b, G, dt);
    //     }
    //   }
    // }

    // physics update
    updateParticle(GravitationalConstant, dt);
    clearGrid();
    SpatialGrid();
    CollisionHandler(dt);

    // change particle color based on speed
    changeColor();

    // draw
    drawParticle();
    // imgui window
    ImguiWindow();
    if (drawForces == true) {
      DrawForces(particles);
    }
    DrawAxes();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwPollEvents();

    // buffer swap
    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
