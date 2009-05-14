#pragma once

class Facelet {
public:
	// define the colour of the face
    Facelet();
	double r, g, b;
	
    float v[4][3];
	
    int colour;
    int i, j, side;

    void setFace(double *face[4], int n);

    void draw();
    
    void copy(Facelet *f);
};