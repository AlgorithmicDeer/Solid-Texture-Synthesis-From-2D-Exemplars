// /**
//  * @file main.cpp
//  * @brief "**Add Brief**"
//  * @author Lakshya Agrawal and Maanas Gaur
//  * Obj taken from : http://web.mit.edu/djwendel/www/weblogo/shapes/
// */
#include "utils/utils.hpp"
#include "globals/globals.h"
#include "camera/camera.hpp"
#include "mesh/mesh.hpp"
#include "texture/texture.hpp"

// Reference for normal map generation has been taken from:
// https://stackoverflow.com/questions/30801003/how-can-i-make-normal-map-for-given-image-in-opengl-programmatically
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++********************************************************************************************************
void generateNormalMap(std::string& inputImagePath,std::string& outputImagePath, float strength, int level){
    cv::Mat inputImage = cv::imread(inputImagePath);
    
    if (inputImage.empty()) {
        std::cerr << "Error: Unable to load input image." << std::endl;
        return;
    }

    // Convert the image to grayscale
    cv::Mat grayImage;
    cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);

    // Apply Sobel operator for edge detection
    cv::Mat gradX, gradY;
    cv::Sobel(grayImage, gradX, CV_32F, 1, 0);
    cv::Sobel(grayImage, gradY, CV_32F, 0, 1);

    // Combine gradient X and Y components to get normals
    cv::Mat normalMap = cv::Mat::zeros(gradX.size(), CV_32FC3);
    cv::Mat normalMapChannels[] = {gradX, gradY, cv::Mat::ones(gradX.size(), CV_32F)};
    cv::merge(normalMapChannels, 3, normalMap);
    cv::cvtColor(normalMap, normalMap, cv::COLOR_RGB2BGR);

    // Normalize the normals
    cv::normalize(normalMap, normalMap, 0, 255, cv::NORM_MINMAX);

    // Adjust strength and level
    normalMap *= strength;
    normalMap += level;

    // Convert the normal map to 8-bit unsigned integers
    normalMap.convertTo(normalMap, CV_8U);

    // Save the output normal map
    cv::imwrite(outputImagePath, normalMap);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*********************************************************************************************
int main()
{
    GLFWwindow *window = setupWindow(screen_width, screen_height);
    ImGuiIO &io = ImGui::GetIO();
    ImVec4 clearColor = ImVec4(0.8f, 0.8f, 0.8f, 1.00f);

    // unsigned int shaderProgram = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");
    unsigned int shaderProgram = createProgram("./shaders/vshader_without_NM.vs", "./shaders/fshader_without_NM.fs");
    //Get handle to light position variable in shader
    lpos_world_uniform = glGetUniformLocation(shaderProgram, "lpos_world");
    if(lpos_world_uniform == -1){
        fprintf(stderr, "Could not bind location: lpos_world\n");
    }

    //Get handle to eye normal variable in shader
    eye_normal_uniform = glGetUniformLocation(shaderProgram, "eye_normal");
    if(eye_normal_uniform == -1){
        fprintf(stderr, "Could not bind location: eye_normal. Specular Lighting Switched Off.\n");
    }
    glUseProgram(shaderProgram);
    Camera camera;
    camera.setupModelTransformation(shaderProgram);
    camera.setupViewTransformation(shaderProgram);
    camera.setupProjectionTransformation(shaderProgram);

    //--------------------------------------------
    // const char* exemplar_path="./2D_Exemplars_jpg/Exemplar_11.jpg";
    // generate_solid_texture(exemplar_path);
    //--------------------------------------------

    Mesh mesh;
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    std::string objPath = "src/Objects/dragon.obj"; // Default object
    std::string texturePath = "2D_Exemplars_jpg/Exemplar_11.jpg"; // Default texture path
    std::string NormalPath = "2D_ExemplarsNormal_jpg/ExemplarNormal_11.jpg";
    // mesh.createMeshObject(shaderProgram, VAO, objPath, texturePath,NormalPath); // Calling with default exemplar
    mesh.createMeshObject(shaderProgram, VAO, objPath, texturePath,NormalPath,texturePath,NormalPath,texturePath,NormalPath); // Calling with default exemplar

    oldX, oldY, currentX, currentY= 0.0;
    // bool isDragging = false;
    int prevLeftButtonState = GLFW_RELEASE;
    int mode = 1;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        camera.processMouseInput(window, prevLeftButtonState, oldX, oldY, currentX, currentY, isDragging);
        camera.rotateModel(mode, oldX, oldY, currentX, currentY, isDragging, modelT, vModel_uniform, viewT);

//  -------------------------------------------------
        glBindTexture(GL_TEXTURE_2D, texture);
// ----------------------------------------------------
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Set ImGui styles
        ImGuiStyle& style = ImGui::GetStyle();

        // Example: Dark theme
        style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.40f, 0.42f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.10f, 0.22f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.56f, 1.00f, 0.80f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.53f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.56f, 1.00f, 0.80f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.28f, 0.56f, 1.00f, 0.35f);
        style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

       
// *******************************************************************(Section 1 : Background Colour)**********************************************************************
          // ImGui for background color and object selection
        ImGui::SetNextWindowSize(ImVec2(450, 300));
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Background Color", (float *)&clearColor);

        static int selectedObject = -1;
        ImGui::Text("Select Object:");
        ImGui::Columns(3, "ObjectButtons", false);
        ImGui::SetColumnWidth(0, 100.0f);
        ImGui::SetColumnWidth(1, 100.0f);
        ImGui::SetColumnWidth(2, 100.0f);
// *******************************************************************(Section 2 : Object Selection)**********************************************************************
        if (ImGui::RadioButton("Bunny", selectedObject == 0)) {
            selectedObject = 0;
            objPath = "src/Objects/bunny.obj";
            // texturePath = "2D_Exemplars_jpg/Exemplar_7.jpg";
            // mesh.createMeshObject(shaderProgram, VAO, objPath, texturePath,NormalPath);
            mesh.createMeshObject(shaderProgram, VAO, objPath, texturePath,NormalPath,texturePath,NormalPath,texturePath,NormalPath);
        }
        ImGui::NextColumn();

        if (ImGui::RadioButton("Dragon", selectedObject == 1)) {
            selectedObject = 1;
            objPath = "src/Objects/dragon.obj";
            // texturePath = "2D_Exemplars_jpg/Exemplar_7.jpg"; // Default texture path for Dragon
            // mesh.createMeshObject(shaderProgram, VAO, objPath, texturePath,NormalPath);
            mesh.createMeshObject(shaderProgram, VAO, objPath, texturePath,NormalPath,texturePath,NormalPath,texturePath,NormalPath);
        }
        ImGui::NextColumn();

        if (ImGui::RadioButton("Penguin", selectedObject == 2)) {
            selectedObject = 2;
            objPath = "src/Objects/penguin.obj";
            // texturePath = "2D_Exemplars_jpg/Exemplar_7.jpg"; // Default texture path for Penguin
            // mesh.createMeshObject(shaderProgram, VAO, objPath, texturePath,NormalPath);
            mesh.createMeshObject(shaderProgram, VAO, objPath, texturePath,NormalPath,texturePath,NormalPath,texturePath,NormalPath);
        }
// *******************************************************************(Section 3 : Exemplar Selection)********************************************************************************************************
        ImGui::NextColumn();

        ImGui::Columns(1);

        ImGui::Text("Select Texture:");
        static const char* textureItems[] = { "Exemplar_1", "Exemplar_2", "Exemplar_3", "Exemplar_4", "Exemplar_5", "Exemplar_6", "Exemplar_7",
                                      "Exemplar_8", "Exemplar_9", "Exemplar_10", "Exemplar_11", "Exemplar_12", "Exemplar_13", "Exemplar_14",
                                      "Exemplar_15", "Exemplar_16", "Exemplar_17", "Exemplar_18", "Exemplar_19", "Exemplar_20", "Exemplar_21" };

        static int selectedTexture = 10; // Default Exemplar_6

       if (ImGui::Combo("Texture", &selectedTexture, textureItems, IM_ARRAYSIZE(textureItems))) {
        std::string texturePath = "2D_Exemplars_jpg/Exemplar_" + std::to_string(selectedTexture + 1) + ".jpg";
        std::string NormalPath = "2D_ExemplarsNormal_jpg/ExemplarNormal_" + std::to_string(selectedTexture + 1) + ".jpg";
        
        // mesh.createMeshObject(shaderProgram, VAO, objPath, texturePath,NormalPath);
        mesh.createMeshObject(shaderProgram, VAO, objPath, texturePath,NormalPath,texturePath,NormalPath,texturePath,NormalPath);
       }
        // Display the selected texture image
        // std::string imagePath = texturePath;
        // ImGui::Text("Selected Texture:");

        // ImTextureID textureID = reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture));  
        // ImGui::Image(textureID, ImVec2(100, 100)); 
        // ImGui::Text("Image Path: %s", imagePath.c_str());

// *******************************************************************(Section 4 : Expanded Exmeplar Selection)********************************************************************************************************

        ImGui::Text("Select Texture (Expanded):");
        static const char* textureItemsExpanded[] = { "Exemplar_1", "Exemplar_2", "Exemplar_3", "Exemplar_4", "Exemplar_5", "Exemplar_6", "Exemplar_7",
                                            "Exemplar_8", "Exemplar_9", "Exemplar_10", "Exemplar_11", "Exemplar_12", "Exemplar_13", "Exemplar_14",
                                            "Exemplar_15", "Exemplar_16", "Exemplar_17", "Exemplar_18", "Exemplar_19", "Exemplar_20", "Exemplar_21" };

        static int selectedTextureExpanded = 10; // Default Exemplar_6

        if (ImGui::Combo("Texture (Expanded)", &selectedTextureExpanded, textureItemsExpanded, IM_ARRAYSIZE(textureItemsExpanded))) {
            std::string texturePathExpanded = "2D_Exemplars_jpg/Exemplar_" + std::to_string(selectedTextureExpanded + 1) + ".jpg";
            // std::string NormalPathExpanded = "2D_ExemplarsNormal_jpg/ExemplarNormal_" + std::to_string(selectedTextureExpanded + 1) + ".jpg";

            // std::string outputPath = "texture_outputs/output_" + std::to_string(selectedTextureExpanded + 1) + ".jpg";
            // std::string outputNormalPath = "texture_outputs_normals/output_normal_map_" + std::to_string(selectedTextureExpanded + 1) + ".jpg";

            std::string outputPathXY = "texture_outputs_xy/output_" + std::to_string(selectedTextureExpanded + 1) + ".jpg";
            std::string outputNormalPathXY = "texture_outputs_normals_xy/output_normal_map_" + std::to_string(selectedTextureExpanded + 1) + ".jpg";

            std::string outputPathXZ = "texture_outputs_xz/output_" + std::to_string(selectedTextureExpanded + 1) + ".jpg";
            std::string outputNormalPathXZ = "texture_outputs_normals_xz/output_normal_map_" + std::to_string(selectedTextureExpanded + 1) + ".jpg";

            std::string outputPathYZ = "texture_outputs_yz/output_" + std::to_string(selectedTextureExpanded + 1) + ".jpg";
            std::string outputNormalPathYZ = "texture_outputs_normals_yz/output_normal_map_" + std::to_string(selectedTextureExpanded + 1) + ".jpg";

            // generate_solid_texture(texturePathExpanded);
            // generateNormalMap(outputPath, outputNormalPath, 1.0f, 128);


            generate_solid_texture(texturePathExpanded);
            generateNormalMap(outputPathXY, outputNormalPathXY, 1.0f, 128);
            generateNormalMap(outputPathXZ, outputNormalPathXZ, 1.0f, 128);
            generateNormalMap(outputPathYZ, outputNormalPathYZ, 1.0f, 128);

            mesh.createMeshObject(shaderProgram, VAO, objPath, outputPathYZ, outputNormalPathYZ, outputPathXZ, outputNormalPathXZ, outputPathXY, outputNormalPathXY);
            // mesh.createMeshObject(shaderProgram, VAO, objPath, outputPathYZ, outputNormalPathYZ, outputPathYZ, outputNormalPathYZ, outputPathYZ, outputNormalPathYZ);

            // mesh.createMeshObject(shaderProgram, VAO, objPath, texturePath,NormalPath,texturePath,NormalPath,texturePath,NormalPath);

            // mesh.createMeshObject(shaderProgram, VAO, objPath, outputPath, outputNormalPath);
        }
        // Display the selected texture image
        // std::string imagePathExpanded = outputPath;
        ImGui::Text("Selected Texture (Expanded):");

        ImTextureID textureIDExpanded = reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture));
        ImGui::Image(textureIDExpanded, ImVec2(100, 100));

// ***************************************************************************************************************************************************************************************************************************
        ImGui::End();


        glUseProgram(shaderProgram);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);

        glUniform3f(lpos_world_uniform, -50.0, 500.0, 30.0);
        glUniform3f(eye_normal_uniform, 40.0, -40.0, 40.0);

        glDrawArrays(GL_TRIANGLES, 0, nVertices);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    cleanup(window);
}




    

 