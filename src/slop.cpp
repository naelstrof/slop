#include <chrono>
#include <thread>

#include <stdlib.h>

#include "slopstates.hpp"
#include "mouse.hpp"
#include "resource.hpp"
#include "keyboard.hpp"

#include "window.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"
#include "glrectangle.hpp"
#include "xshaperectangle.hpp"
#include "slop.hpp"

namespace slop {

X11* x11;
Mouse* mouse;
Keyboard* keyboard;
Resource* resource;

SlopSelection GLSlopSelect( slop::SlopOptions* options, bool* cancelled, slop::SlopWindow* window );
SlopSelection XShapeSlopSelect( slop::SlopOptions* options, bool* cancelled);

}

using namespace slop;

// Defaults!
slop::SlopOptions::SlopOptions() {
    borderSize = 1;
    nokeyboard = false;
    noopengl = false;
    nodecorations = false;
    tolerance = 2;
    padding = 0;
    shader = "textured";
    highlight = false;
    r = 0.5;
    g = 0.5;
    b = 0.5;
    a = 1;

    char * envdisplay = getenv("DISPLAY");
    if (envdisplay == NULL)
        xdisplay = ":0";
    else
        xdisplay = envdisplay;
}

slop::SlopSelection::SlopSelection( float x, float y, float w, float h, int id ) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->id = id;
}

slop::SlopSelection slop::SlopSelect( slop::SlopOptions* options, bool* cancelled, bool quiet) {
    slop::SlopSelection returnval(0,0,0,0,0);
    bool deleteOptions = false;
    if ( !options ) {
        deleteOptions = true;
        options = new slop::SlopOptions();
    }
    resource = new Resource();
    // Set up x11 temporarily
    x11 = new X11(options->xdisplay);
    keyboard = new Keyboard( x11 );
    bool success = false;
    std::string errorstring = "";
    SlopWindow* window;
    // First we check if we have a compositor available
	if ( x11->hasCompositor() && !options->noopengl ) {
        // If we have a compositor, we try using OpenGL
        try {
            window = new SlopWindow();
            success = true;
        } catch (...) {
            success = false;
        }
    } else {
        errorstring += "Failed to detect a compositor, OpenGL hardware-accelleration disabled...\n";
    }
    if ( !success ) {
        // If we fail, we launch the XShape version of slop.
        if ( !quiet && !options->noopengl ) {
            if ( errorstring.length() <= 0 ) {
                errorstring += "Failed to launch OpenGL context, --shader parameter will be ignored.\n";
                std::cerr << errorstring;
            } else {
                std::cerr << errorstring;
            }
        }
        returnval = slop::XShapeSlopSelect( options, cancelled );
    } else {
        returnval = slop::GLSlopSelect( options, cancelled, window );
    }
    delete x11;
    delete slop::resource;
    if ( deleteOptions ) {
        delete options;
    }
    return returnval;
}

slop::SlopSelection slop::XShapeSlopSelect( slop::SlopOptions* options, bool* cancelled ) {
    // Init our little state machine, memory is a tad of a misnomer
    slop::SlopMemory memory( options, new XShapeRectangle(glm::vec2(0,0), glm::vec2(0,0), options->borderSize, options->padding, glm::vec4( options->r, options->g, options->b, options->a ), options->highlight) );
    slop::mouse = new slop::Mouse( x11, options->nodecorations, ((XShapeRectangle*)memory.rectangle)->window );

    // We have no GL context, so the matrix is useless...
    glm::mat4 fake;
    // This is where we'll run through all of our stuffs
    while( memory.running ) {
        slop::mouse->update();
        slop::keyboard->update();
        // We move our statemachine forward.
        memory.update( 1 );

        // We don't actually draw anything, but the state machine uses
        // this to know when to spawn the window.
        memory.draw( fake );

        // X11 explodes if we update as fast as possible, here's a tiny sleep.
        XFlush(x11->display);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // Then we draw the framebuffer to the screen
        if ( (slop::keyboard->anyKeyDown() && !options->nokeyboard) || slop::mouse->getButton( 3 ) ) {
            memory.running = false;
            if ( cancelled ) {
                *cancelled = true;
            }
        } else {
            *cancelled = false;
        }
    }

    // Now we should have a selection! We parse everything we know about it here.
    glm::vec4 output = memory.rectangle->getRect();

    // Lets now clear both front and back buffers before closing.
    // hopefully it'll be completely transparent while closing!
    // Then we clean up.
    delete slop::mouse;
    // Finally return the data.
    return slop::SlopSelection( output.x, output.y, output.z, output.w, memory.selectedWindow );
}

slop::SlopSelection slop::GLSlopSelect( slop::SlopOptions* options, bool* cancelled, SlopWindow* window ) {
    slop::mouse = new slop::Mouse( x11, options->nodecorations, window->window );

    if ( options->shader != "textured" ) {
        window->framebuffer->setShader( options->shader );
    }

    // Init our little state machine, memory is a tad of a misnomer
    slop::SlopMemory memory( options, new GLRectangle(glm::vec2(0,0), glm::vec2(0,0), options->borderSize, options->padding, glm::vec4( options->r, options->g, options->b, options->a ), options->highlight) );

    // This is where we'll run through all of our stuffs
    while( memory.running ) {
        slop::mouse->update();
        slop::keyboard->update();
        // We move our statemachine forward.
        memory.update( 1 );

        // Then we draw our junk to a framebuffer.
        window->framebuffer->bind();
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glClear (GL_COLOR_BUFFER_BIT);
        memory.draw( window->camera );
        window->framebuffer->unbind();

        // Then we draw the framebuffer to the screen
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glClear (GL_COLOR_BUFFER_BIT);
        window->framebuffer->draw();
        window->display();
        GLenum err = glGetError();
        if ( err != GL_NO_ERROR ) {
            throw err;
        }
        if ( (slop::keyboard->anyKeyDown() && !options->nokeyboard) || slop::mouse->getButton( 3 ) ) {
            memory.running = false;
            if ( cancelled ) {
                *cancelled = true;
            }
        } else {
            *cancelled = false;
        }
    }

    // Now we should have a selection! We parse everything we know about it here.
    glm::vec4 output = memory.rectangle->getRect();

    // Lets now clear both front and back buffers before closing.
    // hopefully it'll be completely transparent while closing!
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT);
    window->display();
    glClear (GL_COLOR_BUFFER_BIT);
    window->display();
    // Then we clean up.
    delete window;
    delete slop::mouse;
    // Finally return the data.
    return slop::SlopSelection( output.x, output.y, output.z, output.w, memory.selectedWindow );
}
