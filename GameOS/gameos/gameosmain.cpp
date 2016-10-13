#include "gameos.hpp"
#include "gos_render.h"
#include <stdio.h>

// TODO:remove
#include <SDL2/SDL.h>
#include "utils/camera.h"
#include "utils/shader_builder.h"
#include "utils/gl_utils.h"

extern void gos_CreateRenderer(int w, int h);

static bool g_exit = false;
static camera g_camera;
static glsl_program* g_myprogram = NULL;

static void handle_key_down( SDL_Keysym* keysym ) {
    switch( keysym->sym ) {
        case SDLK_ESCAPE:
            g_exit = true;
            break;
    }
}

static void process_events( void ) {
    SDL_Event event;
    while( SDL_PollEvent( &event ) ) {
		
        switch( event.type ) {
        case SDL_KEYDOWN:
            handle_key_down( &event.key.keysym );
            break;
        case SDL_QUIT:
            g_exit = true;
            break;
		case SDL_WINDOWEVENT_RESIZED:
			{
				glViewport(0, 0, (GLsizei) event.window.data1, (GLsizei) event.window.data2);
				float w = (float)event.window.data1;
				float h = (float)event.window.data2;
                SPEW(("INPUT", "resize event: w: %f h:%f\n", w, h));
			}
			break;
        }
    }
}

static void draw_screen( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //CHECK_GL_ERROR;
    
    const int w = Environment.screenWidth;
    const int h = Environment.screenHeight;

    Environment.UpdateRenderers();

    mat4 proj;
    g_camera.get_projection(&proj);
    mat4 viewM;
    g_camera.get_view(&viewM);

    float aspect = (float)w/(float)h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadTransposeMatrixf((const float*)proj);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadTransposeMatrixf((const float*)viewM);

	mat4 viewproj = proj*viewM;
	g_myprogram->setMat4("ModelViewProjectionMatrix", (const float*)viewproj);
    draw_textured_cube(0);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glUseProgram(0);
    //CHECK_GL_ERROR;
}

int main(int argc, char** argv)
{
    // fills in Environment structure
    GetGameOSEnvironment("");

    int w = Environment.screenWidth;
    int h = Environment.screenHeight;

    graphics::RenderWindowHandle win = graphics::create_window("mc2", w, h);
    if(!win)
        return 1;

    graphics::RenderContextHandle ctx = graphics::init_render_context(win);
    if(!ctx)
        return 1;

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        SPEW(("GLEW", "Error: %s\n", glewGetErrorString(err)));
        return 1;
    }

    SPEW(("GRAPHICS", "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION)));
    if (!GLEW_ARB_vertex_program || !GLEW_ARB_vertex_program)
    {
        SPEW(("GRAPHICS", "No shader program support\n"));
        return 1;
    }

    graphics::make_current_context(ctx, win);

	g_myprogram = glsl_program::makeProgram("object_tex", "data/shaders/object_tex.vert", "data/shaders/object_tex.frag");
    if(!g_myprogram) {
		SPEW(("SHADERS", "Failed to create object_tex material\n"));
        return 1;
	}

    gos_CreateRenderer(w, h);

    Environment.InitializeGameEngine();

	float aspect = (float)w/(float)h;
	mat4 proj_mat = frustumProjMatrix(-aspect*0.5f, aspect*0.5f, -.5f, .5f, 1.0f, 100.0f);
	g_camera.set_projection(proj_mat);
	g_camera.set_view(mat4::translation(vec3(0, 0, -16)));

    while( !g_exit ) {

        Environment.DoGameLogic();

        process_events();

        // TODO: add window as context member, to not pass 2 parameters
        graphics::make_current_context(ctx, win);
        draw_screen();
        graphics::swap_window(win);
    }
    
    Environment.TerminateGameEngine();

    graphics::destroy_render_context(ctx);
    graphics::destroy_window(win);

    return 0;
}

