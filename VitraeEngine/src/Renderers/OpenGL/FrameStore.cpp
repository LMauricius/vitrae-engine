#include "Vitrae/Renderers/OpenGL/FrameStore.hpp"
#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Renderers/OpenGL.hpp"
#include "Vitrae/Renderers/OpenGL/Texture.hpp"

namespace Vitrae
{
OpenGLFrameStore::OpenGLFrameStore(const FrameStore::TextureBindParams &params)
{
    GLuint glFramebufferId;

    glGenFramebuffers(1, &glFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, glFramebufferId);

    int width, height;

    if (params.p_depthTexture.has_value()) {
        auto p_texture =
            dynasma::dynamic_pointer_cast<OpenGLTexture>(params.p_depthTexture.value());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                               p_texture->glTextureId, 0);
        width = p_texture->getSize().x;
        height = p_texture->getSize().y;
    }

    if (params.p_colorTexture.has_value()) {
        auto p_texture =
            dynasma::dynamic_pointer_cast<OpenGLTexture>(params.p_colorTexture.value());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               p_texture->glTextureId, 0);
        width = p_texture->getSize().x;
        height = p_texture->getSize().y;
    }

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_contextSwitcher = FramebufferContextSwitcher{width, height, glFramebufferId};
}

OpenGLFrameStore::OpenGLFrameStore(const WindowDisplayParams &params)
{
    GLFWwindow *window =
        glfwCreateWindow(params.width, params.height, params.title.c_str(), nullptr, nullptr);
    // provjeri je li se uspio napraviti prozor
    if (window == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    m_contextSwitcher = WindowContextSwitcher{window, params.onClose, params.onDrag};

    // register callbacks
    glfwSetWindowUserPointer(window, &std::get<WindowContextSwitcher>(m_contextSwitcher));

    glfwSetWindowCloseCallback(window, [](GLFWwindow *window) {
        auto switcher = static_cast<WindowContextSwitcher *>(glfwGetWindowUserPointer(window));
        switcher->onClose();
    });
    glfwSetWindowPosCallback(window, [](GLFWwindow *window, int xpos, int ypos) {
        auto switcher = static_cast<WindowContextSwitcher *>(glfwGetWindowUserPointer(window));

        if (switcher->bLeft || switcher->bRight || switcher->bMiddle) {

            switcher->onDrag(glm::vec2(xpos, ypos), switcher->bLeft, switcher->bRight,
                             switcher->bMiddle);
        }

        switcher->lastPos = glm::vec2(xpos, ypos);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
        auto switcher = static_cast<WindowContextSwitcher *>(glfwGetWindowUserPointer(window));
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            switcher->bLeft = action != GLFW_RELEASE;
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            switcher->bRight = action != GLFW_RELEASE;
        } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
            switcher->bMiddle = action != GLFW_RELEASE;
        }
    });
}

OpenGLFrameStore::~OpenGLFrameStore()
{
    std::visit([](auto &&contextSwitcher) { contextSwitcher.destroyContext(); }, m_contextSwitcher);
}

void OpenGLFrameStore::startRender(glm::vec2 topLeft, glm::vec2 bottomRight)
{
    std::visit([&](auto &&contextSwitcher) { contextSwitcher.enterContext(topLeft, bottomRight); },
               m_contextSwitcher);
}

void OpenGLFrameStore::finishRender()
{
    std::visit([](auto &&contextSwitcher) { contextSwitcher.exitContext(); }, m_contextSwitcher);
}

/*
Framebuffer drawing
*/

void OpenGLFrameStore::FramebufferContextSwitcher::destroyContext()
{
    glDeleteFramebuffers(1, &glFramebufferId);
}
void OpenGLFrameStore::FramebufferContextSwitcher::enterContext(glm::vec2 topLeft,
                                                                glm::vec2 bottomRight)
{
    glBindFramebuffer(GL_FRAMEBUFFER, glFramebufferId);
    glViewport(topLeft.x * width, topLeft.y * height, (bottomRight.x - topLeft.x) * width,
               (bottomRight.y - topLeft.y) * height);
}
void OpenGLFrameStore::FramebufferContextSwitcher::exitContext()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
Window drawing
*/

void OpenGLFrameStore::WindowContextSwitcher::destroyContext()
{
    glfwDestroyWindow(window);
}
void OpenGLFrameStore::WindowContextSwitcher::enterContext(glm::vec2 topLeft, glm::vec2 bottomRight)
{
    // we need to call this somewhere, so before rendering is ok since it gets called periodically
    // calling this multiple times when we have multiple windows shouldn't be a problem
    glfwPollEvents();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glfwMakeContextCurrent(window);
    glViewport(topLeft.x * width, topLeft.y * height, (bottomRight.x - topLeft.x) * width,
               (bottomRight.y - topLeft.y) * height);
}
void OpenGLFrameStore::WindowContextSwitcher::exitContext()
{
    glfwSwapBuffers(window);
    glfwMakeContextCurrent(0);
}

} // namespace Vitrae