#ifdef SLOP_OPENGL
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#include <chrono>
#include <thread>
#include <string>
#include <vector>

#include <stdlib.h>

#include "slopstates.hpp"
#include "mouse.hpp"
#include "resource.hpp"
#include "keyboard.hpp"

#ifdef SLOP_OPENGL
#include "window.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"
#include "glrectangle.hpp"
#endif

#include "xshaperectangle.hpp"
#include "slop.hpp"

namespace slop {

X11* x11;
Mouse* mouse;
Keyboard* keyboard;
Resource* resource;

#ifdef SLOP_OPENGL
SlopSelection GLSlopSelect( slop::SlopOptions* options, slop::SlopWindow* window );
#endif
SlopSelection XShapeSlopSelect( slop::SlopOptions* options );

static int TmpXError(Display * d, XErrorEvent * ev) {
    return 0;
}

}

template<typename Out>
static void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

static std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}


using namespace slop;

char slop_default_xdisplay[] = ":0";
char slop_default_shaders[] = "textured";

// Defaults!
slop::SlopOptions::SlopOptions() {
    border = 1;
    nokeyboard = false;
    noopengl = false;
    nodrag = false;
    nodecorations = false;
    tolerance = 2;
    padding = 0;
    shaders = slop_default_shaders;
    highlight = false;
    x_ratio = 0;
    y_ratio = 0;
    r = 0.5;
    g = 0.5;
    b = 0.5;
    a = 1;
    quiet = false;

    char* envdisplay = getenv("DISPLAY");
    if (envdisplay == NULL) {
        xdisplay = slop_default_xdisplay;
    } else {
        xdisplay = envdisplay;
    }
}

slop::SlopSelection::SlopSelection( float x, float y, float w, float h, int id, bool cancelled ) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->id = id;
    this->cancelled = cancelled;
}

slop::SlopSelection slop::SlopSelect( slop::SlopOptions* options ) {
    slop::SlopSelection returnval(0,0,0,0,0,true);
    bool deleteOptions = false;
    if ( !options ) {
        deleteOptions = true;
        options = new slop::SlopOptions();
    }
    resource = new Resource();
    // Set up x11 temporarily
    x11 = new X11(options->xdisplay);
    if ( !options->nokeyboard ) {
        XErrorHandler ph = XSetErrorHandler(slop::TmpXError);
        keyboard = new Keyboard( x11 );
        XSetErrorHandler(ph);
    }
#ifdef SLOP_OPENGL
    bool success = false;
    std::string errorstring = "";
    SlopWindow* window;
    // First we check if we have a compositor available
    if ( x11->hasCompositor() && !options->noopengl ) {
        // If we have a compositor, we try using OpenGL
        try {
            window = new SlopWindow();
            if (!GLEW_VERSION_3_0) {
                delete window;
                throw std::runtime_error( "OpenGL version is not high enough, slop requires OpenGL 3.0!\nOpenGL acceleration is disabled. Use -o or -q to suppress this message." );
            }
            success = true;
        } catch( std::exception& e ) {
            errorstring += std::string(e.what()) + "\n";
            success = false;
        } catch (...) {
            success = false;
        }
    } else {
        errorstring += "Failed to detect a compositor, OpenGL hardware-acceleration disabled...\n";
    }
    if ( !success ) {
        // If we fail, we launch the XShape version of slop.
        if ( !options->quiet && !options->noopengl ) {
            if ( errorstring.length() <= 0 ) {
                errorstring += "Failed to launch OpenGL context, --shader parameter will be ignored.\n";
                std::cerr << errorstring;
            } else {
                std::cerr << errorstring;
            }
        }
#endif // SLOP_OPENGL
        returnval = slop::XShapeSlopSelect( options );
#ifdef SLOP_OPENGL
    } else {
        returnval = slop::GLSlopSelect( options, window );
    }
#endif
    delete x11;
    delete slop::resource;
    if ( deleteOptions ) {
        delete options;
    }
    return returnval;
}

slop::SlopSelection slop::XShapeSlopSelect( slop::SlopOptions* options ) {
    // Init our little state machine, memory is a tad of a misnomer
    bool cancelled = false;
    slop::SlopMemory* memory = new slop::SlopMemory( options, new XShapeRectangle(glm::vec2(0,0), glm::vec2(0,0), options->border, options->padding, glm::vec4( options->r, options->g, options->b, options->a ), options->highlight) );
    slop::mouse = new slop::Mouse( x11, options->nodecorations, ((XShapeRectangle*)memory->rectangle)->window );

    // We have no GL context, so the matrix is useless...
    glm::mat4 fake;
    // This is where we'll run through all of our stuffs
    auto last = std::chrono::high_resolution_clock::now();
    while( memory->running ) {
        slop::mouse->update();
        if ( !options->nokeyboard ) {
            slop::keyboard->update();
        }
        // We move our statemachine forward.
        auto current = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> frametime = current-last;
        last = current;
        memory->update( frametime.count()/1000.f );

        // We don't actually draw anything, but the state machine uses
        // this to know when to spawn the window.
        memory->draw( fake );

        // X11 explodes if we update as fast as possible, here's a tiny sleep.
        XSync(x11->display, False);

        // Then we draw the framebuffer to the screen
        if ( (!options->nokeyboard && slop::keyboard->anyKeyDown()) || slop::mouse->getButton( 3 ) ) {
            memory->running = false;
            cancelled = true;
        }
    }

    // Now we should have a selection! We parse everything we know about it here.
    glm::vec4 output = memory->rectangle->getRect();

    // Lets now clear both front and back buffers before closing.
    // hopefully it'll be completely transparent while closing!
    // Then we clean up.
    delete slop::mouse;
    Window selectedWindow = memory->selectedWindow;
    delete memory;

    // Try to detect the window dying.
    int tries = 0;
    while( tries < 50 ) {
        XEvent event;
        if ( XCheckTypedEvent( x11->display, UnmapNotify, &event ) ) { break; }
        if ( XCheckTypedEvent( x11->display, DestroyNotify, &event ) ) { break; }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        tries++;
    }
    // Finally return the data.
    return slop::SlopSelection( output.x, output.y, output.z, output.w, selectedWindow, cancelled );
}

#ifdef SLOP_OPENGL
slop::SlopSelection slop::GLSlopSelect( slop::SlopOptions* options, SlopWindow* window ) {
    bool cancelled = false;
    slop::mouse = new slop::Mouse( x11, options->nodecorations, window->window );

    std::string vert = "#version 120\nattribute vec2 position;\nattribute vec2 uv;\nvarying vec2 uvCoord;\nvoid main()\n{\nuvCoord = uv;\ngl_Position = vec4(position,0,1);\n}\n";
    std::string frag = "#version 120\nuniform sampler2D texture;\nvarying vec2 uvCoord;\nvoid main()\n {\ngl_FragColor = texture2D( texture, uvCoord );\n}\n";
    slop::Shader* textured = new slop::Shader( vert, frag, false );
    std::vector<slop::Shader*> shaders;
    std::vector<std::string> stringShaders = split( options->shaders, ',' );
    for( int i=0;i<stringShaders.size();i++ ) {
        std::string sn = stringShaders[i];
        if ( sn != "textured" ) {
            shaders.push_back( new slop::Shader( sn + ".vert", sn + ".frag" ) );
        } else {
            shaders.push_back( textured );
        }
    }
    // Init our little state machine, memory is a tad of a misnomer
    slop::SlopMemory* memory = new slop::SlopMemory( options, new GLRectangle(glm::vec2(0,0), glm::vec2(0,0), options->border, options->padding, glm::vec4( options->r, options->g, options->b, options->a ), options->highlight) );

    slop::Framebuffer *ball = new slop::Framebuffer(WidthOfScreen(x11->screen), HeightOfScreen(x11->screen));
    slop::Framebuffer *ping, *pong;

    // This is where we'll run through all of our stuffs
    auto start = std::chrono::high_resolution_clock::now();
    auto last = start;
    while( memory->running ) {
        slop::mouse->update();
        if ( !options->nokeyboard ) {
            slop::keyboard->update();
        }
        // We move our statemachine forward.
        auto current = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> frametime = current-last;
        last = current;
        memory->update( frametime.count()/1000.f );

        // Then we draw our junk to a framebuffer.
        window->framebuffer->setShader( textured );
        window->framebuffer->bind();
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glClear (GL_COLOR_BUFFER_BIT);
        memory->draw( window->camera );
        window->framebuffer->unbind();

        std::chrono::duration<double, std::milli> elapsed = current-start;
        ping=ball;
        pong=window->framebuffer;

        // We have our clean buffer, now to slather it with some juicy shader chains.
        glEnable( GL_BLEND );
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (int i=0; i < shaders.size() - 1; i++) {
            ping->bind();
            glClearColor (0.0, 0.0, 0.0, 0.0);
            glClear (GL_COLOR_BUFFER_BIT);
            pong->setShader( shaders[i] );
            pong->draw(slop::mouse->getMousePos(), elapsed.count()/1000.f, glm::vec4( options->r, options->g, options->b, options->a ) );
            ping->unbind();
            std::swap(ping, pong);
        }
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glClear (GL_COLOR_BUFFER_BIT);
        pong->setShader( shaders[shaders.size() - 1] );
        pong->draw(slop::mouse->getMousePos(), elapsed.count()/1000.f, glm::vec4( options->r, options->g, options->b, options->a ) );
        glDisable( GL_BLEND );

        window->display();
        // Here we sleep just to prevent our CPU usage from going to 100%.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        GLenum err = glGetError();
        if ( err != GL_NO_ERROR ) {
            std::string error;
            switch(err) {
                case GL_INVALID_OPERATION: error="INVALID_OPERATION"; break;
                case GL_INVALID_ENUM: error="INVALID_ENUM"; break;
                case GL_INVALID_VALUE: error="INVALID_VALUE"; break;
                case GL_OUT_OF_MEMORY: error="OUT_OF_MEMORY"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: error="INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            throw std::runtime_error( "OpenGL threw an error: " + error );
        }
        if ( (!options->nokeyboard && slop::keyboard->anyKeyDown()) || slop::mouse->getButton( 3 ) ) {
            memory->running = false;
            cancelled = true;
        }
    }

    // Now we should have a selection! We parse everything we know about it here.
    glm::vec4 output = memory->rectangle->getRect();

    // Lets now clear both front and back buffers before closing.
    // hopefully it'll be completely transparent while closing!
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT);
    window->display();
    glClear (GL_COLOR_BUFFER_BIT);
    window->display();
    // Then we clean up.
    for( int i=0;i<shaders.size();i++ ) {
      delete shaders[i];
    }
    delete ball;
    delete window;
    delete slop::mouse;
    Window selectedWindow = memory->selectedWindow;
    delete memory;

    // Try to detect the window dying.
    int tries = 0;
    while( tries < 50 ) {
        XEvent event;
        if ( XCheckTypedEvent( x11->display, UnmapNotify, &event ) ) { break; }
        if ( XCheckTypedEvent( x11->display, DestroyNotify, &event ) ) { break; }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        tries++;
    }
    // Finally return the data.
    return slop::SlopSelection( output.x, output.y, output.z, output.w, selectedWindow, cancelled );
}
#endif

extern "C" struct slop_options slop_options_default() {
    struct slop_options options;
    options.border = 1;
    options.nokeyboard = false;
    options.noopengl = false;
    options.nodecorations = false;
    options.tolerance = 2;
    options.padding = 0;
    options.shaders = slop_default_shaders;
    options.highlight = false;
    options.x_ratio = 0;
    options.y_ratio = 0;
    options.r = 0.5;
    options.g = 0.5;
    options.b = 0.5;
    options.a = 1;
    options.quiet = false;

    char* envdisplay = getenv("DISPLAY");
    if (envdisplay == NULL) {
        options.xdisplay = slop_default_xdisplay;
    } else {
        options.xdisplay = envdisplay;
    }
    return options;
}

extern "C" struct slop_selection slop_select( struct slop_options* options ) {
    slop::SlopOptions realOptions = slop::SlopOptions();
    if ( options != NULL ) {
        realOptions.border = options->border;
        realOptions.nokeyboard = options->nokeyboard;
        realOptions.noopengl = options->noopengl;
        realOptions.nodecorations = options->nodecorations;
        realOptions.tolerance = options->tolerance;
        realOptions.padding = options->padding;
        realOptions.shaders = options->shaders;
        realOptions.highlight = options->highlight;
        realOptions.x_ratio = options->x_ratio;
        realOptions.y_ratio = options->y_ratio;
        realOptions.r = options->r;
        realOptions.g = options->g;
        realOptions.b = options->b;
        realOptions.a = options->a;
        realOptions.quiet = options->quiet;
        realOptions.xdisplay = options->xdisplay;
    }
    slop::SlopSelection select = SlopSelect( &realOptions );
    slop_selection realSelect;
    realSelect.x = select.x;
    realSelect.y = select.y;
    realSelect.w = select.w;
    realSelect.h = select.h;
    realSelect.id = select.id;
    realSelect.cancelled = select.cancelled;
    return realSelect;
}
