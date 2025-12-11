#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#include "imgui.h"
#include "implot.h"
#include <GLFW/glfw3.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <deque>

#define GL_SILENCE_DEPRECATION
using namespace std;

#define WIDTH 1920
#define HEIGHT 1080

/* TODO: Clean this shit up*/
#define TRACING_COLOR_MAP_UPDATE 0x0000ff
#define TRACING_COLOR_RENDER 0x227733
#define NUM_ELEM(array) (sizeof(array) / sizeof((array)[0]))
static uint32_t rrggbb_to_aabbggrr(uint32_t u24_tracing_color) {
    return
        0xff << 24
        | (u24_tracing_color & 0xff0000) >> 16
        | (u24_tracing_color & 0xff00)
        | (u24_tracing_color & 0xff) << 16;
}

GLFWwindow* window;

void reset();

float lastTime = glfwGetTime();
float dt;
float currentTime;

float gravity = 0.0;
float separationForce = 2000.0;
float radius = 2.6f;

float startingX = 5.0, startingY = 710.0;
float startingVX = 0.0, startingVY = 0.0;
float VX = startingVX, VY = startingVY;
float startingColorR = 0.31;
float startingColorG = 0.62;
float startingColorB = 0.62;

bool mb1pressed;
bool mb2pressed;

double GLFWmX, GLFWmY;
double ParticlemX, ParticlemY;

float mouseFRadius = 200.0;

bool resetSimButton;

bool show_demo_window = false;
bool show_implot_demo_window = false;

int particle_ammount;

int maxFPSbufSamples = 300;

static float col1[3] = { 1.0f, 0.0f, 0.2f };

class Particle {
public:
    float x, y;
    float vx, vy;
    float r, g, b;
};

std::vector<Particle> particles;

// Print GLFW errors to console
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


void drawParticle() {
    glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (auto& p : particles) {
        float nx = p.x / WIDTH * 2 - 1;
        float ny = p.y / HEIGHT * 2 - 1;
        
        glVertex2f(nx, ny);
        glColor3f(p.r, p.g, p.b);
    }
    glEnd();

}

void updateParticle(float dt) {
    for (auto& p : particles) {
        p.vy -= gravity * dt;
        p.y += p.vy * dt;
        p.x += p.vx * dt;
    }
    
    glfwGetCursorPos(window, &GLFWmX, &GLFWmY);

    ParticlemX = GLFWmX;
    ParticlemY = HEIGHT - GLFWmY;

    if (mb1pressed == true) {
        for (auto& p : particles) {
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
        }
    }

    if (resetSimButton) {
        reset();
    }
}

void CollisionHandler(float dt) {
    for (int i = 0; i < particles.size(); i++) {
        for (int j = 0; j < i; j++) {
            auto& a = particles[i];
            auto& b = particles[j];
            float dx = a.x - b.x;
            float dy = a.y - b.y;
            auto distance = sqrt(((dx) * (dx)) + ((dy) * (dy)));
            
            if (distance < radius * 2) {
                //a.vx -= separationForce * dt;
                //a.vy -= separationForce * dt;
                //b.vx += separationForce * dt;
                //b.vy += separationForce * dt;

                a.vx += dx * separationForce * dt;
                a.vy += dy * separationForce * dt;
                b.vx -= dx * separationForce * dt;
                b.vy -= dy * separationForce * dt;
            }
        }
    }

    for (auto& a : particles) {
        // collisions with ground
        if (a.y < 2) {
            a.y = 2;
            a.vy *= -0.5;
        }
        else if (a.y > 1075) {
            a.y = 1075;
            a.vy *= -0.5;
        }

        // colliosions with walls
        if (a.x > WIDTH) {
            a.x = WIDTH;
            a.vx *= -0.6;
        }
        else if (a.x < 0) {
            a.x = 0;
            a.vx *= -0.6;
        }

    }
}

void particleCreation() {
    for (int n = 0; n < particle_ammount; n++) {
        particles.push_back(Particle{ startingX, startingY, VX, VY, startingColorR, startingColorG, startingColorB});
        //startingX += 5;
        startingX++;
        VX++;
        VY++;
    }
}

/* TODO: fix colors */
void changeColor() {
    for (auto& p : particles) {
        if (p.vx > 300.0 || p.vy > 300.0) {
            p.r = col1[0];
            p.g = col1[1];
            p.b = col1[2];
        }
        else if (p.vx > 300.0 && p.vy > 300.0) {
            p.r = col1[0];
            p.g = col1[1];
            p.b = col1[2];
        }
        else if (p.vx < 100.0 || p.vy < 100.0) {
            p.r = startingColorR;
            p.g = startingColorG;
            p.b = startingColorB;
        }
    }
}

/* void mousecallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mb1pressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mb1pressed = false;
    }
} */

void ImguiWindow(ImGuiIO& io = ImGui::GetIO()) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }
    if (show_implot_demo_window) {
        ImPlot::ShowDemoWindow();
    }

    ImGui::Begin("Particle Simulator", NULL, ImGuiWindowFlags_NoBackground);                          // Create a window called "Hello, world!" and append into it.

    // window settings and style
    ImGuiStyle& style = ImGui::GetStyle();
    
    style.WindowRounding = 5.0f;
    style.FrameRounding = 5.0f;

    // Edit bools storing our window open/close state
    //ImGui::Checkbox("Another Window", &show_another_window);
    ImGui::Spacing();
    ImGui::SliderInt("Ammount of particles", &particle_ammount, 0, 5000);
    ImGui::Spacing();
    ImGui::SliderFloat("Gravity", &gravity, 0.0f, 5000.0f);
    ImGui::Spacing();
    ImGui::SliderFloat("Separation Force", &separationForce, 0.0, 5000.0f);
    ImGui::Spacing();
    ImGui::SliderFloat("Starting Velocity X", &startingVX, -1000.0, 1000.0f);
    ImGui::Spacing();
    ImGui::SliderFloat("Starting Velocity Y", &startingVY, -1000.0, 1000.0f);
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::ColorEdit3("clear color", col1); // Edit 3 floats representing a color
    ImGui::Spacing();
    ImGui::Spacing();
    //ImGui::SliderFloat("Starting Velocity Y", &separationForce, 0.0, 5000.0f);
    /*
    if (ImGui::Begin("fps")) {
        auto size = ImVec2(ImGui::GetWindowSize().x - 20, 200 * 1.0);
        if (ImPlot::BeginPlot("Timing", size, ImPlotFlags_NoInputs)) {
            static const ImU32 color_map_data[] = {
                rrggbb_to_aabbggrr(TRACING_COLOR_MAP_UPDATE),
                rrggbb_to_aabbggrr(TRACING_COLOR_RENDER),
            };
            static ImPlotColormap timing_color_map = -1;
            if (timing_color_map == -1) {
                timing_color_map = ImPlot::AddColormap("CycleTimesUpdateTimeColorMap", color_map_data, NUM_ELEM(color_map_data));
            }

            ImPlot::PushColormap(timing_color_map);

            //ImPlot::SetupAxes("sample", "time (ms)");
            //ImPlot::PlotLine("map_update", map_update_x, map_update_y, total_samples, 0, 0);
            //ImPlot::PlotLine("framerate", fpsBuf., fpsBuf.size());

            ImPlot::PopColormap();

            ImPlot::EndPlot();
        }
        ImGui::End();
    } */
    

    if (ImGui::Button("reset")) { resetSimButton = true; };
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Checkbox("imgui demo window", &show_demo_window);
    ImGui::Spacing();
    ImGui::Checkbox("implot demo window", &show_implot_demo_window);
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::TextLinkOpenURL("Github repo", "https://github.com/anAndjel/Particle-Fluid-simulation");
    ImGui::Spacing();
    ImGui::TextLinkOpenURL("Buy me a coffee!", "https://buymeacoffee.com/alexmtavares?new=1");
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}

void reset() {
    lastTime = glfwGetTime();
    dt = 0;
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
    }

    particleCreation();
    resetSimButton = false;
}

int main() {

    // initialize glfw
    if (!glfwInit()) {
        return -1;
    }

    // create window and context
    window = glfwCreateWindow(WIDTH, HEIGHT, "particle sim", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    //glfwSwapInterval(1); // Disable vsync

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    float main_scale = 1.0;//ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImPlot::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

     // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    //style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    //style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    //glfwSetMouseButtonCallback(window, mousecallback);

    //particles.push_back(Particle{ 10, 710, 0, 0 });
    particleCreation();

    // main loop
    while (!glfwWindowShouldClose(window)) {

        currentTime = glfwGetTime();
        dt = currentTime - lastTime;
        lastTime = currentTime;

        // physics update
        updateParticle(dt);
        CollisionHandler(dt);

        // change particle color based on speed
        changeColor();
        
        // draw
        drawParticle();
        
        // poll for and process events
        glfwPollEvents();

        // imgui window
        ImguiWindow();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
