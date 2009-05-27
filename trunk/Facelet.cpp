#pragma once
#include "Facelet.h"

Facelet::Facelet() {
    
}

Facelet::Facelet(int xn, int yn, int zn, int fn) {
    fid = fn + (zn*6) + (yn*3*6) + (xn * 3 * 3 * 6);
}

void Facelet::setFace(double *f[4], int n) {
	for(int i = 0; i< 4; i++) {
        for(int j = 0; j < 3; j ++) {
            if(abs(j - 2) == n%3) {
                v[i][j] = f[i][j]*SIDE_LENGTH/2 + (f[i][j]*ROUND_WIDTH);
            } else {
                v[i][j] = f[i][j]*SIDE_LENGTH/2;
            }
            
        }
	}
}

void Facelet::draw() {
	// draw each face
	glColor3d(r, g, b);
	glLoadName(fid);
	glBegin(GL_QUADS);
		for(int n=0;n<4;++n)
			glVertex3d(v[n][0],v[n][1],v[n][2]);
	glEnd();
}

void Facelet::copy(Facelet *f) {
    r = f->r;
    g = f->g;
    b = f->b;
    colour = f->colour;
}