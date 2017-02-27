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

static int TmpXError(Display * d, XErrorEvent * ev) {
    return 0;
}

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
    shaders.push_back("textured");
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
    if ( !options->nokeyboard ) {
        XErrorHandler ph = XSetErrorHandler(slop::TmpXError);
        keyboard = new Keyboard( x11 );
        XSetErrorHandler(ph);
    }
    bool success = false;
    std::string errorstring = "";
    SlopWindow* window;
    // First we check if we have a compositor available
    if ( x11->hasCompositor() && !options->noopengl ) {
        // If we have a compositor, we try using OpenGL
        try {
            window = new SlopWindow();
            success = true;
        } catch( std::exception* e ) {
            errorstring += std::string(e->what()) + "\n";
            success = false;
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
        if ( !options->nokeyboard ) {
            slop::keyboard->update();
        }
        // We move our statemachine forward.
        memory.update( 1 );

        // We don't actually draw anything, but the state machine uses
        // this to know when to spawn the window.
        memory.draw( fake );

        // X11 explodes if we update as fast as possible, here's a tiny sleep.
        XFlush(x11->display);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // Then we draw the framebuffer to the screen
        if ( (!options->nokeyboard && slop::keyboard->anyKeyDown()) || slop::mouse->getButton( 3 ) ) {
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

    std::string vert = "#version 130\nattribute vec2 position;\nattribute vec2 uv;\nvarying vec2 uvCoord;\nvoid main()\n{\nuvCoord = uv;\ngl_Position = vec4(position,0,1);\n}\n";
    std::string frag = "#version 130\n uniform sampler2D texture;\n varying vec2 uvCoord;\n out vec4 outColor;\n void main()\n {\n outColor = texture2D( texture, uvCoord );\n }\n";
    slop::Shader* textured = new slop::Shader( vert, frag, false );
    std::vector<slop::Shader*> shaders;
    for( int i=0;i<options->shaders.size();i++ ) {
        std::string sn = options->shaders[i];
        if ( sn != "textured" ) {
            shaders.push_back( new slop::Shader( sn + ".vert", sn + ".frag" ) );
        } else {
            shaders.push_back( textured );
        }
    }
    // Init our little state machine, memory is a tad of a misnomer
    slop::SlopMemory memory( options, new GLRectangle(glm::vec2(0,0), glm::vec2(0,0), options->borderSize, options->padding, glm::vec4( options->r, options->g, options->b, options->a ), options->highlight) );

    slop::Framebuffer* pingpong;
    if ( shaders.size() > 1 ) {
        pingpong = new slop::Framebuffer(WidthOfScreen(x11->screen), HeightOfScreen(x11->screen));
    }

    // This is where we'll run through all of our stuffs
    auto start = std::chrono::high_resolution_clock::now();
    while( memory.running ) {
        slop::mouse->update();
        if ( !options->nokeyboard ) {
            slop::keyboard->update();
        }
        // We move our statemachine forward.
        memory.update( 1 );

        // Then we draw our junk to a framebuffer.
        window->framebuffer->setShader( textured );
        window->framebuffer->bind();
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glClear (GL_COLOR_BUFFER_BIT);
        memory.draw( window->camera );
        window->framebuffer->unbind();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end-start;
        if ( shaders.size() > 1 ) {
            int i;
            // We have our clean buffer, now to slather it with some juicy shader chains.
            for (i=0;i<=(int)shaders.size()-2;i+=2) {
                pingpong->bind();
                glClearColor (0.0, 0.0, 0.0, 0.0);
                glClear (GL_COLOR_BUFFER_BIT);
                window->framebuffer->setShader( shaders[i] );
                window->framebuffer->draw(slop::mouse->getMousePos(), elapsed.count()/1000.f, glm::vec4( options->r, options->g, options->b, options->a ) );
                pingpong->unbind();

                window->framebuffer->bind();
                glClearColor (0.0, 0.0, 0.0, 0.0);
                glClear (GL_COLOR_BUFFER_BIT);
                pingpong->setShader( shaders[i+1] );
                pingpong->draw(slop::mouse->getMousePos(), elapsed.count()/1000.f, glm::vec4( options->r, options->g, options->b, options->a ) );
                window->framebuffer->unbind();
            }
            for (;i<shaders.size();i++) {
                pingpong->bind();
                glClearColor (0.0, 0.0, 0.0, 0.0);
                glClear (GL_COLOR_BUFFER_BIT);
                window->framebuffer->setShader( shaders[i] );
                window->framebuffer->draw(slop::mouse->getMousePos(), elapsed.count()/1000.f, glm::vec4( options->r, options->g, options->b, options->a ) );
                pingpong->unbind();
            }
            if ( i%2 != 0 ) {
                window->framebuffer->draw(slop::mouse->getMousePos(), elapsed.count()/1000.f, glm::vec4( options->r, options->g, options->b, options->a ) );
            } else {
                pingpong->draw(slop::mouse->getMousePos(), elapsed.count()/1000.f, glm::vec4( options->r, options->g, options->b, options->a ) );
            }
        } else {
            window->framebuffer->setShader( shaders[0] );
            window->framebuffer->draw(slop::mouse->getMousePos(), elapsed.count()/1000.f, glm::vec4( options->r, options->g, options->b, options->a ) );
        }

        window->display();
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
            throw new std::runtime_error( "OpenGL threw an error: " + error );
        }
        if ( (!options->nokeyboard && slop::keyboard->anyKeyDown()) || slop::mouse->getButton( 3 ) ) {
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
