#include "Vitrae/Renderers/OpenGL/FrameStore.hpp"
#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Renderers/OpenGL.hpp"
#include "Vitrae/Renderers/OpenGL/Texture.hpp"

#include "dynasma/standalone.hpp"

namespace Vitrae
{
OpenGLFrameStore::OpenGLFrameStore(const FrameStore::TextureBindParams &params)
{
    GLuint glFramebufferId;

    glGenFramebuffers(1, &glFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, glFramebufferId);

    int width, height;
    std::vector<PropertySpec> renderComponents;

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

        renderComponents.emplace_back(
            PropertySpec{.name = StandardShaderPropertyNames::FRAGMENT_OUTPUT,
                         .typeInfo = StandardShaderPropertyTypes::FRAGMENT_OUTPUT});
    }

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_contextSwitcher = FramebufferContextSwitcher{width, height, glFramebufferId};
    mp_renderComponents =
        dynasma::makeStandalone<PropertyList, std::vector<Vitrae::PropertySpec> &&>(
            std::move(renderComponents));
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
    gladLoadGL(); // seems we need to do this after setting the first context... for whatev reason

    mp_renderComponents =
        dynasma::makeStandalone<PropertyList, std::span<const Vitrae::PropertySpec>>(
            {{PropertySpec{.name = StandardShaderPropertyNames::FRAGMENT_OUTPUT,
                           .typeInfo = StandardShaderPropertyTypes::FRAGMENT_OUTPUT}}});

    m_contextSwitcher = WindowContextSwitcher{window, params.onClose, params.onDrag};

    // register callbacks
    glfwSetWindowUserPointer(window, &std::get<WindowContextSwitcher>(m_contextSwitcher));

    glfwSetWindowCloseCallback(window, [](GLFWwindow *window) {
        auto switcher = static_cast<WindowContextSwitcher *>(glfwGetWindowUserPointer(window));
        switcher->onClose();
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
        auto switcher = static_cast<WindowContextSwitcher *>(glfwGetWindowUserPointer(window));

        if (switcher->bLeft || switcher->bRight || switcher->bMiddle) {

            switcher->onDrag(glm::vec2(xpos, ypos) - switcher->lastPos, switcher->bLeft,
                             switcher->bRight, switcher->bMiddle);
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
    std::visit([](auto &contextSwitcher) { contextSwitcher.destroyContext(); }, m_contextSwitcher);
}

std::size_t OpenGLFrameStore::memory_cost() const
{
    return sizeof(OpenGLFrameStore);
}
void OpenGLFrameStore::resize(glm::vec2 size)
{
    /// TODO: resize capabilities
}

glm::vec2 OpenGLFrameStore::getSize() const
{
    return std::visit([](auto &contextSwitcher) { return contextSwitcher.getSize(); },
                      m_contextSwitcher);
}

dynasma::FirmPtr<const PropertyList> OpenGLFrameStore::getRenderComponents() const
{
    return mp_renderComponents;
}

void OpenGLFrameStore::sync()
{
    std::visit([](auto &contextSwitcher) { contextSwitcher.sync(); }, m_contextSwitcher);
}

void OpenGLFrameStore::enterRender(glm::vec2 topLeft, glm::vec2 bottomRight)
{
    std::visit([&](auto &contextSwitcher) { contextSwitcher.enterContext(topLeft, bottomRight); },
               m_contextSwitcher);
}

void OpenGLFrameStore::exitRender()
{
    std::visit([](auto &contextSwitcher) { contextSwitcher.exitContext(); }, m_contextSwitcher);
}

/*
Framebuffer drawing
*/

void OpenGLFrameStore::FramebufferContextSwitcher::destroyContext()
{
    glDeleteFramebuffers(1, &glFramebufferId);
}
glm::vec2 OpenGLFrameStore::FramebufferContextSwitcher::getSize() const
{
    return glm::vec2(width, height);
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

void OpenGLFrameStore::FramebufferContextSwitcher::sync() {}

/*
Window drawing
*/

void OpenGLFrameStore::WindowContextSwitcher::destroyContext()
{
    glfwDestroyWindow(window);
}
glm::vec2 OpenGLFrameStore::WindowContextSwitcher::getSize() const
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return glm::vec2(width, height);
}
void OpenGLFrameStore::WindowContextSwitcher::enterContext(glm::vec2 topLeft, glm::vec2 bottomRight)
{

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(topLeft.x * width, topLeft.y * height, (bottomRight.x - topLeft.x) * width,
               (bottomRight.y - topLeft.y) * height);
}
void OpenGLFrameStore::WindowContextSwitcher::exitContext() {}

void OpenGLFrameStore::WindowContextSwitcher::sync()
{
    // we need to call this somewhere, so before rendering is ok since it gets called periodically
    // calling this multiple times when we have multiple windows shouldn't be a problem
    glfwPollEvents();

    glfwSwapBuffers(window);
}

} // namespace Vitrae