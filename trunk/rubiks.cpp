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
    	CubeFinder finder;
    	cvNamedWindow("Rubiks", 1);
    cvShowImage("Rubiks", finder.read_frame(cvLoadImage("cube3.jpg", 1)));
    cvWaitKey(0);
    //CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
    return 0;
    /*
    CubeFinder finder;
    cvNamedWindow( "Rubiks", 1 );
    
    cvWaitKey(0);
    //cvShowImage("Rubiks", finder.read_frame(cvLoadImage("test/pair2b.jpg", 1)));
    //cvWaitKey(0);
*/
	printf("Starting...\n");
	RubiksCube cube;
	


	bool running = true;
	
    CvCapture* capture = cvCaptureFromCAM( 0 );
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 640 );
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 480 );
	// read in the cube state
	
	while(running) {
		// Get one frame
        IplImage* cframe = cvQueryFrame( capture );
        
        if( !cframe ) {
            fprintf( stderr, "ERROR: frame is null...\n" );
            getchar();
            break;
        }

		cvShowImage( "Rubiks", finder.read_frame(cframe) );


        if( (cvWaitKey(10) & 255) == 27 ) running = false;
		
	}
	return 0;
	
	/*
	
	
	RubiksCube cube;
	Side s1, s2, s3, s4, s5, s6;
	// enum Colours { RED, YELLOW, GREEN, ORANGE, WHITE, BLUE};
	int f1[3][3] = {
		{1, 5, 0},
		{5, 1, 4},
		{4, 3, 1}
	};
	int f2[3][3]= {
		{2, 2, 5},
		{0, 5, 4},
		{3, 0, 4}
	};
	int f3[3][3] ={
		{4, 0, 1},
		{2, 4, 2},
		{2, 3, 4}
	};
	int f4[3][3] ={
        {5, 4, 5},
        {1, 2, 3},
        {3, 5, 2}
	};
	int f5[3][3] ={
		{0, 0, 1},
		{5, 0, 4},
		{0, 3, 3}
	};
	int f6[3][3] ={
		{5, 1, 0},
		{2, 3, 1},
		{3, 1, 2}
	};

	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			s1.face[i][j] = f1[i][j];
			s2.face[i][j] = f2[i][j];
			s3.face[i][j] = f3[i][j];
			s4.face[i][j] = f4[i][j];
			s5.face[i][j] = f5[i][j];
			s6.face[i][j] = f6[i][j];
		}
	}
    
	cube.read_side(&s1);
	cube.read_side(&s2);
	cube.read_side(&s3);
	cube.read_side(&s4);
	cube.read_side(&s5);
	cube.read_side(&s6);
	
	cube.update();
	cube.align_sides();
	
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
    */
    return 0;

}
