#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

enum BufferAttachmentType
{
    // TODO
    // determine whether to attach depth stencil or color
    COLOR,
    DEPTH,
    STENCIL
};

struct FrameBuffer
{
    GLuint ID;
    GLuint texture;

    FrameBuffer(int width, int height)
    {
        glGenFramebuffers(1, &ID);
        glBindFramebuffer(GL_FRAMEBUFFER, ID);

        // To complete a framebuffer we need:
        // 1. attach at least one buffer (color, depth, or stencil)
        // 2. should at least have one color attachment
        // 3. attachments should be complete
        // 4. buffers should have the same number of samples
        // NOTE: if sampling data then use textures
        // else use renderbuffers

        // Attach a texture to fbo
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // Texture will be filled when we render to framebuffer
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        // Create rbo for depth and stencil
        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        // Attach rbo
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            // TODO move all prints to a imgui debug log
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer not complete!\n";
        }
        // Bind framebuffer if successful
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

#endif // FRAME_BUFFER_H
