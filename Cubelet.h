#pragma once
#include "Facelet.h"
#include "rubiks.h"

#define ROUND_WIDTH 0.06
#define SIDE_LENGTH 0.88
//#define ROUND_WIDTH 0.18
//#define SIDE_LENGTH 2.64
#define ROUND_STEPS 2
#define CUBE_SIZE 0.5
#define FACE_SIZE 1.1

// 27 Individual cubelets make up the entire cube
class Cubelet {
public:
    Cubelet();
    // each cubelet has 6 faces
    void draw();
    void init();
    
    void MakeRoundedSide();
    void MakeCornerSphere();
    
    void set_face(int n, int colour);
    char* description(int f1, int f2, int f3);
    char* description(int f1, int f2);
    Facelet* face[6];
    void SwapFaces(int f1, int f2, int f3, int f4);
    void RCCW(int a);
    void RCW(int a);
private:
    
};