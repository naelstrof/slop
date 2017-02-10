#include "glslop.hpp"

X11* x11;
Mouse* mouse;
Keyboard* keyboard;
Resource* resource;

// Defaults!
SlopOptions::SlopOptions() {
    borderSize = 1;
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

SlopSelection::SlopSelection( float x, float y, float w, float h, Window id ) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->id = id;
}

SlopMemory::SlopMemory( SlopOptions* options ) {
    running = true;
    state = (SlopState*)new SlopStart();
    nextState = NULL;
    tolerance = options->tolerance;
    nodecorations = options->nodecorations;
    rectangle = new Rectangle(glm::vec2(0,0), glm::vec2(0,0), options->borderSize, options->padding, glm::vec4( options->r, options->g, options->b, options->a ), options->highlight);
    selectedWindow = x11->root;
    state->onEnter( *this );
}

SlopMemory::~SlopMemory() {
    delete state;
    if ( nextState ) {
        delete nextState;
    }
    delete rectangle;
}

void SlopMemory::update( double dt ) {
    state->update( *this, dt );
    if ( nextState ) {
        state->onExit( *this );
        delete state;
        state = nextState;
        state->onEnter( *this );
        nextState = NULL;
    }
}

void SlopMemory::setState( SlopState* state ) {
    if ( nextState ) {
        delete nextState;
    }
    nextState = state;
}

void SlopMemory::draw( glm::mat4& matrix ) {
    state->draw( *this, matrix );
}

SlopSelection SlopSelect( SlopOptions* options, bool* cancelled ) {
    bool deleteOptions = false;
    if ( !options ) {
        deleteOptions = true;
        options = new SlopOptions();
    }
    resource = new Resource();
    // Set up x11 temporarily
    x11 = new X11(options->xdisplay);
    keyboard = new Keyboard( x11 );

    // Set up window with GL context
    SlopWindow* window = new SlopWindow();

    mouse = new Mouse( x11, options->nodecorations, window->window );

    if ( options->shader != "textured" ) {
        window->framebuffer->setShader( options->shader );
    }

    // Init our little state machine, memory is a tad of a misnomer
    SlopMemory memory( options );

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
        if ( keyboard->anyKeyDown() || mouse->getButton( 3 ) ) {
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
    delete x11;
    delete resource;
    if ( deleteOptions ) {
        delete options;
    }
    // Finally return the data.
    return SlopSelection( output.x, output.y, output.z, output.w, memory.selectedWindow );
}
