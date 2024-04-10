// This code is a modification of the original "cefsimple" example that can be
// found at https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage

#ifndef BROWSERVIEW_HPP
#define BROWSERVIEW_HPP

// OpenGL
#include <gl.h>

// Chromium Embedded Framework
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_render_handler.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

typedef struct GLModifiers_ {
    bool shift;
    bool ctrl;
    bool alt;
    bool leftMouse;
    bool middleMouse;
    bool rightMouse;
} GLModifiers_;

// ****************************************************************************
//! \brief Interface class rendering a single web page.
// ****************************************************************************
class BrowserView {
  public:
    //! \brief Default Constructor using a given URL.
    BrowserView(const std::string& url);

    //! \brief
    ~BrowserView();

    //! \brief Load the given web page.
    void load(const std::string& url);

    //! \brief Render the web page.
    void draw();

    //! \brief Set the windows size.
    void reshape(int w, int h);

    //! \brief Set the viewport: the rectangle on the window where to display
    //! the web document.
    //! \return false if arguments are incorrect.
    bool viewport(float x, float y, float w, float h);

    //! \brief Get the viewport.
    inline const glm::vec4& viewport() const { return m_viewport; }

    //! \brief TODO
    // void executeJS(const std::string &cmd);

    //! \brief Set the new mouse position
    void mouseMove(int x, int y, GLModifiers_ mod);

    //! \brief Set the new mouse state (clicked ...)
    void mouseClick(int btn, int action, int mods);

    //! \brief Set the new keyboard state (char typed ...)
    void keyPress(int key, int scanCode, int mods, int action);

    void setMenuVisible(bool vis);

  private:
    // *************************************************************************
    //! \brief Private implementation to handle CEF events to draw the web page.
    // *************************************************************************
    class RenderHandler : public CefRenderHandler {
      public:
        RenderHandler(const glm::vec4& viewport);

        //! \brief
        ~RenderHandler();

        //! \brief Compile OpenGL shaders and create OpenGL objects (VAO,
        //! VBO, texture, locations ...)
        bool init();

        //! \brief Render OpenGL VAO (rotating a textured square)
        void draw(const glm::vec4& viewport, bool fixed);

        //! \brief Resize the view
        void reshape(int w, int h);

        //! \brief Return the OpenGL texture handle
        GLuint texture() const { return m_tex; }

        //! \brief CefRenderHandler interface
        virtual void GetViewRect(CefRefPtr<CefBrowser> browser,
                                 CefRect& rect) override;

        //! \brief CefRenderHandler interface
        //! Update the OpenGL texture.
        virtual void OnPaint(CefRefPtr<CefBrowser> browser,
                             PaintElementType type, const RectList& dirtyRects,
                             const void* buffer, int width,
                             int height) override;

        //! \brief CefBase interface
        IMPLEMENT_REFCOUNTING(RenderHandler);

      private:
        //! \brief Dimension
        int m_width;
        int m_height;

        //! \brief Where to draw on the OpenGL window
        const glm::vec4& m_viewport;

        //! \brief OpenGL shader program handle
        GLuint m_prog = 0;
        //! \brief OpenGL texture handle
        GLuint m_tex = 0;
        //! \brief OpenGL vertex array object handle
        GLuint m_vao = 0;
        //! \brief OpenGL vertex buffer obejct handle
        GLuint m_vbo = 0;

        //! \brief OpenGL shader variable locations for vertices of the
        //! rectangle
        GLint m_pos_loc = -1;
        //! \brief OpenGL shader variable locations for the texture
        GLint m_tex_loc = -1;
        //! \brief OpenGL shader variable locations for the Model View
        //! Projection matrix.
        GLint m_mvp_loc = -1;
    };

    // *************************************************************************
    //! \brief Provide access to browser-instance-specific callbacks. A single
    //! CefClient instance can be shared among any number of browsers.
    // *************************************************************************
    class BrowserClient : public CefClient {
      public:
        BrowserClient(CefRefPtr<CefRenderHandler> ptr) : m_renderHandler(ptr) {}

        virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override {
          return m_renderHandler;
        }

        bool OnProcessMessageReceived(
            CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
            CefProcessId source_process,
            CefRefPtr<CefProcessMessage> message) override;

        CefRefPtr<CefRenderHandler> m_renderHandler;

        IMPLEMENT_REFCOUNTING(BrowserClient);
    };

  private:
    //! \brief Mouse cursor position on the OpenGL window
    int m_mouse_x;
    int m_mouse_y;

    //! \brief Where to draw on the OpenGL window
    glm::vec4 m_viewport;

    //! \brief Chromium Embedded framework elements
    CefRefPtr<CefBrowser> m_browser;
    CefRefPtr<BrowserClient> m_client;
    RenderHandler* m_render_handler = nullptr;

    //! \brief OpenGL has created GPU elements with success
    bool m_initialized = false;

  public:
    //! \brief If set to false then the web page is turning.
    bool m_fixed = true;
};

#endif  // BROWSERVIEW_HPP
