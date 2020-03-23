#include "TentGui.h"

#include "GlObject.h"
#include "Light.h"
#include "FrameBuffer.h"

#include <vector>

const int TAG_LENGTH = 32;

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

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

void TentGui::RenderStateButtons()
{
    ImGui::Begin("Game State");
    static int state = 1; // State in stop state (scene mode)
    ImGui::RadioButton("Play",  &state, 0); ImGui::SameLine();
    ImGui::RadioButton("Stop",  &state, 1); ImGui::SameLine();
    ImGui::RadioButton("Pause", &state, 2);
    ImGui::End();
}

void TentGui::RenderGUI(ObjectManager& manager)
{
    RenderStateButtons();
    ShowSceneHierarchy(manager);
    ShowMetrics();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void TentGui::ShowRenderPasses(const std::vector<FrameBuffer>& renderPasses)
{
    ImGui::Begin("Render Passes Outputs");
    if (ImGui::TreeNode("Render Passes"))
    {
        for (int i = 0; i < renderPasses.size(); ++i)
        {
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode((void*)(intptr_t)i, "%s", renderPasses[i].name))
            {
                ImGui::Image((void*)(intptr_t)renderPasses[i].texture.ID, ImVec2(1600/4, 900/4), ImVec2(0,1), ImVec2(1,0));
                ImGui::Separator();
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
    ImGui::End();
}


void TentGui::ShowCamera(Camera& cam)
{
    ImGui::Begin("Camera");

    float position[3] = {
        cam.Position.x,
        cam.Position.y,
        cam.Position.z
    };
    ImGui::DragFloat3("Position", position, 0.01f);
    ImGui::DragFloat("Near Plane", &cam.nearPlane, 0.01f);
    ImGui::DragFloat("Far Plane" , &cam.farPlane , 0.01f);

    ImGui::Separator();

    ImGui::End();
}


void TentGui::ShowPrimitiveGenerator(ObjectManager& manager)
{
    static int objectIdx = -1;
    const char* objects[] = {"Cube", "Quad", "Sphere", "Light"};
    static Geometry selectedGeom = NONE;

    ImGui::Text("Generate Primitive");
    ImGui::Combo("Geometry", &objectIdx, objects, IM_ARRAYSIZE(objects));
    ImGui::SameLine(); HelpMarker("Select a geometry to spawn into the scene.\n");
    selectedGeom = static_cast<Geometry>(objectIdx);

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

    // TODO light inspector breaks when
    // no longer using i for index
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

    if (object->isLight)
    { // Attenuation factors
        Light* light = static_cast<Light*>(object);
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Light Settings"))
        {
            float linear = light->linear;
            float quadratic = light->quadratic;
            float* color = glm::value_ptr(light->color);

            ImGui::ColorEdit4("Light Color", color);
            ImGui::DragFloat("Linear", &linear, 0.01f);
            ImGui::DragFloat("Quadratic", &quadratic, 0.01f);

            light->color = glm::make_vec4(color);
            light->linear = linear;
            light->quadratic = quadratic;

            ImGui::TreePop();
        }
    }
    else
    { // Mesh details
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Mesh Details"))
        {
            Texture objectTex = object->texture;
            static char tex[64];
            strcpy(tex, objectTex.GetName().c_str());
            ImGui::InputText("Texture", tex, IM_ARRAYSIZE(tag));

            // TODO allow loading up new texture and shader
            //if (strcmp(tex, objectTex.GetName().c_str()) != 0)
            //{
            //    std::string texPath;
            //    texPath.assign(tex);
            //    Texture newTexture(tex);
            //    object->texture = newTexture;
            //}

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

    ImGui::End();
}


