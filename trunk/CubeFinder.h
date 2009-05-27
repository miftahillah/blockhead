#pragma once
#define MAX_CORNERS 100
#define PORTION 0.3
#define DISTANCE_BETWEEN_END_POINTS 10

#define PI 3.14159265

#include "RubiksCube.h"

struct Face {
    // face defined from p1 -> p2 -> p3 -> p4 -> p1
    CvPoint p1, p2, p3, p4;
    CvScalar rgb;
	CvPoint cen;
    int length1, length2;
    int ang1, ang2;
    int dir1, dir2;
    int area;
    int side;
    int colour;
    int x, y;
};

struct LineStat {
    int avg, var;
};

struct LineInfo {
    int hue_avg, sat_avg, val_avg;
    int hue_var, sat_var, val_var;
    int pixels;
    int *hue, *sat, *val;
    int start, end;
    CvPoint s, e;
	bool success;
};

struct IntersectionResult {
    bool intersects;
    CvPoint intersection;
};

struct SeriesPeaks {
    int num_peaks;
    int *peak_avg;
    int *closest_peak;
};

class CubeFinder{
public:
	CvSeq* read_frame(IplImage* cframe);
	CvSeq* read_frame(IplImage* cframe, bool correct);
    void draw();
	CvSeq* final_faces;
	IplImage* final_frame;
private:
    //
    RubiksCube cube;
	
};

struct Correction{
    IplImage* frame;
    CvSeq* faces;
    bool finished;
};

void click_callback(int event, int x, int y, int flags, void* param);
void print_faces(CvSeq* faces);
void correct_detection(Correction* c);
IplImage* isolate_cube(IplImage* cframe);
void flood_face(CvPoint i, CvScalar target, Face* face);
bool faces_collide(Face* f1, Face* f2);
int side(CvPoint* pt1, CvPoint* pt2, CvPoint* point);
bool inside_face(Face* face, CvPoint point);
LineInfo read_line(int x0, int y0, int x1, int y1);
LineStat line_stats(int* array, int pixels);
LineStat line_stats(int* array, int start, int end);
IntersectionResult intersection(CvPoint* p1, CvPoint* p2, CvPoint* p3, CvPoint* p4);
bool in_face(CvSeq* faces, Face* face);
bool in_face(CvSeq* faces, CvPoint intersection);
void explore_ext(CvSeq* faces, CvPoint p, int dx, int dy) ;
bool low_variance( int hue_var, int sat_var, int val_var);
bool valid_face_angles(Face* face);
SeriesPeaks group_peaks(int *line_direction);
int average_face_area(CvSeq* faces);
void exclude_invalid_faces(CvSeq* faces);
int pt_dist_from_line(CvPoint pt1, CvPoint pt2, CvPoint point);
int inside_face(CvSeq* faces, CvPoint intersection);
void find_possible_cross_lines(CvSeq* possible_cross_line, IplImage* cframe);
void find_candidate_faces(CvSeq* faces, CvSeq* possible_cross_line);
void find_missing_faces(CvSeq* faces);
SeriesPeaks find_dominant_angles(CvSeq* faces);
void align_sides(CvSeq* faces, CvSeq* sides, SeriesPeaks peak);
void colour_result(IplImage* cframe, CvSeq* faces);
void split_double_faces(CvSeq* faces, SeriesPeaks peak);
void split_face(CvSeq* faces, Face* face, CvPoint m1, CvPoint m2);
void print_face(int i);