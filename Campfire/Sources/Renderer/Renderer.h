#ifndef RENDERER_H
#define RENDERER_H

enum class RendererAPI
{
    None = 0,
    OpenGL
};

class Renderer
{
public:
    inline static RendererAPI GetAPI()
    {
        return rendererAPI;
    }
    inline static RendererAPI SetAPI(RendererAPI api)
    {
        return rendererAPI = api;
    }

private:
    static RendererAPI rendererAPI;
};

#endif // RENDERER_H
