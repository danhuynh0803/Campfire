#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"

glm::mat4 Renderer::viewProjMatrix = glm::mat4(1.0f);
SharedPtr<Shader> Renderer::shader;

SharedPtr<Texture2D> whiteTexture;

void Renderer::Init()
{
    RenderCommand::Init();
    Renderer2D::Init();

    shader = ShaderManager::Create("generic", "../Campfire/Shaders/generic.vert", "../Campfire/Shaders/generic.frag");
    shader->Bind();
    shader->SetUniformBlock("Camera", 0);
    shader->SetUniformBlock("LightBuffer", 1);

    // If no textures available then use the default white texture
    whiteTexture = Texture2D::Create(1, 1);
    uint32_t whiteTextureData = 0xffffffff;
    whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
}

void Renderer::Shutdown()
{
    Renderer2D::Shutdown();
}

void Renderer::BeginScene(Camera& camera)
{
    RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    RenderCommand::Clear();

    viewProjMatrix = camera.GetViewProjMatrix();

    Renderer2D::BeginScene(camera);
}

void Renderer::EndScene()
{
    Renderer2D::EndScene();
}

void Renderer::SubmitMesh(const SharedPtr<Mesh>& mesh, const glm::mat4& transform, SharedPtr<Material> overrideMaterial)
{
    shader->Bind();
    shader->SetMat4("model", transform);

    // TODO Replace with Materials
    for (auto submesh : mesh->GetSubmeshes())
    {
        // Load default white texture of model had no textures
        if (submesh.textures.size() == 0)
        {
            //LOG_WARN("{0}::Warning: No material found, defaulting to white texture", mesh->GetName());
            shader->SetFloat("texDiffuse", 0);
            whiteTexture->Bind();
        }
        else
        {
            for(size_t i = 0; i < submesh.textures.size(); ++i)
            {
                auto texture = submesh.textures[i];
                if (texture)
                {
                    shader->SetFloat("texDiffuse", i);
                    submesh.textures[i]->Bind(i);
                }
            }
        }

        RenderCommand::DrawIndexed(submesh.vertexArray);
    }

}

void Renderer::Draw(const SharedPtr<Shader>& shader, const SharedPtr<VertexArray>& vertexArray, const glm::mat4& transform)
{
    // TODO convert to using materials instead of passing shader
    shader->Bind();
    shader->SetMat4("model", transform);
    RenderCommand::DrawIndexed(vertexArray);
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
    RenderCommand::SetViewport(0, 0, width, height);
}


