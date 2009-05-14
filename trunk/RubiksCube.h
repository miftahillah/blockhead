#pragma once

#include "Side.h"
#include "Cubelet.h"
#include "Solver.cpp"

#define TURN_TIME 3

class RubiksCube {
private:
    Side* side[6];
    Cubelet* cubes[3][3][3];
    float xSpin, ySpin, zSpin;
    int *xRot, *yRot, *zRot;
    bool m_moving;
    int m_side;
    int m_angle;
    int m_dir;
    int m_start_time;
    int m_times;
    int m_counter;
    int m_current_step;
    Solution s;
    
public:
    RubiksCube();
    void read_side(Side* side);
    void draw();
    void align_sides();
    int valid_state(int s);
    int valid_state();
    void set_face_colour(int side, int x, int y, int colour);
    Cubelet* get_cubelet(int side, int i, int j);
    void update();
    char* get_notation();
    char *find_corner(char* p);
    char *find_edge(char* p);
    
    void solve();
    
    void ShowMove(int move);
    void ShowMove(char *m);
    void UpdatePositions();
    
    void ShowNextMove();
    void RotateSide(int side, int degrees);

	void StepForward();
	void StepBack();
};