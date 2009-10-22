#include "Cubelet.h"
#include "Facelet.h"
#include "SDL_opengl.h"

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

char DEF[6] ={ 'U', 'F', 'L', 'D', 'B', 'R'};

//#include "3d.h"
// each point corresponds to a corner
// each point corresponds to a corner

Cubelet::Cubelet() {
    for(int i = 0; i < 6; i++) {
        face[i] = new Facelet();
    }
}

void Cubelet::SwapFaces(int f1, int f2, int f3, int f4) {
    Facelet *f = new Facelet();
    f->copy(face[f1]);
    face[f1]->copy(face[f2]);
    face[f2]->copy(face[f3]);
    face[f3]->copy(face[f4]);
    face[f4]->copy(f);
}

void Cubelet::RCCW(int a) {
    if(a == 0) {
        // rotate about x
        SwapFaces(WHITE, RED, YELLOW, ORANGE);
    } else if (a == 1) {
        // rotate about y
        SwapFaces(RED, BLUE, ORANGE, GREEN);
    } else {
        // rotate about z
        SwapFaces(GREEN, YELLOW, BLUE, WHITE);
    }
}

void Cubelet::RCW(int a) {
    if(a == 0) {
        // rotate about x
        SwapFaces(ORANGE, YELLOW, RED, WHITE);
    } else if (a == 1) {
        // rotate about y
        SwapFaces(GREEN, ORANGE, BLUE, RED);
    } else {
        // rotate about z
        SwapFaces(WHITE, BLUE, YELLOW, GREEN);
    }
}

void Cubelet::init() {
    // define geometry
    // Red
	//printf("Initiating cubelet with side %d=%d\n", TOP, GREEN);
	face[RED]->setFace(TOP, RED);
	// Green
	face[GREEN]->setFace(LEFT, GREEN);
	// white
	face[WHITE]->setFace(FRONT, WHITE);
	// Blue
	face[BLUE]->setFace(RIGHT, BLUE);
	// yellow
	face[YELLOW]->setFace(REAR, YELLOW);
	// orange
	face[ORANGE]->setFace(BASE, ORANGE);
	
    // set face colours to grey
    for(int i = 0; i < 6; i++) {
        set_face(i, -1);
    }
}

void Cubelet::set_face(int n, int colour) {
    double r=0, g=0, b=0;
    switch(colour) {
        case RED:
            // top
            r=0.8;
            break;
        case BLUE:
            // left
            b = 0.9;
            r = 0.1;
            g = 0.1;
            break;
        case WHITE:
            // front
            r=g=b=1;
            break;
        case GREEN:
            // right
            g = 0.6;
            //r = 0.2;
            break;
        case YELLOW:
            // back
            r = g = 0.9;
            b=0.1;
            break;
        case ORANGE:
            // base
            r = 1; g = 0.4;
            break;
        default:
            // unknown
            r = g = b = 0;
            break;
    }
    face[n]->r = r;
    face[n]->g = g;
    face[n]->b = b;
    face[n]->colour = colour;
}

void Cubelet::draw() {
	
	for(int n=0;n<6;++n)
    	face[n]->draw();
	// now draw the bevels
	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, m_BevelDiffuse);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, m_BevelMaterial);
	//glMaterialfv(GL_FRONT, GL_SHININESS, m_BevelShininess);
	glColor3f(0.2, 0.2, 0.2);
	glPushMatrix();
	// Upper, right, back Corner
	glTranslatef(SIDE_LENGTH/2, SIDE_LENGTH/2, SIDE_LENGTH/2);
	MakeCornerSphere();
	
	// Upper, left, back Corner
	glTranslatef(-SIDE_LENGTH, 0, 0);
	glRotatef(90, 0, 0, 1);
	MakeCornerSphere();
	glRotatef(-90, 0, 0, 1);
	
	// Lower, left, back Corner
	glTranslatef(0, -SIDE_LENGTH, 0);
	glRotatef(180, 0, 0, 1);
	MakeCornerSphere();
	glRotatef(-180, 0, 0, 1);
	
	// Lower, right, back Corner
	glTranslatef(SIDE_LENGTH, 0, 0);
	glRotatef(270, 0, 0, 1);
	MakeCornerSphere();
	glRotatef(-270, 0, 0, 1);
	
	// Upper, right, front Corner
	glTranslatef(0, SIDE_LENGTH, -SIDE_LENGTH);
	glRotatef(90, 0, 1, 0);
	MakeCornerSphere();
	glRotatef(-90, 0, 1, 0);
	
	
	// Upper, left, front Corner
	glTranslatef(-SIDE_LENGTH, 0, 0);
	glRotatef(180, 0, 1, 0);
	MakeCornerSphere();
	glRotatef(-180, 0, 1, 0);
	
	// Lower, left, front Corner
	glTranslatef(0, -SIDE_LENGTH, 0);
	glRotatef(180, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	MakeCornerSphere();
	glRotatef(-90, 1, 0, 0);
	glRotatef(-180, 0, 1, 0);
	
	// Lower, right, front Corner
	glTranslatef(SIDE_LENGTH, 0, 0);
	glRotatef(180, 1, 0, 0);
	MakeCornerSphere();
	glRotatef(-180, 1, 0, 0);
	
	glTranslatef(0, SIDE_LENGTH, SIDE_LENGTH/2);
	
	MakeRoundedSide();
	
	glTranslatef(-SIDE_LENGTH, 0, 0);
	glRotatef(90, 0, 0, 1);
	MakeRoundedSide();
	glRotatef(-90, 0, 0, 1);
	
	glTranslatef(0, -SIDE_LENGTH, 0);
	glRotatef(180, 0, 0, 1);
	MakeRoundedSide();
	glRotatef(-180, 0, 0, 1);
	
	glTranslatef(SIDE_LENGTH, 0, 0);
	glRotatef(270, 0, 0, 1);
	MakeRoundedSide();
	glRotatef(-270, 0, 0, 1);
	
	glTranslatef(0, SIDE_LENGTH/2, SIDE_LENGTH/2);
	glRotatef(90, 1, 0, 0);
	MakeRoundedSide();
	
	glTranslatef(-SIDE_LENGTH, 0, 0);
	glRotatef(90, 0, 0, 1);
	MakeRoundedSide();
	
	glTranslatef(-SIDE_LENGTH, 0, 0);
	glRotatef(90, 0, 0, 1);
	MakeRoundedSide();
	
	glTranslatef(-SIDE_LENGTH, 0, 0);
	glRotatef(90, 0, 0, 1);
	MakeRoundedSide();
	
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0, SIDE_LENGTH/2, -SIDE_LENGTH/2);
	
	MakeRoundedSide();
	
	glTranslatef(-SIDE_LENGTH, 0, 0);
	glRotatef(90, 0, 0, 1);
	MakeRoundedSide();
	
	glTranslatef(-SIDE_LENGTH, 0, 0);
	glRotatef(90, 0, 0, 1);
	MakeRoundedSide();
	
	glTranslatef(-SIDE_LENGTH, 0, 0);
	glRotatef(90, 0, 0, 1);
	MakeRoundedSide();
	
	glPopMatrix();
}

void Cubelet::MakeRoundedSide() 
{
	float StepSize = M_PI/2/ROUND_STEPS;
	float v, vv;
	
	glBegin(GL_QUADS);
	glColor3f(0, 0, 0);
	
	for (int i = 0; i < ROUND_STEPS; i++) 
	{
		v = i * StepSize;
		vv = (i+1)*StepSize;
		
		glNormal3f((float)cos(v),(float)sin(v), 0);
		glVertex3f((float)cos(v) * ROUND_WIDTH, (float)sin(v) * ROUND_WIDTH,  SIDE_LENGTH/2);
		glVertex3f((float)cos(v) * ROUND_WIDTH, (float)sin(v) * ROUND_WIDTH, -SIDE_LENGTH/2);
		
		glNormal3f((float)cos(vv), (float)sin(vv), 0);
		glVertex3f((float)cos(vv) * ROUND_WIDTH, (float)sin(vv) * ROUND_WIDTH, -SIDE_LENGTH/2);
		glVertex3f((float)cos(vv) * ROUND_WIDTH, (float)sin(vv) * ROUND_WIDTH,  SIDE_LENGTH/2);
	}
	
	glEnd();
}

void Cubelet::MakeCornerSphere() 
{
	float StepSize = M_PI/2/ROUND_STEPS;
	float v, w, vv, ww;
	
	glBegin(GL_QUADS);
	glColor3f(0, 0, 0);
	
	for (int i = 0; i < ROUND_STEPS; i++) 
	{
		v = i * StepSize;
		vv = (i+1)*StepSize;
		
		for (int j = 0; j < ROUND_STEPS; j++) 
		{
			w = j * StepSize;
			ww = (j+1) * StepSize;
			
			glNormal3f((float)cos(v) * (float)cos(w), (float)cos(v) * (float)sin(w),(float) sin(v));
			glVertex3f((float)cos(v) * (float)cos(w) * ROUND_WIDTH,
					   (float)cos(v) * (float)sin(w) * ROUND_WIDTH,
					   (float)sin(v) * ROUND_WIDTH);
			
			glNormal3f((float)cos(vv) * (float)cos(w), (float)cos(vv) * (float)sin(w), (float)sin(vv));
			glVertex3f((float)cos(vv) * (float)cos(w) * ROUND_WIDTH,
					   (float)cos(vv) * (float)sin(w) * ROUND_WIDTH,
					   (float)sin(vv) * ROUND_WIDTH);
			
			glNormal3f((float)cos(vv) * (float)cos(ww), (float)cos(vv) * (float)sin(ww), (float)sin(vv));
			glVertex3f((float)cos(vv) * (float)cos(ww) * ROUND_WIDTH,
					   (float)cos(vv) * (float)sin(ww) * ROUND_WIDTH,
					   (float)sin(vv) * ROUND_WIDTH);
			
			glNormal3f((float)cos(v) * (float)cos(ww), (float)cos(v) * (float)sin(ww), (float)sin(v));
			glVertex3f((float)cos(v) * (float)cos(ww) * ROUND_WIDTH,
					   (float)cos(v) * (float)sin(ww) * ROUND_WIDTH,
					   (float)sin(v) * ROUND_WIDTH);
		}
	}
	glEnd();
}

bool desc_comp(int *items, int i, char* p) {
    for(int j = 0; j < i; j++) {
        if(DEF[items[j]] != p[j]) {
            return false;
        }
    }
    return true;
}

bool desc_type(int* items, int i, char* p) {
    // generate each combination of p
    for(int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
            for(int z = 0; z < 3; z++) {
                if(x != y && x != z && y != z) {
                    if(i == 2) {
                        char* st = new char[3];
                        st[0] = p[x];
                        st[1] = p[y];
                        if(desc_comp(items, i, st)) {
                            return true;
                        }
                    } else {
                        char* st = new char[4];
                        st[0] = p[x];
                        st[1] = p[y];
                        st[2] = p[z];
                        if(desc_comp(items, i, st)) {
                            return true;
                        }
                    }
					
                }
            }
        }
    }
    return false;
}

char* Cubelet::description(int f1, int f2, int f3) {
    // find the colours on this cubelet
    // these need to be in a very specific sequence in order to be understood by the solving algorithm
    char *desc = new char[4];
    desc[0] = DEF[f1];
    desc[1] = DEF[f2];
    desc[2] = DEF[f3];
    desc[3] = '\0';
    
    return desc;
}

char* Cubelet::description(int f1, int f2) {
    // find the colours on this cubelet
    // these need to be in a very specific sequence in order to be understood by the solving algorithm
    char *desc = new char[3];
    desc[0] = DEF[f1];
    desc[1] = DEF[f2];
    desc[2] = '\0';
    
    return desc;
}

