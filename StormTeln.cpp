#define PROJECTNAME "StormTeln"
#define PROJECTVER PROJECTNAME ## _versions
#define USEMSV_MSVCORE

#define USEMSV_ILIST
#define USEMSV_MLIST
#define USEMSV_MWND
//#define USEMSV_MWND_OPENGL

#define USEMSV_XDATACONT
#define USEMSV_HTTP
#define USEMSV_CONSOLE

#define USEMSV_IMGUI

#include "../msvcore2/msvcore.cpp"

Versions PROJECTVER[] = {
    // new version to up
    "0.0.0.2", "15.02.2025 14:09",
    "0.0.0.1", "14.02.2025 11:40" // (Moscow time)
};

char _msv_zero_code = '\0';
VString _msv_zero_str(&_msv_zero_code, 1);

// GL3W
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3dll.lib")

// Imgui
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// OpenGL
#include "../msvcore2/opengl/mgl.h"

// Telnet
#include "StormTeln.h"

// Windows
#include "Windows.h"

int main(int args, char *arg[], char *env[]) {
    // Init
    print(PROJECTNAME, " v.", PROJECTVER[0].ver, " (", PROJECTVER[0].date, ").\r\n");
    //msvcoremain(__argc, __argv, _environ);
    msvcoremain(args, arg, env);

    // Glfw
    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE, 1);

    glfwWindowHint(GLFW_SAMPLES, 4);

    // Window
    GLFWwindow *window;
    window = glfwCreateWindow((GLsizei)MsvWnd.screen.x, (GLsizei)MsvWnd.screen.y, VString(LString() + PROJECTNAME + " " + PROJECTVER[0].ver + _msv_zero_str), 0, NULL); // 

    // Position
    if (MsvWnd.pos.x >= 0) {
        glfwSetWindowPos(window, MsvWnd.pos.x, MsvWnd.pos.y);
    }

    if (MsvWnd.maximized)
        glfwMaximizeWindow(window);

    glfwMakeContextCurrent(window);

    gladLoadGL();
    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    // OpenGL state 
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glLineWidth(2.0);

    // Debug
    MsvGlDebug(1, 0);

    // Init windows
    MsvWnd.Init(window);
    MsvWndUpdateTitle();
    MsvWndResize(window, (GLsizei)MsvWnd.screen.x, (GLsizei)MsvWnd.screen.y);

    // Initialize ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &imgui = ImGui::GetIO(); (void)imgui;
    ImGui::StyleColorsLight();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Extender options
    srand(time());

    // Callbacks
    glfwSetWindowSizeCallback(window, MsvWndResize);
    glfwSetKeyCallback(window, MsvWndKeyCallback);
    glfwSetMouseButtonCallback(window, MsvWndMouseClickCallback);
    glfwSetCursorPosCallback(window, MsvWndMouseMotionCallback);
    glfwSetScrollCallback(window, MsvWndMouseScrollCallback);
    //glfwSetDropCallback(window, MsvWndDrop);
    glfwSetWindowCloseCallback(window, MsvWndClose);

    // Variables to be changed in the ImGUI window
    bool drawTriangle = true;
    float size = 1.0f;
    float color[4] = { 0.8f, 0.3f, 0.02f, 1.0f };

    // Process
    while (!glfwWindowShouldClose(window)) {
        //double delta = glfwGetTime();
        MsvWndUpdate(window, 0);
        //		MaticalsOpenGl.UpdateTime(delta);
        MsvWndRenderScene(window, 0);

        // Tell OpenGL a new frame is about to begin
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        MsvWndRender();

        // Renders the ImGUI elements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //glsl.Render(draw_text.);

        /*
        proj.Render(delta);
        //dfont.Draw();

        if(MaticalsOpenGl.IsUpdateTitle()){
            glfwSetWindowTitle(window, MaticalsOpenGl.GetWindowTitle());
        }

        // Render
        if(MaticalsOpenGl.IsRender()){
            MaticalsOpenGl.time_now = 1. * MaticalsOpenGl.frame_id / MaticalsOpenGl.opt_fps;
            MaticalsOpenGl.time_delta = 1. / MaticalsOpenGl.opt_fps;

            //MaticalsOpenGlRender.Write(MaticalsOpenGl.frame_id, proj.GetScriptTime() * MaticalsOpenGl.opt_fps);

            //MaticalsOpenGl.frame_id ++;

            if(MaticalsOpenGl.frame_id >= proj.GetScriptTime() * MaticalsOpenGl.opt_fps)
                break;
        }*/

        //glfwSetTime(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Deletes all ImGUI instances
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);

    return 0;
}
