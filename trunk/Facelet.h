#pragma once

class Facelet {
    
public:
	// define the colour of the face
    Facelet(int xn, int yn, int zn, int fn);
    Facelet();
	double r, g, b;
    int fid;
	
    float v[4][3];
	
    int colour;
    int i, j, side;

    void setFace(double *face[4], int n);

    void draw();
    
    void copy(Facelet *f);
};