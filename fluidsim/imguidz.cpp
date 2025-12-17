#include "imguidz.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "implot.h"

void SetupImGuiStyle() {
  // Comfortable Dark Cyan style by SouthCraftX from ImThemes
  ImGuiStyle &style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 1.0f;
  style.WindowPadding = ImVec2(20.0f, 20.0f);
  style.WindowRounding = 11.5f;
  style.WindowBorderSize = 0.0f;
  style.WindowMinSize = ImVec2(20.0f, 20.0f);
  style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_None;
  style.ChildRounding = 20.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 17.4f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(20.0f, 3.4f);
  style.FrameRounding = 11.9f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(8.9f, 13.4f);
  style.ItemInnerSpacing = ImVec2(7.1f, 1.8f);
  style.CellPadding = ImVec2(12.1f, 9.2f);
  style.IndentSpacing = 0.0f;
  style.ColumnsMinSpacing = 8.7f;
  style.ScrollbarSize = 11.6f;
  style.ScrollbarRounding = 15.9f;
  style.GrabMinSize = 3.7f;
  style.GrabRounding = 20.0f;
  style.TabRounding = 9.8f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] =
      ImVec4(0.27450982f, 0.31764707f, 0.4509804f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] =
      ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
  style.Colors[ImGuiCol_ChildBg] =
      ImVec4(0.09411765f, 0.101960786f, 0.11764706f, 1.0f);
  style.Colors[ImGuiCol_PopupBg] =
      ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
  style.Colors[ImGuiCol_Border] =
      ImVec4(0.15686275f, 0.16862746f, 0.19215687f, 1.0f);
  style.Colors[ImGuiCol_BorderShadow] =
      ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
  style.Colors[ImGuiCol_FrameBg] =
      ImVec4(0.11372549f, 0.1254902f, 0.15294118f, 1.0f);
  style.Colors[ImGuiCol_FrameBgHovered] =
      ImVec4(0.15686275f, 0.16862746f, 0.19215687f, 1.0f);
  style.Colors[ImGuiCol_FrameBgActive] =
      ImVec4(0.15686275f, 0.16862746f, 0.19215687f, 1.0f);
  style.Colors[ImGuiCol_TitleBg] =
      ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
  style.Colors[ImGuiCol_TitleBgActive] =
      ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] =
      ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
  style.Colors[ImGuiCol_MenuBarBg] =
      ImVec4(0.09803922f, 0.105882354f, 0.12156863f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarBg] =
      ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrab] =
      ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] =
      ImVec4(0.15686275f, 0.16862746f, 0.19215687f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] =
      ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] =
      ImVec4(0.03137255f, 0.9490196f, 0.84313726f, 1.0f);
  style.Colors[ImGuiCol_SliderGrab] =
      ImVec4(0.03137255f, 0.9490196f, 0.84313726f, 1.0f);
  style.Colors[ImGuiCol_SliderGrabActive] =
      ImVec4(0.6f, 0.9647059f, 0.03137255f, 1.0f);
  style.Colors[ImGuiCol_Button] =
      ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
  style.Colors[ImGuiCol_ButtonHovered] =
      ImVec4(0.18039216f, 0.1882353f, 0.19607843f, 1.0f);
  style.Colors[ImGuiCol_ButtonActive] =
      ImVec4(0.15294118f, 0.15294118f, 0.15294118f, 1.0f);
  style.Colors[ImGuiCol_Header] =
      ImVec4(0.14117648f, 0.16470589f, 0.20784314f, 1.0f);
  style.Colors[ImGuiCol_HeaderHovered] =
      ImVec4(0.105882354f, 0.105882354f, 0.105882354f, 1.0f);
  style.Colors[ImGuiCol_HeaderActive] =
      ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
  style.Colors[ImGuiCol_Separator] =
      ImVec4(0.12941177f, 0.14901961f, 0.19215687f, 1.0f);
  style.Colors[ImGuiCol_SeparatorHovered] =
      ImVec4(0.15686275f, 0.18431373f, 0.2509804f, 1.0f);
  style.Colors[ImGuiCol_SeparatorActive] =
      ImVec4(0.15686275f, 0.18431373f, 0.2509804f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] =
      ImVec4(0.14509805f, 0.14509805f, 0.14509805f, 1.0f);
  style.Colors[ImGuiCol_ResizeGripHovered] =
      ImVec4(0.03137255f, 0.9490196f, 0.84313726f, 1.0f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_Tab] =
      ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
  style.Colors[ImGuiCol_TabHovered] =
      ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
  style.Colors[ImGuiCol_TabActive] =
      ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocused] =
      ImVec4(0.078431375f, 0.08627451f, 0.101960786f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocusedActive] =
      ImVec4(0.1254902f, 0.27450982f, 0.57254905f, 1.0f);
  style.Colors[ImGuiCol_PlotLines] =
      ImVec4(0.52156866f, 0.6f, 0.7019608f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] =
      ImVec4(0.039215688f, 0.98039216f, 0.98039216f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] =
      ImVec4(0.03137255f, 0.9490196f, 0.84313726f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] =
      ImVec4(0.15686275f, 0.18431373f, 0.2509804f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] =
      ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] =
      ImVec4(0.047058824f, 0.05490196f, 0.07058824f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] =
      ImVec4(0.11764706f, 0.13333334f, 0.14901961f, 1.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] =
      ImVec4(0.09803922f, 0.105882354f, 0.12156863f, 1.0f);
  style.Colors[ImGuiCol_TextSelectedBg] =
      ImVec4(0.9372549f, 0.9372549f, 0.9372549f, 1.0f);
  style.Colors[ImGuiCol_DragDropTarget] =
      ImVec4(0.49803922f, 0.5137255f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_NavHighlight] =
      ImVec4(0.26666668f, 0.2901961f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] =
      ImVec4(0.49803922f, 0.5137255f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingDimBg] =
      ImVec4(0.19607843f, 0.1764706f, 0.54509807f, 0.5019608f);
  style.Colors[ImGuiCol_ModalWindowDimBg] =
      ImVec4(0.19607843f, 0.1764706f, 0.54509807f, 0.5019608f);
}

// void ImGuiUI(ImGuiIO &io = ImGui::GetIO()) {
//
//   ImGui_ImplOpenGL3_NewFrame();
//   ImGui_ImplGlfw_NewFrame();
//   ImGui::NewFrame();
//
//   if (show_demo_window) {
//     ImGui::ShowDemoWindow(&show_demo_window);
//   }
//   if (show_implot_demo_window) {
//     ImPlot::ShowDemoWindow();
//   }
//
//   ImGui::Begin("Particle Simulator", NULL,
//                ImGuiWindowFlags_NoBackground); // Create a window called
//                "Hello,
//   // world!" and append into it.
//
//   // window settings and style
//   ImGuiStyle &style = ImGui::GetStyle();
//
//   style.WindowRounding = 5.0f;
//   style.FrameRounding = 5.0f;
//
//   // Edit bools storing our window open/close state
//   // ImGui::Checkbox("Another Window", &show_another_window);
//   ImGui::Spacing();
//   ImGui::SliderInt("Particle Ammount", &particle_ammount, 0, 100000, "%d",
//                    ImGuiSliderFlags_Logarithmic);
//   // ImGui::SliderInt("Ammount of particles", &particle_ammount, 0, 10000);
//   ImGui::Spacing();
//   ImGui::SliderFloat("Gravity", &gravity, 0.0f, 5000.0f);
//   ImGui::Spacing();
//   ImGui::SliderFloat("Separation Force", &separationForce, 0.0, 5000.0f);
//   ImGui::Spacing();
//   ImGui::SliderFloat("Dampening Factor", &AirResistance, 0.0, 1.0f);
//   ImGui::Spacing();
//   ImGui::SliderFloat("Starting Velocity X", &startingVX, -5000.0, 5000.0f);
//   ImGui::Spacing();
//   ImGui::SliderFloat("Starting Velocity Y", &startingVY, -5000.0, 5000.0f);
//   ImGui::Spacing();
//   ImGui::Spacing();
//   ImGui::ColorEdit3("clear color",
//                     col1); // Edit 3 floats representing a color
//   ImGui::Spacing();
//   ImGui::Spacing();
//   // ImGui::SliderFloat("Starting Velocity Y", &separationForce, 0.0,
//   // 5000.0f);
//   /*
//   if (ImGui::Begin("fps")) {
//       auto size = ImVec2(ImGui::GetWindowSize().x - 20, 200 * 1.0);
//       if (ImPlot::BeginPlot("Timing", size, ImPlotFlags_NoInputs)) {
//           static const ImU32 color_map_data[] = {
//               rrggbb_to_aabbggrr(TRACING_COLOR_MAP_UPDATE),
//               rrggbb_to_aabbggrr(TRACING_COLOR_RENDER),
//           };
//           static ImPlotColormap timing_color_map = -1;
//           if (timing_color_map == -1) {
//               timing_color_map =
//   ImPlot::AddColormap("CycleTimesUpdateTimeColorMap", color_map_data,
//   NUM_ELEM(color_map_data));
//           }
//
//           ImPlot::PushColormap(timing_color_map);
//
//           //ImPlot::SetupAxes("sample", "time (ms)");
//           //ImPlot::PlotLine("map_update", map_update_x, map_update_y,
//   total_samples, 0, 0);
//           //ImPlot::PlotLine("framerate", fpsBuf., fpsBuf.size());
//
//           ImPlot::PopColormap();
//
//           ImPlot::EndPlot();
//       }
//       ImGui::End();
//   } */
//
//   if (ImGui::Button("reset")) {
//     resetSimButton = true;
//   };
//   ImGui::Spacing();
//   ImGui::Spacing();
//   ImGui::Checkbox("imgui demo window", &show_demo_window);
//   ImGui::Spacing();
//   ImGui::Checkbox("implot demo window", &show_implot_demo_window);
//   ImGui::Spacing();
//   ImGui::Spacing();
//   ImGui::Spacing();
//   ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
//               1000.0f / io.Framerate, io.Framerate);
//   ImGui::End();
// }
