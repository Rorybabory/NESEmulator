// NESEmulator2024.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Bus.h"
#include "CPU.h"
#include "Cartridge.h"

#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_memory_editor.h"

#include "Timer.h"

#include <nfd.h>

uint8_t button = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    key -= 65;
    key += 0x61;
    if (action == GLFW_PRESS)
    {
        button = key;
    }

}

int main()
{
    Bus* bus = new Bus();
    CPU* cpu = bus->getCPU();
    
    
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Emulator", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    //Setup IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();


    

    
    int display_w, display_h = 0;
    static MemoryEditor mem_edit;

    unsigned char data[255];

    bool playing = false;

    for (int i = 0; i < 255; ++i) {
        data[i] = i;
    }
    srand(time(0));
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        cpu->SetButton(button);

        {
            mem_edit.DrawWindow("RAM", bus->getRAM(), 0x800);
        }
        bus->cart->DrawPrgROM();
        //bus->cart->drawWindow();
        {
            ImGui::Begin("Controls");

            ImGui::SetWindowSize(ImVec2(350, 75));
            ImGui::SetWindowPos(ImVec2(1280 - 350, 0));

            if (ImGui::Button("Start")) {
                playing = true;
                printf("START\n");
            }
            ImGui::SameLine();

            if (ImGui::Button("Stop")) {
                playing = false;
            }
            ImGui::SameLine();

            if (ImGui::Button("Step")) {
                while (!cpu->Cycle()) {

                }
            }
            ImGui::SameLine();

            if (ImGui::Button("Reset")) {
                cpu->RestartClear();
                playing = false;
                bus->setDone(false);

            }
            ImGui::End();
        }
        cpu->DrawState();
        cpu->DrawScreen();
        
        if (playing) {
            Timer t = Timer("Timer");
            
            for (int i = 0; i < 351; i++) {
                cpu->Cycle();
            }
            t.Stop();
        }
        if (bus->getDone() == true && playing == true) {
            playing = false;
            std::cout << "Done\n";
        }
        ImGui::Render();
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.356f, 0.617f, 0.816f, 1);
        glClear(GL_COLOR_BUFFER_BIT);


        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    //std::cout << "Hello World!\n";
}

