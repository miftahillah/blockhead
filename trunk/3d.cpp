#include "3d.h"

void InitGraphics()
{
    const int width = 800, height = 600;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) exit(1);

    // Window Settings
    SDL_WM_SetCaption("Rubiks Cube", "Rubiks Cube");

    // Get Video Info
    const SDL_VideoInfo *video = SDL_GetVideoInfo();
    if (!video) { SDL_Quit(); exit(2); }

    // Create Video Surface
    SDL_Surface *screen = SDL_SetVideoMode(width, height, video->vfmt->BitsPerPixel, SDL_OPENGL);
    if (!screen) { SDL_Quit(); exit(3); }

    // Size OpenGL to Video Surface
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.33, 1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

    // Set Pixel Format
    //SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    //SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    //SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // OpenGL Render Settings
    //glClearColor(0, 0, 0, 1);
    //glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
}

// Handle events
bool Events()
{
    SDL_Event event;
	int xrel, yrel;
	
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
			case SDL_KEYDOWN:
				// Exit when ESC is pressed
				if (event.key.keysym.sym == SDLK_ESCAPE) return false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					// left mouse button
					left_mouse = true;
				} 
				
				if(event.button.button == SDL_BUTTON_RIGHT) {
					// right mouse button
					right_mouse = true;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT) {
					// left mouse button
					left_mouse = false;
				} 
				if(event.button.button == SDL_BUTTON_RIGHT) {
					// right mouse button
					right_mouse = false;
				}
				break;
			case SDL_MOUSEMOTION:
				xrel = event.motion.xrel;
				yrel = event.motion.yrel;

				if( right_mouse && !left_mouse) {
					// update perspective
					// theta represents angle in vertical direction
					// phi is horizontal rotation
					theta = theta + yrel / 100.0;
					phi = phi - xrel/50.0;

					// don't allow the cube to tilt too far
					if(theta<-1.57) theta=-1.57;
					if(theta>1.57) theta=1.57;
				}
				break;
			case SDL_QUIT:
				return false;
			}
    }
    return true;
}