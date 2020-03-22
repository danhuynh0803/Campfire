#include "TentGui.h"

#include "GlObject.h"

const int TAG_LENGTH = 32;

void TentGui::Init(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
    // Setup Dear ImGui style;
    ImGui::StyleColorsDark();
}

void TentGui::RenderGUI(ObjectManager& manager)
{
    ShowSceneHierarchy(manager);
    ShowMetrics();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void TentGui::ShowPrimitiveGenerator(ObjectManager& manager)
{
    static int objectIdx = -1;
    const char* objects[] = {"Cube", "Quad", "Sphere", "Light"};
    static Geometry selectedGeom = NONE;

    ImGui::Text("Generate Primitive");
    if (ImGui::Button("Geometry"))
    {
        ImGui::OpenPopup("selectPopup");
    }
    ImGui::SameLine();
    ImGui::TextUnformatted(objectIdx == -1 ? "<None>" : objects[objectIdx]);
    if (ImGui::BeginPopup("selectPopup"))
    {
        ImGui::Text("Primitives");
        ImGui::Separator();
        for (int i = 0; i < IM_ARRAYSIZE(objects); i++)
        {
            if (ImGui::Selectable(objects[i]))
            {
                objectIdx = i;
                selectedGeom = static_cast<Geometry>(i);
            }
        }

        ImGui::EndPopup();
    }

    static char tag[TAG_LENGTH] = "";
    ImGui::InputTextWithHint("Tag", "Enter object tag here", tag, IM_ARRAYSIZE(tag));

    static float position[3] = { 0.0f, 0.0f, 0.0f };
    ImGui::DragFloat3("Position", position, 0.01f);
    ImGui::Spacing();

    static float rotation[3] = { 0.0f, 0.0f, 0.0f };
    ImGui::DragFloat3("Rotation", rotation, 0.01f);
    ImGui::Spacing();

    static float scale[3] = { 1.0f, 1.0f, 1.0f };
    ImGui::DragFloat3("Scale", scale, 0.01f);
    ImGui::Spacing();

    std::string tagString(tag);
    if (ImGui::Button("Generate"))
    {
        manager.LoadObject(selectedGeom, tagString, position, rotation, scale);
    }
}


void TentGui::ShowObjects(ObjectManager& manager)
{
    static int selected = -1;

    ImGui::Text("Scene Objects");
    // TODO
    // Note: the number 105 was just chosen by eye
    // Eventually figure out a way that takes into account the button's width
    ImGui::SameLine(ImGui::GetWindowWidth()-105);
    if (ImGui::Button("Remove Object") && selected != -1)
    {
        manager.RemoveObject(selected);
        selected = -1;
    }

    int i = 0;
    for (auto object : manager.objectList)
    {
        char tag[TAG_LENGTH];
        sprintf(tag, "Tag:%s", object->name.c_str());
        if (ImGui::Selectable(tag, selected == i))
        {
            selected = i;
        }
        ++i;
    }

    if (selected != -1)
    {
        ShowInspector(manager.objectList[selected]);
    }
}


void TentGui::ShowMetrics()
{
    ImGui::Begin("Metrics");
    static bool animate = true;
    ImGui::Checkbox("Animate", &animate);

    static float values[90] = {};
    static int values_offset = 0;
    static double refresh_time = 0.0;
    if (!animate || refresh_time == 0.0)
        refresh_time = ImGui::GetTime();

    while (refresh_time < ImGui::GetTime()) // Create dummy data at fixed 60 hz rate for the demo
    {
        static float phase = 0.0f;
        values[values_offset] = cosf(phase);
        values_offset = (values_offset+1) % IM_ARRAYSIZE(values);
        phase += 0.10f*values_offset;
        refresh_time += 1.0f/60.0f;
    }

    // Display Frame window
    {
        float average = 0.0f;
        for (int n = 0; n < IM_ARRAYSIZE(values); n++)
            average += values[n];
        average /= (float)IM_ARRAYSIZE(values);
        char overlay[32];
        sprintf(overlay, "avg %f", average);
        ImGui::PlotLines("Frame times", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0,80));
    }

    ImGui::End();
}

void TentGui::ShowLights(LightManager manager)
{
    static int selected = -1;

    ImGui::Text("Light Objects");
    // TODO
    // Note: the number 105 was just chosen by eye
    // Eventually figure out a way that takes into account the button's width
    ImGui::SameLine(ImGui::GetWindowWidth()-105);
    if (ImGui::Button("Remove Object") && selected != -1)
    {
        //DeleteObject(selected);
        selected = -1;
    }

    int i = 0;
    for (auto object : manager.objectList)
    {
        char tag[TAG_LENGTH];
        sprintf(tag, "Idx:%d Tag:%s", i, object->name.c_str());
        if (ImGui::Selectable(tag, selected == i))
        {
            selected = i;
        }
        ++i;
    }

    if (selected != -1)
    {
        ShowInspector(manager.objectList[selected]);
    }
}

void TentGui::ShowInspector(Light* object)
{
    // Show and be able to modify information on selected object
    ImGui::Begin("Light Inspector");

    // TODO highlight the selected object in the scene
    {

    }

    { // TODO Draw object local coordinates to show orientation

    }

    ImGui::Checkbox("", &object->isActive);
    ImGui::SameLine();
    // TODO: cleaner way of doing this somehow?
    char tag[TAG_LENGTH];
    strcpy(tag, object->name.c_str());
    ImGui::InputText("Tag", tag, IM_ARRAYSIZE(tag));
    object->name.assign(tag);

    { // Transform info
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Transform"))
        {
            float position[3] = {
                object->position.x,
                object->position.y,
                object->position.z
            };
            ImGui::DragFloat3("Position", position, 0.01f);
            object->position = glm::make_vec3(position);
            ImGui::Spacing();

            float rotation[3] = {
                object->rotation.x,
                object->rotation.y,
                object->rotation.z
            };
            ImGui::DragFloat3("Rotation", rotation, 0.1f);
            object->rotation = glm::make_vec3(rotation);
            ImGui::Spacing();

            float scale[3] = {
                object->scale.x,
                object->scale.y,
                object->scale.z
            };
            ImGui::DragFloat3("Scale", scale, 0.01f);
            object->scale = glm::make_vec3(scale);
            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    { // Attenuation factors
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Light Settings"))
        {
            float linear = object->linear;
            float quadratic = object->quadratic;
            float* color = glm::value_ptr(object->color);

            ImGui::ColorEdit4("Light Color", color);
            ImGui::DragFloat("Linear", &linear, 0.01f);
            ImGui::DragFloat("Quadratic", &quadratic, 0.01f);

            object->color = glm::make_vec4(color);
            object->linear = linear;
            object->quadratic = quadratic;

            ImGui::TreePop();
        }
    }

    { // TODO what other details to add?
    }

    ImGui::End();
}


void TentGui::ShowInspector(GlObject* object)
{
    // Show and be able to modify information on selected object
    ImGui::Begin("Inspector");

    // TODO highlight the selected object in the scene
    {

    }

    { // TODO Draw object local coordinates to show orientation

    }

    ImGui::Checkbox("", &object->isActive);
    ImGui::SameLine();
    // TODO: cleaner way of doing this somehow?
    char tag[TAG_LENGTH];
    strcpy(tag, object->name.c_str());
    ImGui::InputText("Tag", tag, IM_ARRAYSIZE(tag));
    object->name.assign(tag);

    { // Transform info
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Transform"))
        {
            float position[3] = {
                object->position.x,
                object->position.y,
                object->position.z
            };
            ImGui::DragFloat3("Position", position, 0.01f);
            object->position = glm::make_vec3(position);
            ImGui::Spacing();

            float rotation[3] = {
                object->rotation.x,
                object->rotation.y,
                object->rotation.z
            };
            ImGui::DragFloat3("Rotation", rotation, 0.1f);
            object->rotation = glm::make_vec3(rotation);
            ImGui::Spacing();

            float scale[3] = {
                object->scale.x,
                object->scale.y,
                object->scale.z
            };
            ImGui::DragFloat3("Scale", scale, 0.01f);
            object->scale = glm::make_vec3(scale);
            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    { // Mesh details
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Mesh Details"))
        {
            // TODO allow loading up new texture and shader
            Texture objectTex = object->texture;
            ImGui::Text("Tex: %s", objectTex.GetName().c_str());
            ImGui::Text("Dim: %dx%d", objectTex.width, objectTex.height);
            ImGui::Image((void*)(intptr_t)objectTex.ID, ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0));

            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    { // TODO what other details to add?

    }

    ImGui::End();
}


void TentGui::ShowSceneHierarchy(ObjectManager& manager)
{
    ImGui::Begin("Scene Hierarchy");

    ShowPrimitiveGenerator(manager);

    ImGui::Separator();

    ShowObjects(manager);

    ImGui::Separator();

    //ShowLights(lightManager);

    ImGui::End();
}

