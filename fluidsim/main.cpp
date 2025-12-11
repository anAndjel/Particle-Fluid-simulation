#include "imgui.h"
#include "implot.h"
#include <GLFW/glfw3.h>
#include <Windows.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>

#define GL_SILENCE_DEPRECATION
using namespace std;

#define WIDTH 1280
#define HEIGHT 720

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

float lastTime = glfwGetTime();
float dt;
float currentTime;

float gravity = 0.0;
float radius = 3.0f;

float startingX = 5.0;
float startingY = 710.0;
float startingVX = 0.0;
float startingVY = 0.0;
float startingColorR = 0.31;
float startingColorG = 0.62;
float startingColorB = 0.62;

bool mb1pressed;
bool mb2pressed;

double GLFWmX, GLFWmY;
double ParticlemX, ParticlemY;

float mouseFRadius = 200.0;

bool resetSimButton;

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
    glPointSize(3.0f);
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
                float separationForce = 1000.0f;
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
        else if (a.y > 720) {
            a.y = 720;
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

        // collisions between particles
        //for (auto& b : particles) {
        //    auto distance = sqrt(((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y)));
        //}
    }
}

void particleCreation() {
    for (int i = 0; i < 1; i++) {
        particles.push_back(Particle{ startingX, startingY - 10, startingVX, -100, 1, 0, 0});
        //startingX += 5;
        //startingVX++;
        //startingVY++;
    }
    startingX = 5;
    startingY -= 10;
    for (int n = 0; n < 2500; n++) {
        particles.push_back(Particle{ startingX, startingY, startingVX, startingVY, startingColorR, startingColorG, startingColorB});
        //startingX += 5;
        startingX += 5;
        startingVX++;
        startingVY++;
    }
}

void changeColor() {
    for (auto& p : particles) {
        if (p.vx > 100.0 || p.vy > 100.0) {
            p.r = p.vy;
            p.g = p.vy;
            p.b = p.vy;
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

void ImguiWindow() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    //ImGui::Checkbox("Demo Window", &);      // Edit bools storing our window open/close state
    //ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

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

        ImPlot::SetupAxes("sample", "time (ms)");
        //ImPlot::PlotLine("map_update", map_update_x, map_update_y, total_samples, 0, 0);
        //ImPlot::PlotLine("render", render_x, render_y, total_samples, 0, 0);

        ImPlot::PopColormap();

        ImPlot::EndPlot();
    }


    ImGui::Button("resetSimButton");                            // Buttons return true when clicked (most widgets return true when edited/activated)
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}

void reset() {
    startingX = 5.0;
    startingY = 710.0;
    startingVX = 0.0;
    startingVY = 0.0;
    startingColorR = 0.31;
    startingColorG = 0.62;
    startingColorB = 0.62;

    for ()
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

        ImPlot::ShowDemoWindow();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // buffer swap
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}