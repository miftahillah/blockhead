#include "cv.h"
#include "highgui.h"
#include <math.h>
#include <stdio.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>


#include "rubiks.h"
#include "RubiksCube.h"
#include "Side.h"
#include "Cubelet.h"
#include "Facelet.h"
#include "CubeFinder.h"



int main(int argc, char** argv)
{
    double phi;
    double theta;
    
    bool left_mouse = false;
    bool right_mouse = false;
    
    RubiksCube cube;
    srand((unsigned)time(NULL));
    
    cube.set_solved();
    cube.update();
    cube.solve();
    
    //cube.solve();
    //cube.xRot[0] = 45;
    //cube.RotateSide(RED, 30);
    //cube.RotateSide(GREEN, 45);
    InitGraphics();
    // Event loop
    
    phi = 10;
    theta = 0.4;
    
    bool solving = false;
    bool running = true;
    bool demo = false;
    bool scrambling = false;
    bool scramble_next = true;
    bool panning = true;
    bool capturing = false;
    bool capturing_demo = false;
    bool just_existed = false;
    int scram = 0, pan = 0;
    
    bool initialised = true;
    
    CubeFinder f;
    
    CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
    
    while (running)
    {
        
        SDL_Event event;
        int xrel, yrel;
        glClearColor(0.8f, 0.8f, 0.8f,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(7*sin(phi)*cos(theta),-7*cos(phi)*cos(theta),7*sin(theta),0,0,0,0,0,1);
        
        if(demo && panning) {
            pan++;
            phi += M_PI*2 / 1000;
            theta = 0 + 0.5 * sin(pan * M_PI*2 / 800);
            if(pan > 1000) {
                panning = false;
                if(scramble_next) {
                    scrambling = true;
                    scramble_next = false;
                } else {
                    // solve next
                    cube.solve();
                    solving = true;
                }
                pan = 0;
            }
        }
        
        if(demo && scrambling) {
            // make a random move
            if(!cube.Moving()) {
                cube.ShowMove(a_twist_string[(int)rand()%18], 0.2);
                scram++;
                if(scram > 50) {
                    scrambling = false;
                    panning = true;
                    scram = 0;
                }
            }
        }
        
        //cube.ShowNextMove();
        cube.UpdatePositions();
        cube.draw();
        // Show the frame
        SDL_GL_SwapBuffers();
        if(solving){
            cube.ShowNextMove();
            if(cube.FinishedSolve()) {
                solving = false;
                panning = true;
                scramble_next = true;
            }
        }
        
        if(capturing) {
            // show the capture process
            IplImage* cframe;
            
            while(1) {
                cframe = cvQueryFrame( capture );
                cvShowImage("Rubiks", cframe);
                if( (cvWaitKey(10) & 255) == 27 ) break;
            } 
            CvSeq* sides = f.read_frame(cframe);
            
            printf("Found %d sides\n", sides->total);
            
            for(int i = 0; i < sides->total; i++) {
                Side* s = (Side*)cvGetSeqElem(sides, i);
                cube.read_side(s);
            }
            cube.update();
            capturing = false;
        }
        
        if(capturing_demo) {
            // show the capture process
            IplImage* cframe;
            
            while(1) {
                cframe = cvQueryFrame( capture );
                CvSeq* sides = f.read_frame(cframe);
                if( (cvWaitKey(10) & 255) == 27 ) break;
            } 
        }
        
        
        
        
        
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    // Exit when ESC is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_RIGHT) solving = true;
                    if(event.key.keysym.sym == SDLK_d) {
                        if(demo) {
                            demo = false;
                        } else {
                            demo = true;
                        }
                    }
                    
                    if(event.key.keysym.sym == SDLK_i) {
                        if(capturing) {
                            capturing = false;
                            cvDestroyWindow("Rubiks");
                        } else {
                            capturing = true;
                            cvNamedWindow("Rubiks", 1);
                        }
                    }
                    
                    if(event.key.keysym.sym == SDLK_o) {
                        if(capturing_demo) {
                            capturing_demo = false;
                            cvDestroyWindow("Rubiks");
                        } else {
                            capturing_demo = true;
                            cvNamedWindow("Rubiks", 1);
                        }
                    }
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
                    
                    if(left_mouse) {
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

