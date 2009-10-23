#include "side.h"

void Side::rotate() {
    int tmp = face[0][0];
    face[0][0] = face[0][2];
    face[0][2] = face[2][2];
    face[2][2] = face[2][0];
    face[2][0] = tmp;
    
    tmp = face[0][1];
    face[0][1] = face[1][2];
    face[1][2] = face[2][1];
    face[2][1] = face[1][0];
    face[1][0] = tmp;
    
    int tmp1 = border[0][0];
    int tmp2 = border[0][1];
    int tmp3 = border[0][2];
    
    border[0][0] = border[3][0];
    border[0][1] = border[3][1];
    border[0][2] = border[3][2];
    
    border[3][0] = border[2][2];
    border[3][1] = border[2][1];
    border[3][2] = border[2][0];
    
    border[2][2] = border[1][2];
    border[2][1] = border[1][1];
    border[2][0] = border[1][0];
    
    border[1][2] = tmp1;
    border[1][1] = tmp2;
    border[1][0] = tmp3;
}

Side::Side() {
    // set all face values to unknown
    has_data = false;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 3; j++) {
            if(i < 3)
                face[i][j] = -1;
            border[i][j] = -1;
        }
}