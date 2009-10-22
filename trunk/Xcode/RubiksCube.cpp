#include "SDL_opengl.h"
#include "Solver.h"
#include "RubiksCube.h"
#include "Facelet.h"
#include "Cubelet.h"
#include "Side.h"
#include <unistd.h>

#include <sys/time.h>

char DEF2[6] ={ 'U', 'F', 'L', 'D', 'B', 'R'};

void RubiksCube::align_sides() {
    // recursively evaluate the validity of each face orientation
    update();
    
    int valid = valid_state(0);
    if(valid == 1) {
		
    } else {
        //
    }
    printf("there are %d valid combinations of sides\n", valid);
    if(valid > 0) {
        //printf("setting to a valid combination\n");
        for(int i = 0; i < 6; i++) {
            for(int j = 0; j < 3; j++) {
                for(int k = 0; k < 3; k++) {
                    side[i]->face[j][k] = side[i]->valid_face[j][k];
                }
            }
        }
        update();
    }
    
}

bool RubiksCube::Moving() {
    return m_moving;
}

int RubiksCube::valid_state(int s) {
    int valid = 0;
    // find the next initialised side
    int ns = -1;
	int j;
    for(j = s+1; j < 6; j++) {
        if(side[j]->has_data) {
            ns = j;
            break;
        }
    }
    
    for(int i = 0; i < 4; i++) {
        //printf("Rotating %d\n", ns);
        // keep recursing
        if(ns >= 0) {
            valid += valid_state(ns);
        } else {
            valid += valid_state();
        }
        side[s]->rotate();
        for(int j = 0; j < s; j++) {
            //printf("\t");
        }
        //printf("%d\n", s);
        update();
    }
    
    return valid;
}

// assess if this state is valid or not
// return 1 if valid
// 0 if not
int RubiksCube::valid_state() {
    // go through each cubelet
    // check the combinations of colours
    //printf("\n");
    bool CCorner[6][6][6];
    bool CEdge[6][6];
    for(int x = 0; x < 6; x++) {
        for(int y = 0; y < 6; y++) {
            for(int z = 0; z < 6; z++) {
                CCorner[x][y][z] = false;
            }
            CEdge[x][y] = false;
        }
    }
    
    for(int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
            for(int z = 0; z < 3; z++) {
                if(x != 1 || y != 1 || z != 1) {
                    int colours[3];
                    int i = 0;
                    // get the set colours
                    //printf("\n");
                    for(int j = 0; j < 6; j++) {
                        if(cubes[x][y][z]->face[j]->colour >= 0) {
                            colours[i] = cubes[x][y][z]->face[j]->colour;
                            //printf("c[%d] = %d\n", i, colours[i]);
                            i++;
                        }
                    }
                    // colours aren't in order!
                    if(i == 3) {
                        // check to see if this corner already exists
                        int i1 = colours[0], i2 = colours[1], i3 = colours[2];
                        int tmp;
                        if(i3 < i2) {
                            tmp = i2;
                            i2 = i3;
                            i3 = tmp;
                        }                        
                        if(i2 < i1) {
                            tmp = i1;
                            i1 = i2;
                            i2 = tmp;
                        }
                        if(i3 < i2) {
                            tmp = i2;
                            i2 = i3;
                            i3 = tmp;
                        }
                        
                        //printf("%d %d %d\n\n", i1, i2, i3);
                        
                        if(CCorner[i1][i2][i3]) {
                            // already been found!
                            return 0;
                        }
                        //printf("");
                        CCorner[i1][i2][i3] = true;
                    }
                    
                    if(i == 2) {
                        int i1 = colours[0], i2 = colours[1];
                        if(i2 < i1) {
                            int tmp = i1;
                            i1 = i2;
                            i2 = tmp;
                        }
                        if(CEdge[i1][i2]) {
                            return 0;
                        }
                        CEdge[i1][i2] = true;
                    }
                    //printf("cube at [%d, %d, %d] has %d colours\n", x, y, z, i);
                    if(i > 0) {
                        // if any of the colours are 3 apart then the cubelet is invalid
                        for(int k = 0; k < i-1; k++) {
                            for(int j = k+1; j < i; j++) {
                                if(abs(colours[k] - colours[j]) == 3 || colours[j] == colours[k]) {
                                    //printf("\tinvalid (%d, %d)\n", colours[k], colours[k]);
                                    return 0;
                                }
                            }
                        }
                    }
                    //printf("\tpassed\n");
                    // see if we know the bordering colours
                    // and if so, are they correct?
                    for(int j = 0; j < 6; j++) {
                        if(cubes[x][y][z]->face[j]->colour >= 0) {
                            int r = cubes[x][y][z]->face[j]->i;
                            int c = cubes[x][y][z]->face[j]->j;
                            int s = cubes[x][y][z]->face[j]->side;
                            //printf("c r s= %d %d %d\n", c, r, s);
                            if(r == 0) {
                                int col = side[s]->border[0][c];
                                if(col != -1){
                                    bool found = false;
                                    for(int k = 0; k < i; k++) {
                                        if(colours[k] == col)
                                            found = true;
                                    }
                                    if(!found) {
                                        return 0;
                                    }
                                }
                            }
                            if(c == 0) {
                                int col = side[s]->border[1][r];
                                if(col != -1) {
                                    bool found = false;
                                    for(int k = 0; k < i; k++) {
                                        if(colours[k] == col)
                                            found = true;
                                    }
                                    if(!found) {
                                        return 0;
                                    }
                                }
                            }
                            
                            if(r == 2) {
                                int col = side[s]->border[2][c];
                                if(col != -1) {
                                    bool found = false;
                                    for(int k = 0; k < i; k++) {
                                        if(colours[k] == col)
                                            found = true;
                                    }
                                    if(!found) {
                                        return 0;
                                    }
                                }
                            }
                            if(c == 2) {
                                int col = side[s]->border[3][r];
                                if(col != -1){
                                    bool found = false;
                                    for(int k = 0; k < i; k++) {
                                        if(colours[k] == col)
                                            found = true;
                                    }
                                    if(!found) {
                                        return 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // save this configuration
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 3; j++) {
            for(int k = 0; k < 3; k++) {
                side[i]->valid_face[j][k] = side[i]->face[j][k];
            }
        }
    }
    return 1;
}

RubiksCube::RubiksCube() {
    // go through each cube and initiate
    m_moving = false;
    m_side = 0;
    m_angle = 0;
    m_dir = 0;
    m_start_time = 0;
    m_times = 0;
    m_counter = 0;
    m_current_step = 0;
	int xn, yn, zn;
	for(xn = 0; xn < 3; xn++) {
		for(yn = 0; yn < 3; yn++) {
			for(zn = 0; zn < 3; zn++) {
				// set up cube
                cubes[xn][yn][zn] = new Cubelet();
                cubes[xn][yn][zn]->init();
			}
		}
	}
	
    xSpin = 0;
    ySpin = 0;
    zSpin = 0;
    
    xRot = new int[3];
    yRot = new int[3];
    zRot = new int[3];
	
    for(int i = 0; i < 3; i++) {
        xRot[i] = 0;
        yRot[i] = 0;
        zRot[i] = 0;
    }
	
    for(int i = 0; i < 6; i++) {
        side[i] = new Side();
    }
}

void RubiksCube::update() {
    for(int s = 0; s < 6; s++) {
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                set_face_colour(s, i, j, side[s]->face[i][j]);
            }
        }
    }
}

void RubiksCube::draw() {
    glRotatef(xSpin, 1, 0, 0);
    glRotatef(ySpin, 0, 1, 0);
    glRotatef(zSpin, 0, 0, 1);
    // update the faces
	for(int x = 0; x < 3; x++) {
        glRotatef(xRot[x], 1, 0, 0);
		for(int y = 0; y < 3; y++) {
            glRotatef(yRot[y], 0, 1, 0);
			for(int z = 0; z < 3; z++) {
                glRotatef(zRot[z], 0, 0, 1);
                
                glTranslatef(x-1.0f, y-1.0f, z-1.0f);
				cubes[x][y][z]->draw();
                glTranslatef(1.0f-x, 1.0f-y, 1.0f-z);
                
                glRotatef(-zRot[z], 0, 0, 1);
			}
            glRotatef(-yRot[y], 0, 1, 0);
		}
        glRotatef(-xRot[x], 1, 0, 0);
	}
	
    glRotatef(-zSpin, 0, 0, 1);
    glRotatef(-ySpin, 0, 1, 0);
    glRotatef(-xSpin, 1, 0, 0);
}

void RubiksCube::set_solved() {
	for(int i = 0; i < 6; i++) {
		Side tmp;
		for(int x = 0; x < 3; x++) {
			for(int y = 0; y < 3; y++) {
				tmp.face[x][y] = i;
			}
		}
		read_side(&tmp);
	}
}


void RubiksCube::read_side(Side* tmpside) {
    /*
	 Given a side, try to update the current model of the rubik's cube
	 Or initialise it if nothing has been set
	 Opposite sides:
	 Green -> blue
	 Red -> orange
	 yellow -> white
	 
	 Hence a red side may border: green, blue, yellow white but never orange
	 */
    
    if(tmpside->face[1][1] >= 0) {
        // we've found the centre face, making this job much easier
        int centre = tmpside->face[1][1];
        int s = centre;
        //printf("updated side %d\n", s);
        // try to align tmp face with data we already have
        // initially fill this side with the same data from tmpside
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++) {
                side[s]->face[i][j] = tmpside->face[i][j];
            }
        }
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 3; j++) {
                side[s]->border[i][j] = tmpside->border[i][j];
            }
        }
		side[s]->has_data = true;
        printf("side[%d] has data\n", s);
        // now align all sides
		
    } else {
        // hmm? need a strategy to cope with this
        printf("Skipping face\n");
    }
}

void RubiksCube::set_face_colour(int side, int i, int j, int colour) {
    //cubes[X][Y][Z].set_face(side, colour);
    Cubelet* c = get_cubelet(side, i, j);
    c->set_face(side, colour);
    c->face[side]->i = i;
    c->face[side]->j = j;
    c->face[side]->side = side;
    c->face[side]->colour = colour;
}

Cubelet* RubiksCube::get_cubelet(int side, int i, int j) {
    int X, Y, Z, face;
    switch (side) {
        case RED:
            // base
            X = i;
            Y = j;
            Z = 2;
            break;
        case BLUE:
            // left
            X = 2;
            Y = i;
            Z = j;
            break;
        case WHITE:
            // front
            X = 2-i;
            Y = 2;
            Z = j;
            break;
        case GREEN:
            // right
            X = 0;
            Y = i;
            Z = 2-j;
            break;
        case YELLOW:
            // back
            X = j;
            Y = 0;
            Z = 2-i;
            break;
        case ORANGE:
            // top
            X = i;
            Y = 2-j;
            Z = 0;
            break;
    }
    
    return cubes[X][Y][Z];
}

int notation_to_colour(char p) {
    for(int i = 0; i < 6; i++) {
        if(DEF2[i] == p) {
            return i;
        }
    }
}

char *RubiksCube::find_corner(char* p) {
    // the input descibes the corner we're looking for
    int f1 = notation_to_colour(p[0]);
    int f2 = notation_to_colour(p[1]);
    int f3 = notation_to_colour(p[2]);
    
    // find a cubelet with those faces set
    for (int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
            for (int z = 0; z < 3; z++) {
                if((x == 0 || x == 2) && (y == 0 || y == 2) && (z == 0 || z == 2)) {
                    Cubelet* c = cubes[x][y][z];
                    if(c->face[f1]->colour != -1 && c->face[f2]->colour != -1 && c->face[f3]->colour != -1) {
                        return c->description(c->face[f1]->colour, c->face[f2]->colour, c->face[f3]->colour);
                    }
                }
            }
        }
    }
}

char *RubiksCube::find_edge(char* p) {
    int f1 = notation_to_colour(p[0]);
    int f2 = notation_to_colour(p[1]);
    
    // find a cubelet with those faces set
    for (int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
            for (int z = 0; z < 3; z++) {
                if(x == 1 || y == 1 || z == 1) {
                    Cubelet* c = cubes[x][y][z];
                    if(c->face[f1]->colour != -1 && c->face[f2]->colour != -1) {
                        return c->description(c->face[f1]->colour, c->face[f2]->colour);
                    }
                }
            }
        }
    }
    printf("\nCouldn't an edge on faces %d, %d\n", f1, f2);
	char *bad = new char[2];
	bad[0] = 'X';
	bad[1] = '\0';
	return bad;
}

char* RubiksCube::get_notation() {
    /*You must pass the cube's position as stdin as so:
	 
	 UF UR UB UL DF DR DB DL FR FL BR BL UFR URB UBL ULF DRF DFL DLB DBR
	 */
    char *state = new char[70];
    sprintf(state, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\0",
			find_edge("UF"),
			find_edge("UR"),
			find_edge("UB"),
			find_edge("UL"),
			find_edge("DF"),
			find_edge("DR"),
			find_edge("DB"),
			find_edge("DL"),
			find_edge("FR"),
			find_edge("FL"),
			find_edge("BR"),
			find_edge("BL"),
			find_corner("UFR"), 
			find_corner("URB"),
			find_corner("UBL"),
			find_corner("ULF"),
			find_corner("DRF"), 
			find_corner("DFL"),
			find_corner("DLB"),
			find_corner("DBR")
			);
    return state;
}

void RubiksCube::ShowNextMove() {
    if(!m_moving) {
        // show the next move
        if(m_current_step < s.steps) {
            if(s.solution[m_current_step] != -1) {
                ShowMove(a_twist_string[s.solution[m_current_step]]);
                m_current_step++;
            }
        }
    }
}

void RubiksCube::solve() {
    s = solve_main(get_notation());
    
    if(!s.solved) {
        printf("No solution\n");
    }
	
    //printf("Found a solution in %d moves\n", s.steps);
    for(int i = 0; i < s.steps; i++) {
        if(s.solution[i] != -1) {
            //printf("%s ",a_twist_string[s.solution[i]]);
        }
    }
    //printf("\n");
    
    m_current_step = 0;
}

void RubiksCube::UpdatePositions() {
    if(m_moving) {
        struct timeval m_tv;
        gettimeofday(&m_tv, NULL);
        int new_time = m_tv.tv_usec;
        // time ranges from 0 -> 1000000 us
        int diff;
        if(new_time < m_start_time) {
            diff = (1000000 - m_start_time) + new_time;
        } else {
            diff = new_time - m_start_time;
        }
        int turns = floor(diff*90/(1000000*m_duration));
        if(turns > 0) {
            
            if((m_angle + turns) >= 90*m_times) {
                turns = (90*m_times) - m_angle;
            }
            
            RotateSide(m_side, m_dir*turns);
            m_angle+= turns;
            // update start time
            // include diff so there is no accumilated error
            m_start_time = m_start_time + turns*(1000000*m_duration/90);
			if(m_start_time > 1000000) {
				m_start_time = 1000000 - m_start_time;
			}
			
            if(m_angle >= 90*m_times) {
                //printf("finished move on m_angle = %d\n", m_angle);
				m_moving = false;               
            }
        }
    }
}

void RubiksCube::ShowMove(char* m, double duration) {
    m_duration = duration;
    // m takes form 'R2'
    // first character describes the face to turn
    // second describes how to turn it
    m_side = notation_to_colour(m[0]);
    m_dir = 0;
    m_times = 1;
    switch(m[1]) {
        case '\'':
            m_dir = -1;
            break;
        case '2':
            m_times = 2;
            m_dir = 1;
            break;
        default:
            m_dir = 1;
            break;
    }
    //printf("m = %s dir = %d t = %d\n", m, m_dir, m_times);
    if(m[0] == 'R' || m[0] == 'B' || m[0] == 'U') {
        m_dir *= -1;
    }
    
    m_moving = true;
    m_angle = 0;
    m_counter = 0;
    struct timeval m_tv;
    gettimeofday(&m_tv, NULL);
    m_start_time = m_tv.tv_usec;
    return;
}

void RubiksCube::ShowMove(char* m) {
    ShowMove(m, TURN_TIME);
}

void RubiksCube::ShowMove(int move) {
    char* m = a_twist_string[move];
    //printf("Move = %d\n");
    ShowMove(m);
}

bool RubiksCube::FinishedSolve() {
    if(s.solution[m_current_step] == -1 && !m_moving) {
        return true;
    }
    return false;
}


void RubiksCube::RotateSide(int side, int angle) {
    int *r;
    int b;
    int d;
    if(side == RED) {
        r = zRot;
        b = 2;
        d = 2;
    } else if(side == ORANGE) {
        r = zRot;
        b = 0;
        d = 2;
    } else if(side == YELLOW) {
        r = yRot;
        b = 0;
        d = 1;
    } else if(side == WHITE) {
        r = yRot;
        b = 2;
        d = 1;
    } else if(side == GREEN) {
        r = xRot;
        b = 0;
        d = 0;
    } else {
        r = xRot;
        b = 2;
        d = 0;
    }
    
    r[b] += angle;
    
    if(angle < 0 && r[b] <= -90) {
        // shift cubes around because we've finished the move
        r[b] += 90;
        //printf("Finished rotating in %d direction\n", d);
        
        if(d == 0) {
            // x rotation
            Cubelet *c = cubes[b][0][0];
            cubes[b][0][0] = cubes[b][2][0];
            cubes[b][2][0] = cubes[b][2][2];
            cubes[b][2][2] = cubes[b][0][2];
            cubes[b][0][2] = c;
            
            c = cubes[b][1][0];
            cubes[b][1][0] = cubes[b][2][1];
            cubes[b][2][1] = cubes[b][1][2];
            cubes[b][1][2] = cubes[b][0][1];
            cubes[b][0][1] = c;
            
            // now rotate each cube
            for(int i=0; i < 3; i++){
                for(int j=0; j < 3; j++) {
                    cubes[b][i][j]->RCCW(d);
                }
            }
        } else if(d == 1) {
            // y rotation
            Cubelet *c = cubes[0][b][2];
            cubes[0][b][2] = cubes[2][b][2];
            cubes[2][b][2] = cubes[2][b][0];
            cubes[2][b][0] = cubes[0][b][0];
            cubes[0][b][0] = c;
            
            c = cubes[0][b][1];
            cubes[0][b][1] = cubes[1][b][2];
            cubes[1][b][2] = cubes[2][b][1];
            cubes[2][b][1] = cubes[1][b][0];
            cubes[1][b][0] = c;
            // now rotate each cube
            for(int i=0; i < 3; i++){
                for(int j=0; j < 3; j++) {
                    cubes[i][b][j]->RCCW(d);
                }
            }
        } else {
            // z rotation
            Cubelet *c = cubes[0][0][b];
            cubes[0][0][b] = cubes[2][0][b];
            cubes[2][0][b] = cubes[2][2][b];
            cubes[2][2][b] = cubes[0][2][b];
            cubes[0][2][b] = c;
            
            c = cubes[1][0][b];
            cubes[1][0][b] = cubes[2][1][b];
            cubes[2][1][b] = cubes[1][2][b];
            cubes[1][2][b] = cubes[0][1][b];
            cubes[0][1][b] = c;
            
            for(int i=0; i < 3; i++){
                for(int j=0; j < 3; j++) {
                    cubes[i][j][b]->RCCW(d);
                }
            }
        }
    } else if (angle > 0 && r[b] >= 90) {
        //printf("Finished rotating in %d direction\n", d);
        r[b] -= 90;
        if(d == 0) {
            // x rotation
            Cubelet *c = cubes[b][0][2];
            cubes[b][0][2] = cubes[b][2][2];
            cubes[b][2][2] = cubes[b][2][0];
            cubes[b][2][0] = cubes[b][0][0];
            cubes[b][0][0] = c;
            
            c = cubes[b][0][1];
            cubes[b][0][1] = cubes[b][1][2];
            cubes[b][1][2] = cubes[b][2][1];
            cubes[b][2][1] = cubes[b][1][0];
            cubes[b][1][0] = c;
            for(int i=0; i < 3; i++){
                for(int j=0; j < 3; j++) {
                    cubes[b][i][j]->RCW(d);
                }
            }
        } else if(d == 1) {
            // y rotation
            Cubelet *c = cubes[0][b][0];
            cubes[0][b][0] = cubes[2][b][0];
            cubes[2][b][0] = cubes[2][b][2];
            cubes[2][b][2] = cubes[0][b][2];
            cubes[0][b][2] = c;
            
            c = cubes[1][b][0];
            cubes[1][b][0] = cubes[2][b][1];
            cubes[2][b][1] = cubes[1][b][2];
            cubes[1][b][2] = cubes[0][b][1];
            cubes[0][b][1] = c;
            
            for(int i=0; i < 3; i++){
                for(int j=0; j < 3; j++) {
                    cubes[i][b][j]->RCW(d);
                }
            }
            
        } else {
            // z direction
            Cubelet *c = cubes[0][2][b];
            cubes[0][2][b] = cubes[2][2][b];
            cubes[2][2][b] = cubes[2][0][b];
            cubes[2][0][b] = cubes[0][0][b];
            cubes[0][0][b] = c;
            
            c = cubes[0][1][b];
            cubes[0][1][b] = cubes[1][2][b];
            cubes[1][2][b] = cubes[2][1][b];
            cubes[2][1][b] = cubes[1][0][b];
            cubes[1][0][b] = c;
            
            for(int i=0; i < 3; i++){
                for(int j=0; j < 3; j++) {
                    cubes[i][j][b]->RCW(d);
                }
            }
        }
    }
}

// return true if the cube is fully defined
bool RubiksCube::defined() {
    for(int s = 0; s < 6; s++) {
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                if(side[s]->face[i][j] == -1) {
                    return false;
                }
            }
        }
    }
    
    return true;
}