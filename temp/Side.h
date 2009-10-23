#pragma once
class Side {
public:
    Side();
    int face[3][3];
    int valid_face[3][3];
    /*
    border[4][3] defines the colours which border each face
    one row of 3 for each of the 4 sides
    numbered clockwise
    */
    int border[4][3];
    bool has_data;
    void draw();
    void rotate();
};