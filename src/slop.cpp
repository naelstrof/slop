#include <chrono>
#include <thread>

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

X11* x11;
Mouse* mouse;
Keyboard* keyboard;
Resource* resource;

// Defaults!
SlopOptions::SlopOptions() {
    borderSize = 1;
    nokeyboard = false;
    nodecorations = false;
    tolerance = 2;
    padding = 0;
    shader = "textured";
    highlight = false;
    xdisplay = ":0";
    r = 0.5;
    g = 0.5;
    b = 0.5;
    a = 1;
}

SlopSelection::SlopSelection( float x, float y, float w, float h, int id ) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->id = id;
}

SlopSelection GLSlopSelect( SlopOptions* options, bool* cancelled, SlopWindow* window );
SlopSelection XShapeSlopSelect( SlopOptions* options, bool* cancelled);

SlopSelection SlopSelect( SlopOptions* options, bool* cancelled, bool quiet) {
    SlopSelection returnval(0,0,0,0,0);
    bool deleteOptions = false;
    if ( !options ) {
        deleteOptions = true;
        options = new SlopOptions();
    }
    resource = new Resource();
    // Set up x11 temporarily
    x11 = new X11(options->xdisplay);
    keyboard = new Keyboard( x11 );
    bool success = false;
    std::string errorstring = "";
    SlopWindow* window;
    // First we check if we have a compositor available
	if ( x11->hasCompositor() ) {
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
        if ( !quiet ) {
            if ( errorstring.length() <= 0 ) {
                errorstring += "Failed to launch OpenGL context, --shader parameter will be ignored.\n";
                std::cerr << errorstring;
            } else {
                std::cerr << errorstring;
            }
        }
        returnval = XShapeSlopSelect( options, cancelled );
    } else {
        returnval = GLSlopSelect( options, cancelled, window );
    }
    delete x11;
    delete resource;
    if ( deleteOptions ) {
        delete options;
    }
    return returnval;
}

SlopSelection XShapeSlopSelect( SlopOptions* options, bool* cancelled ) {
    // Init our little state machine, memory is a tad of a misnomer
    SlopMemory memory( options, new XShapeRectangle(glm::vec2(0,0), glm::vec2(0,0), options->borderSize, options->padding, glm::vec4( options->r, options->g, options->b, options->a ), options->highlight) );
    mouse = new Mouse( x11, options->nodecorations, ((XShapeRectangle*)memory.rectangle)->window );

    // We have no GL context, so the matrix is useless...
    glm::mat4 fake;
    // This is where we'll run through all of our stuffs
    while( memory.running ) {
        mouse->update();
        keyboard->update();
        // We move our statemachine forward.
        memory.update( 1 );

        // We don't actually draw anything, but the state machine uses
        // this to know when to spawn the window.
        memory.draw( fake );

        // X11 explodes if we update as fast as possible, here's a tiny sleep.
        XFlush(x11->display);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // Then we draw the framebuffer to the screen
        if ( (keyboard->anyKeyDown() && !options->nokeyboard) || mouse->getButton( 3 ) ) {
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
    delete mouse;
    // Finally return the data.
    return SlopSelection( output.x, output.y, output.z, output.w, memory.selectedWindow );
}

SlopSelection GLSlopSelect( SlopOptions* options, bool* cancelled, SlopWindow* window ) {
    mouse = new Mouse( x11, options->nodecorations, window->window );

    if ( options->shader != "textured" ) {
        window->framebuffer->setShader( options->shader );
    }

    // Init our little state machine, memory is a tad of a misnomer
    SlopMemory memory( options, new GLRectangle(glm::vec2(0,0), glm::vec2(0,0), options->borderSize, options->padding, glm::vec4( options->r, options->g, options->b, options->a ), options->highlight) );

    // This is where we'll run through all of our stuffs
    while( memory.running ) {
        mouse->update();
        keyboard->update();
        // We move our statemachine forward.
        memory.update( 1 );

        // Then we draw our junk to a framebuffer.
        window->framebuffer->bind();
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glClear (GL_COLOR_BUFFER_BIT);
        memory.draw( window->camera );
        window->framebuffer->unbind();

        // Then we draw the framebuffer to the screen
        window->framebuffer->draw();
        window->display();
        GLenum err = glGetError();
        if ( err != GL_NO_ERROR ) {
            throw err;
        }
        if ( (keyboard->anyKeyDown() && !options->nokeyboard) || mouse->getButton( 3 ) ) {
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
    delete mouse;
    // Finally return the data.
    return SlopSelection( output.x, output.y, output.z, output.w, memory.selectedWindow );
}
