#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include "3d.cpp"
#include "rubiks.h"
#include "colours.cpp"
#include "CubeFinder.cpp"
#include "RubiksCube.cpp"
#include <unistd.h>

int main(int argc, char** argv)
{
    RubiksCube cube;
    srand((unsigned)time(NULL));
    
	// create a solved cube
    for(int i = 0; i < 6; i++) {
        Side s;
        for(int x = 0; x < 3; x++) {
            for(int y = 0; y < 3; y++) {
                s.face[x][y] = i;
            }
        }
        cube.read_side(&s);
    }
    cube.update();

    // scramble the cube
    for(int d = 0; d < 100; d++) {
        cube.RotateSide((int)rand()%6, 90);
    }
    printf("%s\n",cube.get_notation());
    
    //cube.solve();
    //cube.xRot[0] = 45;
    //cube.RotateSide(RED, 30);
    //cube.RotateSide(GREEN, 45);
	InitGraphics();
	// Event loop

    phi = 10;
    theta = 0.4;
    
    cube.solve();

	bool solving = false;
	bool running = true;
	while (running)
    {
	    SDL_Event event;
		int xrel, yrel;
		glClearColor(1, 1, 1,0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glLoadIdentity();	
		gluLookAt(7*sin(phi)*cos(theta),-7*cos(phi)*cos(theta),7*sin(theta),0,0,0,0,0,1);
        //cube.ShowNextMove();
        cube.UpdatePositions();
		cube.draw();
		// Show the frame
		SDL_GL_SwapBuffers();
		
		if(solving)
			cube.ShowNextMove();

	    while (SDL_PollEvent(&event))
	    {
	        switch (event.type)
	        {
				case SDL_KEYDOWN:
					// Exit when ESC is pressed
					if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
					break;
				case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_RIGHT) solving = true;;
					//if (event.key.keysym.sym == SDLK_LEFT) cube.StepBack();
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
					running = false;
			}
	    }     
    }

    
    
    SDL_Quit();

    return 0;

}
