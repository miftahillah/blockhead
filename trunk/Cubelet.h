#pragma once
#include "Facelet.h"

#define ROUND_WIDTH 0.06
#define SIDE_LENGTH 0.88
//#define ROUND_WIDTH 0.18
//#define SIDE_LENGTH 2.64
#define ROUND_STEPS 2
#define CUBE_SIZE 0.5
#define FACE_SIZE 1.1

double DFL[3] = { -1,  1, -1};
double DFR[3] = {  1,  1, -1};
double DBL[3] = { -1, -1, -1};
double DBR[3] = {  1, -1, -1};
double UFL[3] = { -1,  1,  1};
double UFR[3] = {  1,  1,  1};
double UBL[3] = { -1, -1,  1};
double UBR[3] = {  1, -1,  1};

double *BASE[4] = {DFL, DFR, DBR, DBL};
double *LEFT[4] = {DFL, DBL, UBL, UFL};
double *FRONT[4] = {DFL, DFR, UFR, UFL};
double *RIGHT[4] = {DFR, DBR, UBR, UFR};
double *TOP[4] = {UFL, UFR, UBR, UBL};
double *REAR[4] = {UBL, UBR, DBR, DBL};

// 27 Individual cubelets make up the entire cube
class Cubelet {
public:
    Cubelet(int xn, int yn, int zn);
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