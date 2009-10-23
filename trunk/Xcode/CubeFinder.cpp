#include "CubeFinder.h"

#include "rubiks.h"
#include "cv.h"
#include "highgui.h"

IplImage* HSV;
IplImage* RGB;

void CubeFinder::draw() {
    cube.align_sides();
    cube.draw();
}

CvSeq* CubeFinder::read_frame(IplImage* cframe) {
    return read_frame(cframe, false);
}

/* search through image data and isolate the image of the rubiks cube */
CvSeq* CubeFinder::read_frame(IplImage* cframe, bool correct) {
    CvMemStorage* possible_cross_storage = cvCreateMemStorage(0);
    CvSeq* possible_cross_line = cvCreateSeq( CV_SEQ_ELTYPE_POINT|CV_32SC2, 
        sizeof(CvSeq), sizeof(CvPoint), possible_cross_storage );

    // find lines of solid colour between corners
    find_possible_cross_lines(possible_cross_line, cframe);
    
    printf("Found %d possible cross lines\n", possible_cross_line->total);
    
    CvMemStorage* possible_face_storage = cvCreateMemStorage(0);
    CvSeq* faces = cvCreateSeq( 0, 
        sizeof(CvSeq), sizeof(Face), possible_face_storage );

    // find candidate faces, which are areas of solid colour
    find_candidate_faces(faces, possible_cross_line);
    printf("Found %d candidate faces\n", faces->total);

    // search around the faces we've found to add any missing faces
    find_missing_faces(faces);
    printf("Found %d candidate faces after missing\n", faces->total);
    
    // get rid of faces at irregular angles / large areas
    exclude_invalid_faces(faces);
    printf("Found %d faces after removing bad ones\n", faces->total);

    // find the dominant angles in image and group faces
    SeriesPeaks peak = find_dominant_angles(faces);
    
    split_double_faces(faces, peak);
    printf("Found %d faces after splitting double\n", faces->total);
    
    // now sort the faces into sides -> 3x3 grid
    CvMemStorage* side_storage = cvCreateMemStorage(0);
    CvSeq* sides = cvCreateSeq( 0, 
        sizeof(CvSeq), sizeof(Side), side_storage );
        
    Correction c;
    
    printf("Started with %d faces\n", faces->total);
    
    c.frame = cframe;
    c.faces = faces;
    c.finished = false;
    
    if(correct) {
        correct_detection(&c);

        printf("Ended up with %d faces\n", faces->total);

        print_faces(faces);
    } 

    
    peak = find_dominant_angles(faces);
    
    // produce 3x3 arrays of sides
    align_sides(faces, sides, peak);
    
    for(int i = 0; i < sides->total; i++) {
        Side* s = (Side*)cvGetSeqElem(sides, i);
        if(i == 1) {
            for(int j = 0; j < 3; j++) {
                int tmp = s->face[0][j];
                s->face[0][j] = s->face[2][j];
                s->face[2][j] = tmp;
            }
        }
        cube.read_side(s);
    }
    
    // read each side into the rubiks cube
    for(int i = 0; i < sides->total; i++) {
        Side* side = (Side*)cvGetSeqElem(sides, i);
        printf("\t\t");
        for(int k = 0; k < 3; k++) {
            print_face(side->border[0][k]);
        }
        printf("\n\n");
        for(int i = 0; i < 3; i++) {
            printf("\t");
            print_face(side->border[1][i]);
            for(int j = 0; j < 3; j++) {
                print_face(side->face[i][j]);
            }
            print_face(side->border[3][i]);
            printf("\n");
        }
        printf("\n");
        printf("\t\t");
        for(int k = 0; k < 3; k++) {
            print_face(side->border[2][k]);
        }
        printf("\n\n\n");
        
        //cube.read_side(side);
    }
    
    //cube.align_sides();
    
    colour_result(cframe, faces);
    cvShowImage("Rubiks", cframe);
    
    /*
    if(faces->total > 6) {
        // let the user check + correct the result
        cvNamedWindow("Checker", 1);
        cvShowImage("")
        final_faces = faces;
        final_frame = cframe;
        
        cvSetMouseCallback("Checker", 
          click_callback, 
          (void*) cube 
        );
    }
    */
    
    // tidy up memory usage
    cvClearSeq(faces);
    cvClearMemStorage(possible_face_storage);   
    cvClearSeq(possible_cross_line);
    cvClearMemStorage(possible_cross_storage);
    //cvClearSeq(sides);
    //cvClearMemStorage(side_storage);
    
    cvReleaseImage( &HSV);
    
    return sides;
    //return cframe;
}

void correct_detection(Correction *c) {
    // make a note of the original image
    IplImage* original = cvCloneImage(c->frame);
    
    IplImage* f = cvCloneImage(c->frame);
    
    colour_result(f, c->faces);
    
    cvShowImage("Rubiks", f);
    
    cvSetMouseCallback("Rubiks", 
      click_callback, 
      (void*) c 
    );
    
    cvWaitKey(0);
}

void click_callback(int event, int x, int y, int flags, void* param) {
    Correction* c = (Correction*) param;
    
    // find where the click was made
    if(event == CV_EVENT_LBUTTONUP) {
       // find the face
        printf("Found mouse up at (%d, %d)\n", x, y);
        int i = inside_face(c->faces, cvPoint(x, y));
        if(i > 0) {
            Face* f = (Face*)cvGetSeqElem(c->faces, i);
            f->colour++;
            if(f->colour > 5) {
                f->colour = 0;
            }
            printf("incremented face %d (now %d)\n", i, f->colour);
        }
        // redraw
        IplImage* n = cvCloneImage(c->frame);
        colour_result(n, c->faces);
        cvShowImage("Rubiks", n);
    }
    
    if(event == CV_EVENT_RBUTTONUP) {
        // find the face
         printf("Found mouse up at (%d, %d)\n", x, y);
         int i = inside_face(c->faces, cvPoint(x, y));
         if(i > 0) {
             cvSeqRemove(c->faces, i);
         }
         // redraw
         IplImage* n = cvCloneImage(c->frame);
         colour_result(n, c->faces);
         cvShowImage("Rubiks", n);
    }
}

void print_face(int i) {
    switch(i) {
        case RED:
        printf("R\t");
        break;
        case ORANGE:
        printf("O\t");
        break;
        case YELLOW:
        printf("Y\t");
        break;
        case GREEN:
        printf("G\t");
        break;
        case BLUE:
        printf("B\t");
        break;
        case WHITE:
        printf("W\t");
        break;
        default:
        printf("?\t");
        break;
    }
}

void find_possible_cross_lines(CvSeq* possible_cross_line, IplImage* cframe) {
    IplImage* frame = cvCreateImage(cvGetSize(cframe), 8, 1);
    // convert to black & white
    cvCvtColor( cframe, frame, CV_BGR2GRAY );
    printf("Finding corners..\n");

    // go through each pair of points
    // trace a line and look for constant colour between points
    // look at colour in HSI colour space
    HSV = cvCreateImage( cvGetSize(cframe), IPL_DEPTH_8U, 3);
    RGB = cvCloneImage(cframe);
    cvCvtColor(cframe, HSV, CV_BGR2HSV);
    
    IplImage* harris_eig_image = cvCreateImage(cvGetSize(cframe), IPL_DEPTH_32F, 1);
    IplImage* harris_temp_image = cvCreateImage(cvGetSize(cframe), IPL_DEPTH_32F, 1);
    CvPoint2D32f harris_corners[MAX_CORNERS];
    int harris_corner_count = MAX_CORNERS;
    double harris_quality_level = 0.01;
    double harris_min_distance = 10;
    int harris_eig_block_size = 3;
    int harris_use_harris = true;

    cvGoodFeaturesToTrack(frame,
            harris_eig_image,
            harris_temp_image,
            harris_corners,
            &harris_corner_count,
            harris_quality_level,
            harris_min_distance,
            NULL,
            harris_eig_block_size,
            harris_use_harris);
    
    for(int i = 0; i < harris_corner_count - 1; i++) {
        for(int j = i + 1; j < harris_corner_count; j++ ) {
            // consider hue variance
            int x1 = harris_corners[i].x;
            int y1 = harris_corners[i].y;
            int x0 = harris_corners[j].x;
            int y0 = harris_corners[j].y;

            /*
            If this line is too long, then don't bother checking it
            this reduces time taken by this function on cube6.jpg
            from 12.9mS to 2.9mS
            */
            if(abs(x1 - x0) > 150 || abs(y1 - y0) > 150) {
                continue;
            }

            LineInfo line = read_line(x0, y0, x1, y1);
            
            if(!line.success)
                continue;

            int avg_hue = line.hue_avg;
            int avg_sat = line.sat_avg;
            int avg_val = line.val_avg;

            int *hue = line.hue;
            int *sat = line.sat;
            int *val = line.val;

            int pixels = line.pixels;

            int limit = 50;

            if(line.hue_var < 20 && line.sat_var < 300 && line.val_var < 500) {
                // this is probably a nice line
                //cvLine( cframe, line.s, line.e, CV_RGB(10,220,30), 1, 8 );
                //printf("Drawing line from (%d, %d) to (%d, %d)\n", line.s.x, line.s.y, line.e.x, line.e.y);
                CvPoint p1 = cvPoint(x1, y0);
                CvPoint p2 = cvPoint(x1, y1);

                cvSeqPush( possible_cross_line, &line.s);
                cvSeqPush( possible_cross_line, &line.e);
            }
        }
    }

    
    //free(harris_corners);
    
    cvReleaseImage( &harris_eig_image);
    cvReleaseImage( &harris_temp_image);
    cvReleaseImage( &frame);
}

void find_candidate_faces(CvSeq* faces, CvSeq* possible_cross_line) {
    printf("Finding candidate faces\n");
    // find candidate faces
    // now go through the possible faces
    for(int i = 0; i + 3 < possible_cross_line->total; i += 2) {
        CvPoint* pt1 = (CvPoint*)cvGetSeqElem(possible_cross_line, i);
        CvPoint* pt2 = (CvPoint*)cvGetSeqElem(possible_cross_line, i+1);
        if(pt2->x < pt1->x) {
            CvPoint* tmp;
            tmp = pt1; pt1 = pt2; pt2 = tmp;
        }
        int maxy = pt1->y;
        int miny = pt2->y;
        if(pt2->y > maxy) {
            maxy = pt2->y;
            miny = pt1->y;
        }

        LineInfo line1 = read_line(pt1->x, pt1->y, pt2->x, pt2->y);
        for(int j = i + 2; j + 1 < possible_cross_line->total; j+= 2) {
            CvPoint* pt3 = (CvPoint*)cvGetSeqElem(possible_cross_line, j);
            CvPoint* pt4 = (CvPoint*)cvGetSeqElem(possible_cross_line, j+1);

            /*
             do some trivial tests to exclude non intersecting lines
            e.g max(pt3->x, pt4->x) < min(pt1->x, pt2-x)
            min(pt3->x, pt4->x) > max(pt1->x, pt2->x)
            then they don't intersect
            */
            if( (
                (pt3->x < pt4->x && (pt4->x < pt1->x || pt3->x > pt2->x)) ||
                (pt3->x >= pt4->x && (pt3->x < pt1->x || pt4->x > pt2->x))) || (
                (pt3->y < pt4->y && (pt4->y < miny || pt3->y > maxy)) ||
                (pt3->y >= pt4->y && (pt3->y < miny || pt4->y > maxy)) )
                ) {
                    // reduced from 142ms -> 116ms
                    //printf("out of range\n");
                    continue;
            }

            if(side(pt1, pt2, pt3) == side(pt1, pt2, pt4) ||
                side(pt3, pt4, pt1) == side(pt3, pt4, pt2)) {
                // pt3 and pt4 are on the same side as pt1 -> pt2
                // so there is no intersection
                // reduced from 127ms -> 117ms
                    //printf("both the same side\n");
                continue;
            }

            LineInfo line2 = read_line(pt3->x, pt3->y, pt4->x, pt4->y);

            // if we draw a line from pt1 -> pt2
            // then pt 3 must be above this line
            if(side(pt1, pt2, pt3) < 0) {
                // swap pt3 and pt4
                CvPoint* tmp;
                //printf("pt3 is on the wrong side!\n");
                tmp = pt3; pt3 = pt4; pt4 = tmp;
            }

            // see if these two lines intersect
            IntersectionResult I = intersection(pt1, pt2, pt3, pt4);

            if(I.intersects) {
                //cvCircle(cframe, I.intersection, 3, CV_RGB(0,0,255));
                Face face;

                // calculate the average hue / sat / intensity of these two lines
                int avg_hue = (line1.hue_avg + line2.hue_avg)/2;
                int avg_sat = (line1.sat_avg + line2.sat_avg)/2;
                int avg_val = (line1.val_avg + line2.val_avg)/2;

                face.p1.x = pt1->x; face.p1.y = pt1->y;
                face.p2.x = pt3->x; face.p2.y = pt3->y;
                face.p3.x = pt2->x; face.p3.y = pt2->y;
                face.p4.x = pt4->x; face.p4.y = pt4->y;

                find_colour(cvScalar(avg_hue, avg_sat, avg_val), &face);
                //printf("Found a face\n");

                face.cen = I.intersection;

                // see if this intersection point lies within any of the previously defined faces
                if(!in_face(faces, &face)) {
                    flood_face(I.intersection, cvScalar(avg_hue, avg_sat, avg_val), &face);
                    if(valid_face_angles(&face)) {
                        cvSeqPush(faces, &face);
                    }

                }
            }
        }
    }
}

void find_missing_faces(CvSeq* faces) {
    printf("Finding missing faces\n");
    /*
    Now we hopefully have a list of candidate faces
    these will likely be enough to identify the full cube
    but if we're reading it in for the first time we'll want
    to find the rest of the faces.
    
    Choose a starting face
    Calculate the average direction of its 2 pairs of similar edges
    Search Above, below, left and right in these directions
    If we find an already existing face, then we pair them up
    And update the averages
    Then continue searching
    */

    int i = 0;
    while(i < faces->total) {
        //printf("%d < %d\n", i, faces->total);
        Face* face = (Face*)cvGetSeqElem(faces, i);
        int dy11 = face->p2.y - face->p1.y;
        int dx11 = face->p2.x - face->p1.x;
        int dy12 = face->p3.y - face->p4.y;
        int dx12 = face->p3.x - face->p4.x;
        int dy21 = face->p3.y - face->p2.y;
        int dx21 = face->p3.x - face->p2.x;
        int dy22 = face->p4.y - face->p1.y;
        int dx22 = face->p4.x - face->p1.x;     
        int dy1 = (dy11 + dy12)/2;
        int dx1 = (dx11 + dx12)/2;
        int dy2 = (dy21 + dy22)/2;
        int dx2 = (dx21 + dx22)/2;

        // starting from the centre point of this face
        // look out at every side
        
        explore_ext(faces, cvPoint(face->cen.x + dx1, face->cen.y + dy1), dx1, dy1);
        explore_ext(faces, cvPoint(face->cen.x - dx1, face->cen.y - dy1), dx1, dy1);
        explore_ext(faces, cvPoint(face->cen.x + dx2, face->cen.y + dy2), dx2, dy2);
        explore_ext(faces, cvPoint(face->cen.x - dx2, face->cen.y - dy2), dx2, dy2);
        i++;    
    }
}

SeriesPeaks find_dominant_angles(CvSeq* faces) {
    printf("Finding Dominant Angles\n");
    int *line_direction = new int[180];
    for(int i = 0; i < 180; i++)
        line_direction[i] = 0;
    
    for(int i = 0; i < faces->total; i++) {
        Face* face = (Face*)cvGetSeqElem(faces, i);
        line_direction[face->ang1]++;
        line_direction[face->ang2]++;
    }
    SeriesPeaks peak = group_peaks(line_direction);
    
    for(int i = 0; i < faces->total; i++) {
        Face* face = (Face*)cvGetSeqElem(faces, i);
        // find closest angle to ang1 and ang2
        face->dir1 = peak.closest_peak[face->ang1];
        face->dir2 = peak.closest_peak[face->ang2];
        face->side = face->dir1 + face->dir2 - 1;
        if(face->dir1 == face->dir2) {
            // get rid of this
            cvSeqRemove(faces, i);
            i--;
        }        
    }

    return peak;
}

void split_double_faces(CvSeq* faces, SeriesPeaks peak) {
    printf("Splitting Double Faces\n");
    // first find average lengths in each direction
    // if any faces have double (or triple) this length
    // then split into two
    int *avg_length = new int[peak.num_peaks];
    int *c = new int[peak.num_peaks];
    for(int i = 0; i < peak.num_peaks; i++) {
        avg_length[i] = 0; 
        c[i] = 0;
    }
    for(int i = 0; i < faces->total; i++) {
        Face* f = (Face*)cvGetSeqElem(faces, i);
        avg_length[f->dir1] += f->length1;
        c[f->dir1]++;
        avg_length[f->dir2] += f->length2;
        c[f->dir2]++;
    }
    
    for(int i = 0; i < peak.num_peaks; i++) {
        if(c[i] == 0) {
            avg_length[i] = 0;
        } else {
            avg_length[i] = avg_length[i] / c[i];
        }
        
    }
        
    int total = faces->total;
    
    for(int i = 0; i < total; i++) {
        Face* f = (Face*)cvGetSeqElem(faces, i);
        
        double d1 = (double)(f->length1) / (double)avg_length[f->dir1];
        double d2 = (double)(f->length2) / (double)avg_length[f->dir2];
        
        int dy11 = f->p2.y - f->p1.y;
        int dx11 = f->p2.x - f->p1.x;
        int dy12 = f->p3.y - f->p4.y;
        int dx12 = f->p3.x - f->p4.x;
        int dy21 = f->p3.y - f->p2.y;
        int dx21 = f->p3.x - f->p2.x;
        int dy22 = f->p4.y - f->p1.y;
        int dx22 = f->p4.x - f->p1.x;       
        int dy1 = (dy11 + dy12)/2;
        int dx1 = (dx11 + dx12)/2;
        int dy2 = (dy21 + dy22)/2;
        int dx2 = (dx21 + dx22)/2;
        
        int length1 = sqrt(dy1*dy1 + dx1*dx1);
        int length2 = sqrt(dy2*dy2 + dx2*dx2);

        if(d1 > 1.8) {
            // split the face into two
            // split lines p1->p2, p3->p4
            CvPoint m1 = cvPoint(f->p1.x + (dx1/2), f->p1.y + (dy1/2));
            CvPoint m2 = cvPoint(f->p4.x + (dx1/2), f->p4.y + (dy1/2));
            split_face(faces, f, m1, m2);
        } else if(d2 > 1.6) {
            CvPoint m1 = cvPoint(f->p2.x + (dx2/2), f->p2.y + (dy2/2));
            CvPoint m2 = cvPoint(f->p1.x + (dx2/2), f->p1.y + (dy2/2));
            split_face(faces, f, m1, m2);
        }
    }
}

void split_face(CvSeq* faces, Face* f, CvPoint m1, CvPoint m2) {
    Face new_face;
    new_face.p2 = f->p2;
    new_face.p3 = f->p3;
    f->p2 = m1;
    f->p3 = m2;
    new_face.p1 = m1;
    new_face.p4 = m2;
    new_face.dir1 = f->dir1;
    new_face.dir2 = f->dir2;
    new_face.ang1 = f->ang1;
    new_face.ang2 = f->ang2;
    new_face.side = f->side;
    // todo: find actual colour
    new_face.colour = f->colour;
    new_face.rgb = f->rgb;
    new_face.length1 = f->length1 / 2;
    new_face.length2 = f->length2 / 2;
    f->length1 = new_face.length1;
    f->length2 = new_face.length2;
    // update centres
    IntersectionResult I1 = intersection(&f->p1, &f->p3, &f->p2, &f->p4);
    f->cen = I1.intersection;
    IntersectionResult I2 = intersection(&new_face.p1, &new_face.p3, &new_face.p2, &new_face.p4);
    new_face.cen = I2.intersection;
    cvSeqPush(faces, &new_face);
}

void shift_faces(CvSeq* faces, CvSeq* queue, Side* side, int p, int x, int y) {
    //printf("Shifting faces\n");
    for(int i = 0; i < faces->total; i++) {
        Face* f = (Face*)cvGetSeqElem(faces, i);
        
        if(f->side == p) {
            if(f->x != -1) {
                f->x += x;
                f->y += y;
            }
        }
    }
    for(int i = 0; i < queue->total; i++) {
        Face* f = (Face*)cvGetSeqElem(queue, i);
        
        if(f->side == p) {
            if(f->x != -1) {
                f->x += x;
                f->y += y;
            }
        }
    }
    
    // update side.found[][]
    if(x > 0) {
        for(int i = 2; i > 0; i--) {
            side->face[i][0] = side->face[i-1][0];
            side->face[i][1] = side->face[i-1][1];
            side->face[i][2] = side->face[i-1][2];
        }
        side->face[0][0] = -1;
        side->face[0][1] = -1;
        side->face[0][2] = -1;
    }
    if(y > 0) {
        for(int i = 2; i > 0; i--) {
            side->face[0][i] = side->face[0][i-1];
            side->face[1][i] = side->face[1][i-1];
            side->face[2][i] = side->face[2][i-1];
        }
        side->face[0][0] = -1;
        side->face[1][0] = -1;
        side->face[2][0] = -1;
    }
}

void align_sides(CvSeq* faces, CvSeq* sides, SeriesPeaks peak) {
    printf("Aligning sides\n");
    // reset all x/y values to -1
    for(int i = 0; i < faces->total; i++) {
        Face* f = (Face*)cvGetSeqElem(faces, i);
        f->x = -1;
        f->y = -1;
    }
    int distinct_faces = 3;
    if(peak.num_peaks == 2) distinct_faces = 1;
    if(faces->total > 0) {
        for(int p = 0; p < distinct_faces; p++) {
            Side tmp_side;
            int num_in_grid = 0;

            // calculate average legnths
            int avg_length1 = 0, avg_length2 = 0;
            int count = 0;
            int start_face;
            for(int i = 0; i < faces->total; i++) {
                Face* face1 = (Face*)cvGetSeqElem(faces, i);
                if(face1->side == p) {
                    count++;
                    start_face = i;
                    avg_length1 += face1->length1;
                    avg_length2 += face1->length2;
                }
            }

            if(count == 0) {
                continue;
            }
            
            // choose the first face
            Face* face = (Face*)cvGetSeqElem(faces, start_face);

            
            avg_length1 = avg_length1 / count;
            avg_length2 = avg_length2 / count;

            // search across to find first row
            // choose the angle closest to 0
            int ang1 = peak.peak_avg[face->dir1];
            int ang2 = peak.peak_avg[face->dir2];

            // calculate average explore directions
            int dy1 = avg_length1 * sin(ang1 * PI / 180)*1.2;
            int dx1 = avg_length1 * cos(ang1 * PI / 180)*1.2;
            int dy2 = avg_length2 * sin(ang2 * PI / 180)*1.2;
            int dx2 = avg_length2 * cos(ang2 * PI / 180)*1.2;



            // search in the direction of -dx1, -dy1 until we reach a corner
            
            bool exploring = true;
            tmp_side.face[0][0] = face->colour;
            CvMemStorage* queue_storage = cvCreateMemStorage(0);
            CvSeq* queue = cvCreateSeq( 0, sizeof(CvSeq), sizeof(Face), queue_storage );
            face->x = 0; face->y = 0;
            cvSeqPush(queue, face);
            while(exploring) {
                // check with 8 connectivity around the face
                // if any faces are found which haven't yet been identified
                // then add them to the queue
                // then remove this face
                /*
                (dx1, dy1) -> (x+1, y)
                (-dx1, -dy1) -> (x-1, y)
                (dx2, dy2) -> (x, y+1)
                (-dx2, -dy2) -> (x, y-1)
                */

                bool changed = false;
                while(queue->total > 0) {
                    // search around for faces
                    Face* f = (Face*)cvGetSeqElem(queue, 0);
                    int qx1 = inside_face(faces, cvPoint(f->cen.x - dx1, f->cen.y - dy1));
                    int qx2 = inside_face(faces, cvPoint(f->cen.x + dx1, f->cen.y + dy1));
                    int qy1 = inside_face(faces, cvPoint(f->cen.x - dx2, f->cen.y - dy2));
                    int qy2 = inside_face(faces, cvPoint(f->cen.x + dx2, f->cen.y + dy2));
                    if(f->x == 0 && qx1 != -1) {
                        Face* g = (Face*)cvGetSeqElem(faces, qx1);
                        if(g->side == p) {
                            // found a face outside the range
                            // so use this as the first face in the queue
                            // and start again
                            shift_faces(faces, queue, &tmp_side, p, 1, 0);
                            changed = true;
                        } else {
                            /**
                            There is a face on a different side bordering this face
                            so store this info to help align
                            */
                            tmp_side.border[0][f->y] = g->colour;
                        }                   
                    }
                    
                    if(f->y == 0 && qy1 != -1) {
                        Face* g = (Face*)cvGetSeqElem(faces, qy1);
                        if(g->side == p) {
                            // found a face outside the range
                            // so use this as the first face in the queue
                            // and start again
                            shift_faces(faces, queue, &tmp_side, p, 0, 1);
                            changed = true;
                        } else {
                            /**
                            There is a face on a different side bordering this face
                            so store this info to help align
                            */
                            tmp_side.border[1][f->x] = g->colour;
                        }                       
                    }
                    
                    // (x+1, y)
                    if(f->x < 2 && tmp_side.face[f->x+1][f->y] < 0) {
                        if(qx2 != -1) {
                            // there is a face at this point
                            Face* g = (Face*)cvGetSeqElem(faces, qx2);
                            if(g->side == p) {
                                g->x = f->x+1; g->y = f->y;
                                //printf("\t{%d, %d} = %d\n", g->x, g->y, g->colour);
                                cvSeqPush(queue, g);
                                changed = true;
                                tmp_side.face[g->x][g->y] = g->colour;
                            }
                        }
                    }
                    if(f->x == 2 && qx2 != -1) {
                        Face* g = (Face*)cvGetSeqElem(faces, qx2);
                        tmp_side.border[2][f->y] = g->colour;
                    }

                    if(f->x > 0 && tmp_side.face[f->x-1][f->y] < 0) {
                        if(qx1 != -1) {
                            // there is a face at this point
                            Face* g = (Face*)cvGetSeqElem(faces, qx1);
                            if(g->side == p) {
                                g->x = f->x-1; g->y = f->y;
                                //printf("\t{%d, %d} = %d\n", g->x, g->y, g->colour);
                                cvSeqPush(queue, g);
                                changed = true;
                                tmp_side.face[g->x][g->y] = g->colour;
                            }
                        }
                    }
                    
                    // (x, y+1)
                    if(f->y < 2 && tmp_side.face[f->x][f->y+1] < 0) {
                        if(qy2 != -1) {
                            // there is a face at this point
                            Face* g = (Face*)cvGetSeqElem(faces, qy2);
                            if(g->side == p) {
                                g->x = f->x; g->y = f->y+1;
                                //printf("\t{%d, %d}\n", g->x, g->y);
                                cvSeqPush(queue, g);
                                changed = true;
                                tmp_side.face[g->x][g->y] = g->colour;
                            }
                        }
                    }
                    if(f->y == 2 && qy2 != -1) {
                        Face* g = (Face*)cvGetSeqElem(faces, qy2);
                        tmp_side.border[3][f->x] = g->colour;
                    }

                    // (x, y-1)
                    if(f->y > 0 && tmp_side.face[f->x][f->y-1] < 0) {
                        if(qy1 != -1) {
                            // there is a face at this point
                            Face* g = (Face*)cvGetSeqElem(faces, qy1);
                            if(g->side == p) {
                                g->x = f->x; g->y = f->y-1;
                                //printf("\t{%d, %d}\n", g->x, g->y);
                                cvSeqPush(queue, g);
                                changed = true;
                                tmp_side.face[g->x][g->y] = g->colour;
                            }
                        }
                    }
                    cvSeqRemove(queue, 0);
                }
                
                if(!changed)
                    exploring = false;
                else
                    num_in_grid++;
            }
            cvSeqPush(sides, &tmp_side);
            if(num_in_grid < count / 2) {
                // the ones that have chosen are probably rubbish
            }
        }
    }   
}

void explore_ext(CvSeq* faces, CvPoint p, int dx, int dy) {
    
    if(p.x < 0 || p.x >= HSV->width || p.y < 0 || p.y >= HSV->height) {
        return;
    }
    //printf("{%d, %d} %d %d\n", p.x, p.y, dx, dy);
    if(!in_face(faces, p)) {
        LineInfo line = read_line(p.x - (dx / 20), p.y - (dy / 20), p.x + (dx / 20), p.y + (dy / 20));
        if(!line.success) {
            return;
        }
        
        if(low_variance(line.hue_var, line.sat_var, line.val_var)) {
            int line_length = (int)sqrt((line.s.x - line.e.x) * (line.s.x - line.e.x) + 
                (line.s.y - line.e.y) * (line.s.y - line.e.y));
            if(line_length < (abs(dy) + abs(dx))) {
                // this is probably a good starting point for a face
                CvPoint line_centre = cvPoint(line.s.x + (line.e.x - line.s.x)/2, line.s.y + (line.e.y - line.s.y)/2);
                //printf("centre = %d, %d\n", line_centre.x, line_centre.y);
                Face new_face;
                new_face.cen.x = line_centre.x;
                new_face.cen.y = line_centre.y;
                flood_face(line_centre, cvScalar(line.hue_avg, line.sat_avg, line.val_avg), &new_face);
                LineInfo new_line = read_line(new_face.p1.x, new_face.p1.y, new_face.p3.x, new_face.p3.y);
                find_colour(cvScalar(new_line.hue_avg, new_line.sat_avg, new_line.val_avg), &new_face);
                // double check that this face doesn't overlap any others
                if(!in_face(faces, &new_face)) {
                    //printf("Found a new face!\n");
                    // looks good, add it to the sequence
                    if(valid_face_angles(&new_face)) {
                        cvSeqPush(faces, &new_face);
                    }
                    
                }
            }
        }
    }
}

int inside_face(CvSeq* faces, CvPoint intersection) {
    for(int i = 0; i < faces->total; i++) {
        // existing face
        Face* eFace = (Face*)cvGetSeqElem(faces, i);
        if( inside_face(eFace, intersection)) {
            return i;
        }
    }
    return -1;
}

bool in_face(CvSeq* faces, CvPoint intersection) {
    for(int i = 0; i < faces->total; i++) {
        // existing face
        Face* eFace = (Face*)cvGetSeqElem(faces, i);
        if( inside_face(eFace, intersection)) {
            return true;
        }
    }
    return false;
}

bool in_face(CvSeq* faces, Face* face) {
    for(int i = 0; i < faces->total; i++) {
        // existing face
        Face* eFace = (Face*)cvGetSeqElem(faces, i);
        if(inside_face(eFace, face->cen) ||
            faces_collide(eFace, face)) {
            /*printf("Face (cen = (%d, %d)) is inside face %d\n", face->cen.x, face->cen.y, i);
            printf("[%d, %d] -> [%d, %d] -> [%d, %d] -> [%d, %d]\n", 
                eFace->p1.x, eFace->p1.y, 
                eFace->p2.x, eFace->p2.y, 
                eFace->p3.x, eFace->p3.y, 
                eFace->p4.x, eFace->p4.y);
                */
            return true;
        }
    }
    return false;
}

void flood_face(CvPoint i, CvScalar target, Face* face) {
    CvMemStorage* queue_storage = cvCreateMemStorage(0);
    CvSeq* queue = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), queue_storage );
    
    IplImage* face_image = cvCreateImage(cvGetSize(HSV), 8, 1);
   
    cvSeqPush(queue, &i);
    int iter = 0;
    while(queue->total > 0) {
        iter++;
        for(int i = 0; i < queue->total; i++) {
            CvPoint* n = (CvPoint*)cvGetSeqElem(queue, i);
            uchar* gs = (uchar*)(face_image->imageData + n->y*face_image->widthStep)[n->x];
            if(gs == 0 && compare_pixels(target, n)) {
                CvPoint e, w;
                e.x = n->x; e.y = n->y;
                w.x = n->x; w.y = n->y;
                bool searching_west = true;
                
                // pointer for all HSV pixels in this row
                uchar* p = (uchar*)(HSV->imageData + n->y*HSV->widthStep);
                while(w.x > 0 && compare_pixels(target, cvScalar(p[w.x*3], p[w.x*3+1], p[w.x*3+2]))) {
                    w.x--;
                }

                bool searching_east = true;
                while(e.x < face_image->width && compare_pixels(target, cvScalar(p[e.x*3], p[e.x*3+1], p[e.x*3+2]))) {
                    e.x++;
                }

                /*
                 9.   Set the color of nodes between w and e to replacement-color.
                10.   For each node n between w and e:
                11.    If the color of the node to the north of n is target-color, add that node to Q.
                       If the color of the node to the south of n is target-color, add that node to Q.
                */
                
                // pointer for all pixels in this row
                // allows fast memory access
                uchar* tp = (uchar*)(face_image->imageData + w.y*face_image->widthStep);
                for(int x = w.x; x < e.x; x++) {
                    // we can keep going
                    
                    tp[x] = 255;
                    CvPoint north = cvPoint(x, e.y + 1);
                    if(north.y < face_image->height) {
                        if(compare_pixels(target, &north)) {
                            uchar* np = (uchar*)(face_image->imageData + north.y*face_image->widthStep)[north.x];
                            if(np == 0) {
                                cvSeqPush(queue, &north);
                            }
                        }
                    }

                    CvPoint south = cvPoint(x, e.y - 1);
                    if(south.y > 0) {
                        if(compare_pixels(target, &south)) {
                            uchar* sp = (uchar*)(face_image->imageData + south.y*face_image->widthStep)[south.x];
                            if(sp == 0) {
                                cvSeqPush(queue, &south);
                            }
                        }
                    }
                }
            }
            // remove point from the queue
            cvSeqRemove(queue, i);
        }
    }
    //cvWaitKey(0);

    cvClearSeq(queue);
    cvClearMemStorage(queue_storage);
    //cvShowImage("Rubiks2", face_image);

    // perform closing to tidy up face
    // N.B this is slow(~70mS)
    //cvDilate(face_image, face_image);
    //cvErode(face_image, face_image);
    
     //cvCvtColor( result, result, CV_BGR2GRAY );
    // now search for contours in the face we've found
    CvMemStorage* contour_storage = cvCreateMemStorage(0); 
    CvSeq* first_contour  = NULL;

    int Nc = cvFindContours( face_image, contour_storage, &first_contour, sizeof(CvContour),CV_RETR_LIST );
    //printf("Found %d contours\n", Nc);
    bool found_a_face = false;
    for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) { 
        // find dominant points
        CvSeq* poly =  cvApproxPoly( c, sizeof(CvContour), contour_storage, CV_POLY_APPROX_DP, 10);
        //printf("points: %d\n", poly->total);
        //printf("Found polygon with %d elements\n", poly->total);
        if(poly->total > 4) {
            // find the shortest pair
            // remove it
            // repeat until only 4 are left
            CvPoint *pt1, *pt2, *pt3, *pt4;
            while(poly->total > 4) {
                int shortest_connection = -1;
                pt1 = (CvPoint*)cvGetSeqElem(poly, poly->total-1);
                pt2 = (CvPoint*)cvGetSeqElem(poly, 0);
                int last_distance = (pt1->x - pt2->x)*(pt1->x - pt2->x) + (pt1->y - pt2->y)*(pt1->y - pt2->y);
                int shortest_node = 0;
                
                for(int j = 0; j < poly->total; j++) {
                    pt1 = (CvPoint*)cvGetSeqElem(poly, j);
                    if(j == poly->total) {
                        pt2 = (CvPoint*)cvGetSeqElem(poly, 0);
                    } else {
                        pt2 = (CvPoint*)cvGetSeqElem(poly, j+1);
                    }
                    
                    int new_distance = (pt1->x - pt2->x)*(pt1->x - pt2->x) + (pt1->y - pt2->y)*(pt1->y - pt2->y);
                    
                    int shortest = new_distance;
                    if(last_distance < new_distance) {
                        shortest = last_distance;
                    }

                    if(shortest_connection < 0 || shortest < shortest_connection) {
                        shortest_node = j;
                        shortest_connection = shortest;
                    }

                    last_distance = new_distance;
                }
                
                // now remove the node with the shortest connections to it
                cvSeqRemove(poly, shortest_node);
                //printf("Removed %d (shortest_connection = %d)\n", shortest_node, shortest_connection);
            }
            pt1 = (CvPoint*)cvGetSeqElem(poly, 0);
            pt2  = (CvPoint*)cvGetSeqElem(poly, 2);
            pt3 = (CvPoint*)cvGetSeqElem(poly, 1);
            pt4 = (CvPoint*)cvGetSeqElem(poly, 3);
            // make sure that pt3 is on the correct side of pt1 -> pt2
            if(side(pt1, pt2, pt3) < 0) {
                // swap pt3 and pt4
                CvPoint* tmp;
                //printf("pt3 is on the wrong side!\n");
                tmp = pt3; pt3 = pt4; pt4 = tmp;
            }
            face->p1.x = pt1->x; face->p1.y = pt1->y;
            face->p2.x = pt3->x; face->p2.y = pt3->y;
            face->p3.x = pt2->x; face->p3.y = pt2->y;
            face->p4.x = pt4->x; face->p4.y = pt4->y;
            found_a_face = true;
        } else if(poly->total == 4) {
            // we only have 4 points, no need to cull
            CvPoint* pt1 = (CvPoint*)cvGetSeqElem(poly, 0);
            CvPoint* pt2  = (CvPoint*)cvGetSeqElem(poly, 2);
            CvPoint* pt3 = (CvPoint*)cvGetSeqElem(poly, 1);
            CvPoint* pt4 = (CvPoint*)cvGetSeqElem(poly, 3);
            // make sure that pt3 is on the correct side of pt1 -> pt2
            if(side(pt1, pt2, pt3) < 0) {
                // swap pt3 and pt4
                CvPoint* tmp;
                //printf("pt3 is on the wrong side!\n");
                tmp = pt3; pt3 = pt4; pt4 = tmp;
            }
            face->p1.x = pt1->x; face->p1.y = pt1->y;
            face->p2.x = pt3->x; face->p2.y = pt3->y;
            face->p3.x = pt2->x; face->p3.y = pt2->y;
            face->p4.x = pt4->x; face->p4.y = pt4->y;
            found_a_face = true;
        }
    }
    
    
    if(found_a_face) {
        // update the centre point
        IntersectionResult I = intersection(&face->p1, &face->p3, &face->p2, &face->p4);

        if(I.intersects) {
            if(!inside_face(face, I.intersection)) {
                printf("Serious error: point (%d, %d) is not inside its face\n", I.intersection.x, I.intersection.y);
            }
            //printf("Updated centre to be at %d, %d\n", I.intersection.x, I.intersection.y);
            face->cen.x = I.intersection.x;
            face->cen.y = I.intersection.y;
        }
    }
    
    cvClearMemStorage(contour_storage);
    cvReleaseImage( &face_image);
}
/**
TODO: This doesn't work in some situations, when two faces cross but have no points within each other
need to improve it
*/
bool faces_collide(Face* f1, Face* f2) {
    if( inside_face(f1, f2->p1) || 
        inside_face(f1, f2->p2) || 
        inside_face(f1, f2->p3) || 
        inside_face(f1, f2->p4) ||
        inside_face(f2, f1->p1) || 
        inside_face(f2, f1->p2) || 
        inside_face(f2, f1->p3) || 
        inside_face(f2, f1->p4)) {
        return true;
    }
    return false;        
}

int side(CvPoint* pt1, CvPoint* pt2, CvPoint* point) {
    int side = (point->y - pt1->y)*(pt2->x - pt1->x) - (point->x - pt1->x) * (pt2->y - pt1->y);
    if ( side < 0) {
        return -1;
    } else {
        return 1;
    }
}

bool inside_face(Face* face, CvPoint point) {
    //(y - y0) (x1 - x0) - (x - x0) (y1 - y0)
    // trace line from pt1 -> pt2 -> pt3 -> pt4 -> pt1
    // if point is always on the same side then point is inside
    int s1 = side(&face->p1, &face->p2, &point);
    int s2 = side(&face->p2, &face->p3, &point);
    int s3 = side(&face->p3, &face->p4, &point);
    int s4 = side(&face->p4, &face->p1, &point);
    
    if( s1 == s2 && s2 == s3 && s3 == s4) {
        //printf("[%d %d %d %d]\n", s1, s2, s3, s4);
        return true;
    }
    return false;
}

LineInfo read_line(int x0, int y0, int x1, int y1) {
    LineInfo result;
    int Dx = x1 - x0;
    int Dy = y1 - y0;
    // use Bresenham algorithm to determine pixel coordinates to look at
    // because it is efficient and only uses integer operations
    // this method needs to be quick because it will be performed many times
    bool steep = (abs(Dy) >= abs(Dx));
    int tmp;
    if(x0 < 0 || x0 > HSV->width || x1 < 0 || x1 > HSV->width ||
        y0 < 0 || y0 > HSV->height || y1 < 0 || y1 > HSV->height) {
            printf("Line is out of bounds\n");
            result.success = false;
            return result;
    }
    // create a dynamic array
    if(steep) {
        // swap x0 and y0
        tmp = x0; x0 = y0; y0 = tmp;
        // swap x1 and y1
        tmp = x1; x1 = y1; y1 = tmp;
        // recalculate Dx and Dy after swap
        Dx = x1 - x0;
        Dy = y1 - y0;
    }
    
    int xstep = 1;
    if (Dx < 0) {
        xstep = -1;
        Dx = -Dx;
    }
    int ystep = 1;
    if (Dy < 0) {
        ystep = -1;     
        Dy = -Dy; 
    }
    
    int pixels = abs(Dx) + 1;
    int *hue = new int[pixels];
    int *sat = new int[pixels];
    int *val = new int[pixels];
    int *xP = new int[pixels];
    int *yP = new int[pixels];
    
    int TwoDy = 2*Dy; 
    int TwoDyTwoDx = TwoDy - 2*Dx; // 2*Dy - 2*Dx
    int E = TwoDy - Dx; //2*Dy - Dx
    int y = y0;
    int xDraw, yDraw;
    //printf("Counting %d intervals\n", pixels);
    pixels = 0;
    for (int x = x0; (xstep == -1 && x >= x1) || (xstep == 1 && x <= x1); x += xstep) { 
        if(x == x1) {
            y = y1;
        }
        if (steep) {        
            xDraw = y;
            yDraw = x;
        } else {            
            xDraw = x;
            yDraw = y;
        }        
        // next
        if (E > 0) {
            E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
            y = y + ystep;
        } else {
            E += TwoDy; //E += 2*Dy;
        }

        if(xDraw < 0 || xDraw > HSV->width || yDraw < 0 || yDraw > HSV->height) {
            break;
        }
        
        xP[pixels] = xDraw;
        yP[pixels] = yDraw;

        hue[pixels] = ((uchar*)(HSV->imageData + HSV->widthStep*yDraw))[xDraw*3];
        if(hue[pixels] > 140) {
            // wrap red back to 0
            hue[pixels] = 0;
        }
        sat[pixels] = ((uchar*)(HSV->imageData + HSV->widthStep*yDraw))[xDraw*3+1];
        val[pixels] = ((uchar*)(HSV->imageData + HSV->widthStep*yDraw))[xDraw*3+2];
        pixels++;
    }
    
    /**
    generated the x/y coordinates of the line
    need to trace the line to find the steady middle portion
    For all colours apart from white, we're most interested in a steady hue line
    which indicates a solid block of colour
    For these colours it is not unlikely that the saturation may oscillate slightly
    Find the characteristics of the middle portion of the line
    and try to extend these as far as possible towards each end
    */

    int avg_hue=0, avg_sat=0, avg_val=0;
    
    int start = (int)(pixels * PORTION);
    int end = (int)(pixels * (1 - PORTION));
    
    LineStat hue_stat = line_stats(hue, start, end);
    LineStat sat_stat = line_stats(sat, start, end);
    LineStat val_stat = line_stats(val, start, end);
    
    CvScalar target = cvScalar(hue_stat.avg, sat_stat.avg, val_stat.avg);
    
    //if(hue_stat.var < 100 && val_stat.var < 500) {
    if(low_variance(hue_stat.var, sat_stat.var, val_stat.var)) {
        // looks like the middle portion of the line is of constant colour
        // try to extend the middle portion of the line
        // TODO: Probably need to do this bit properly
        // and update variance as we go along the line and check it is still low
        for(int x = start; x >= 0; x--) {
            if(compare_pixels(target, cvScalar(hue[x], sat[x], val[x]))) {
                start = x;
            } else {
                break;
            }
        }
        for(int x = end; x < pixels; x++) {
            if(compare_pixels(target, cvScalar(hue[x], sat[x], val[x]))) {
                end = x;
            } else {
                break;
            }
        }
    }
    
    hue_stat = line_stats(hue, start, end);
    sat_stat = line_stats(sat, start, end);
    val_stat = line_stats(val, start, end);
    
    result.s.x = xP[start];
    result.s.y = yP[start];
    result.e.x = xP[end];
    result.e.y = yP[end];
    
    result.hue_avg = hue_stat.avg;
    result.sat_avg = sat_stat.avg;
    result.val_avg = val_stat.avg;
    result.hue = hue;
    result.sat = sat;
    result.val = val;
    result.hue_var = hue_stat.var;
    result.sat_var = sat_stat.var;
    result.val_var = val_stat.var;
    result.pixels = pixels;
    result.end = end;
    result.start = start;
    result.success = true;
    
    return result;
}

bool low_variance( int hue_var, int sat_var, int val_var) {
    if((hue_var < 20 && sat_var < 100 && val_var < 100) ||
        (sat_var < 20 && val_var < 20) ||
        (hue_var < 50 && val_var < 50)) {
        return true;
    }
    return false;
}

// wrap the other line stats function
LineStat line_stats(int* array, int pixels) {
    return line_stats(array, 0, pixels);
}

LineStat line_stats(int* array, int start, int end) {
    LineStat stats;
    int sum_x = 0, sum_x2 = 0;
    int n = 0;
    for(int i = start; i < end; i++) {
        sum_x += array[i];
        sum_x2 += array[i]*array[i];
        n++;
    }
    if(n == 0) {
        stats.var = 999;
        stats.avg = -1;
        return stats;
    }
    stats.var = (sum_x2 / n) - (sum_x * sum_x / (n * n));
    stats.avg = sum_x / n;
    return stats;
}


IntersectionResult intersection(CvPoint* p1, CvPoint* p2, CvPoint* p3, CvPoint* p4) {   
    IntersectionResult intersection;
    
    int x1 = p1->x, y1 = p1->y;
    int x2 = p2->x, y2 = p2->y;
    int x3 = p3->x, y3 = p3->y;
    int x4 = p4->x, y4 = p4->y;
    
    // not very accurate, but doesn't really need to be
    // not bothering to use true method
    if( ((abs(x1 - x3) < DISTANCE_BETWEEN_END_POINTS && abs(y1 - y3) < DISTANCE_BETWEEN_END_POINTS) ||
        (abs(x1 - x4) < DISTANCE_BETWEEN_END_POINTS && abs(y1 - y4) < DISTANCE_BETWEEN_END_POINTS) ) &&
        ((abs(x2 - x3) < DISTANCE_BETWEEN_END_POINTS && abs(y2 - y3) < DISTANCE_BETWEEN_END_POINTS) ||
        (abs(x2 - x4) < DISTANCE_BETWEEN_END_POINTS && abs(y2 - y4) < DISTANCE_BETWEEN_END_POINTS))) {
            // start / end points are too close to each other
            intersection.intersects = false;
            return intersection;
        }
    
    int denom = ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));
    
    if(denom == 0) {
        intersection.intersects = false;
        return intersection;
    }
    
    double ua = (double)((x4 - x3)*(y1 - y3) - (y4 - y3)*(x1 - x3)) / denom;
    double ub = (double)((x2 - x1)*(y1 - y3) - (y2 - y1)*(x1 - x3)) / denom;
    
    // only consider intersections in middle 80% of both lines
    if(ua > 0.9 || ua < 0.1 || ub > 0.9 || ub < 0.1) {
        // lines don't intersect
        intersection.intersects = false;
        return intersection;
    }
    
    intersection.intersection.x = x1 + ua*(x2 - x1);
    intersection.intersection.y = y1 + ua*(y2 - y1);    
    intersection.intersects = true;

    return intersection;
}

bool valid_face_angles(Face* face) {
    if(!inside_face(face, face->cen)) {
        return false;
    }
    int dy11 = face->p2.y - face->p1.y;
    int dx11 = face->p2.x - face->p1.x;
    int dy12 = face->p3.y - face->p4.y;
    int dx12 = face->p3.x - face->p4.x;
    int dy21 = face->p3.y - face->p2.y;
    int dx21 = face->p3.x - face->p2.x;
    int dy22 = face->p4.y - face->p1.y;
    int dx22 = face->p4.x - face->p1.x;     
    int dy1 = (dy11 + dy12)/2;
    int dx1 = (dx11 + dx12)/2;
    int dy2 = (dy21 + dy22)/2;
    int dx2 = (dx21 + dx22)/2;
    
    int t11 = atan2(dy11*PI/180, dx11*PI/180) * 180 / PI;
    int t12 = atan2(dy12*PI/180, dx12*PI/180) * 180 / PI;
    int t21 = atan2(dy21*PI/180, dx21*PI/180) * 180 / PI;
    int t22 = atan2(dy22*PI/180, dx22*PI/180) * 180 / PI;
    

    if(t11 < 0) 
        t11 += 180;
    if(t12 < 0)
        t12 += 180;
    if(t21 < 0)
        t21 += 180;
    if(t22 < 0)
        t22 += 180;
    
    //printf("%d %d %d %d\n", t11, t12, t21, t22);
    
    // angles range from -180 -> 180
    // -179 is very similar to 179
    // so becareful when checking
    int t1 = atan2(dy1, dx1) * 180 / PI;
    int t2 = atan2(dy2, dx2) * 180 / PI;
    
    if(t1 < 0)
        t1 += 180;
    if(t1 >= 180)
        t1 -= 180;
    if(t2 < 0)
        t2 += 180;
    if(t2 >= 180)
        t2 += 180;
    
    /*
     if ang2 < ang1
     then shift points around
    
    
    if(t2 < t1) {
        CvPoint tmp;
        tmp = face->p1;
        face->p1 = face->p2;
        face->p2 = face->p3;
        face->p3 = face->p4;
        face->p4 = tmp;
        
        dy11 = face->p2.y - face->p1.y;
        dx11 = face->p2.x - face->p1.x;
        dy12 = face->p3.y - face->p4.y;
        dx12 = face->p3.x - face->p4.x;
        dy21 = face->p3.y - face->p2.y;
        dx21 = face->p3.x - face->p2.x;
        dy22 = face->p4.y - face->p1.y;
        dx22 = face->p4.x - face->p1.x;     
        dy1 = (dy11 + dy12)/2;
        dx1 = (dx11 + dx12)/2;
        dy2 = (dy21 + dy22)/2;
        dx2 = (dx21 + dx22)/2;

        t11 = atan2(dy11*PI/180, dx11*PI/180) * 180 / PI;
        t12 = atan2(dy12*PI/180, dx12*PI/180) * 180 / PI;
        t21 = atan2(dy21*PI/180, dx21*PI/180) * 180 / PI;
        t22 = atan2(dy22*PI/180, dx22*PI/180) * 180 / PI;
    }
    
    */
    
    face->ang1 = t1;
    face->ang2 = t2;
    
    // computer area as a simple estimation
    // use largest edge squared
    int length1 = sqrt(dy1*dy1 + dx1*dx1);
    int length2 = sqrt(dy2*dy2 + dx2*dx2);
    
    face->length1 = length1;
    face->length2 = length2;
    
    face->area = length1*length2;
    
    if(length1 < length2 * 0.2 || length2 < length1 * 0.8) {
        return false;
    }
    
    if((abs(t11 - t12) > 10 && abs(t11 - t12) < 170) || (abs(t21 - t22) > 10 && abs(t21 - t22) < 170)) {
        // lines are very different angles, probably not a face on the cube
        return false;
    }
    
    return true;
}

bool valid_face_size(CvSeq* faces, Face* face) {
    
}

void exclude_invalid_faces(CvSeq* faces) {
    int avg_area = average_face_area(faces);
    // compare each face to this avg area
    int i =0;
    while(i < faces->total) {
        Face* face = (Face*)cvGetSeqElem(faces, i);
        if(face->area > avg_area * 2.5 || face->area < avg_area * 0.6) {
            face = NULL;
            cvSeqRemove(faces, i);
        } else {
            i++;
        }
    }
}

int average_face_area(CvSeq* faces) {
    int total_area = 0;
    if(faces->total == 0) {
        return 0;
    }
    for(int i = 0; i < faces->total; i++) {
        Face* face = (Face*)cvGetSeqElem(faces, i);
        total_area += face->area;
    }
    
    int avg_area = (total_area / faces->total);
    return avg_area;
}

/**
Given a list of angle frequencies
group into 2 or 3 distinct sets of angle directions
*/
SeriesPeaks group_peaks(int *line_direction) {
    SeriesPeaks peak;
    /**
    Look at each peak on the line_direction graph
    Link it to its closest peak
    */
    

    
    /**
    reduce the resolution of the graph to 10 different directions
    */
    int *low_range_line_direction = new int[10];
    for(int i = 0; i < 10; i++) 
        low_range_line_direction[i] = 0;

    for(int i = 0; i < 180; i++) {
        low_range_line_direction[(int)(((double)i/(double)18.0)+0.5)] += line_direction[i];
    }
    
    
    for(int i = 0; i < 10; i++) {
        int iH = i+1, iL = i-1;
        if(iH == 10) iH = 0;
        if(iL < 0) iL = 9;
        if(low_range_line_direction[iH] > low_range_line_direction[i]) {
            low_range_line_direction[iH] += low_range_line_direction[i];
            low_range_line_direction[i] = 0;
        } else if(low_range_line_direction[iL] >= low_range_line_direction[i]) {
            low_range_line_direction[iL] += low_range_line_direction[i];
            low_range_line_direction[i] = 0;
        }
    }
    
    // count the number of peaks
    peak.num_peaks = 0;
    for(int i = 0; i < 10; i++) 
        if(low_range_line_direction[i] > 0) 
            peak.num_peaks++;

    
    // if peaks > 3 then choose 3 highest (hopefully wont happen)
    // find the average of each peak
    int *peaks = new int[9];
    int *peak_id = new int[3];
    int p = 0;
    for(int i = 0; i < 10; i++) 
        if(low_range_line_direction[i] > 0) {
            peaks[i] = p;
            peak_id[p] = i;
            p++;
        }
    
    // now assign each point to its closest low_range_line_direction
    int *closest_peak = new int[180];
    for(int i = 0; i < 180; i++) {
        if(line_direction[i] > 0) {
            int closest = 0;
            int closest_id = 0;
            int id = (int)((i/18)+0.5);
            int iL = id-1, iH = id+1;
            if(iL < 0) iL = 9;
            if(iH > 9) iH = 0;
            if(low_range_line_direction[id] > 0) {
                closest_peak[i] = id;
            } else if(low_range_line_direction[iL] > 0) {
                closest_peak[i] = iL;
            } else if(low_range_line_direction[iH] > 0) {
                closest_peak[i] = iH;
            }
        } else {
            closest_peak[i] = -1;
        }
        //printf("[%d] closest peak = %d\n", i, closest_peak[i]);
    }
    
    peak.peak_avg = new int[peak.num_peaks];
    for(int i = 0; i < peak.num_peaks; i++) {
        peak.peak_avg[i] = 0;
    }
    
    /**
    Need to cope with angles < 5 and > 175 being grouped together
    */
        
    for(int i = 0; i < 180; i++) {
        if(line_direction[i] > 0) {
            int peak_id = peaks[closest_peak[i]];
            
            // closest_peak is the rough peak centre
            int closest_peak_angle = closest_peak[i] * 18;
            //printf("closest_peak_angle = %d, angle = %d\n", closest_peak_angle, i);
            if(abs(closest_peak_angle - i) > 120) {
                // the graph wraps around
                if(closest_peak_angle < line_direction[i]) {
                    //the peak is near zero
                    // but this angle is around 180
                    // so make negative
                    peak.peak_avg[peak_id] += line_direction[i] * (i - 180);
                } else {
                    // peak is near 180
                    // but this angle is near zero
                    // so add 180
                    peak.peak_avg[peak_id] += line_direction[i] * (i + 180);
                }
            } else {
                peak.peak_avg[peak_id] += line_direction[i] * i;
            }
            
        }
    }
    
    for(int i = 0; i < peak.num_peaks; i++) {
        peak.peak_avg[i] = (int)(((double)peak.peak_avg[i] / (double)low_range_line_direction[peak_id[i]])+0.5);
        
        if(peak.peak_avg[i] < 0) {
            peak.peak_avg[i] += 180;
        } else if (peak.peak_avg[i] >= 180) {
            peak.peak_avg[i] -= 180;
        }
    }
    
    peak.closest_peak = new int[180];
    for(int i = 0; i < 180; i++) {
        if(line_direction[i] > 0) {
            peak.closest_peak[i] = peaks[closest_peak[i]];
        } else {
            peak.closest_peak[i] = -1;
        }
    }
    return peak;
}

int pt_dist_from_line(CvPoint pt1, CvPoint pt2, CvPoint point) {
    int x1 = pt1.x; int y1 = pt1.y;
    int x2 = pt2.x; int y2 = pt2.y;
    int x3 = point.x; int y3 = point.y;
    
    int A = x3 - x1;
    int B = y3 - y1;
    int C = x2 - x1;
    int D = y2 - y1;
    
    double denom = sqrt(C * C + D * D);
    
    if(denom == 0) {
        return 0;
    }
    
    int dist = fabs(A * D - C * B) / denom;
    return dist;
}

void colour_result(IplImage* cframe, CvSeq* faces) {
    for(int i = 0; i < faces->total; i++) {
        Face* face = (Face*)cvGetSeqElem(faces, i);
        cvLine( cframe, face->p1, face->p2, get_colour(face->colour), 2, 8 );
        cvLine( cframe, face->p2, face->p3, get_colour(face->colour), 2, 8 );
        cvLine( cframe, face->p3, face->p4, get_colour(face->colour), 2, 8 );
        cvLine( cframe, face->p4, face->p1, get_colour(face->colour), 2, 8 );
        
        cvCircle(cframe, face->cen, 3, get_colour(face->colour), 3);

        CvScalar dir1Colour, dir2Colour;
        switch(face->dir1) {
            case 0:
                dir1Colour = cvScalar(255, 0, 0);
                break;
            case 1:
                dir1Colour = cvScalar(0, 255, 0);
                break;
            case 2:
                dir1Colour = cvScalar(0, 0, 255);
                break;
        }
        switch(face->dir2) {
            case 0:
                dir2Colour = cvScalar(255, 0, 0);
                break;
            case 1:
                dir2Colour = cvScalar(0, 255, 0);
                break;
            case 2:
                dir2Colour = cvScalar(0, 0, 255);
                break;
        }

        if(face->side == 0) {
            dir1Colour = cvScalar(255, 0, 0);
            dir2Colour = cvScalar(255, 0, 0);
        } else if (face->side == 1) {
            dir1Colour = cvScalar(0, 255, 0);
            dir2Colour = cvScalar(0, 255, 0);
        } else {
            dir1Colour = cvScalar(0, 0, 255);
            dir2Colour = cvScalar(0, 0, 255);
        }
        /*
        cvLine( cframe, face->p1, face->p2, dir1Colour, 3, 8 );
        cvLine( cframe, face->p2, face->p3, dir2Colour, 3, 8 );
        cvLine( cframe, face->p3, face->p4, dir1Colour, 3, 8 );
        cvLine( cframe, face->p4, face->p1, dir2Colour, 3, 8 );
        */
        cvCircle(cframe, cvPoint(face->p1.x, face->p1.y), 3, CV_RGB(0,0,0), 3);
        cvCircle(cframe, cvPoint(face->p2.x, face->p2.y), 3, CV_RGB(0,0,0), 3);
        cvCircle(cframe, cvPoint(face->p3.x, face->p3.y), 3, CV_RGB(0,0,0), 3);
        cvCircle(cframe, cvPoint(face->p4.x, face->p4.y), 3, CV_RGB(0,0,0), 3);

    }
}

void print_faces(CvSeq* faces) {
    for(int i = 0; i < faces->total; i++) {
        Face* f = (Face*)cvGetSeqElem(faces, i);
        print_face(f->colour);
        printf(" at [%d, %d] (%d, %d) {%d, %d}->{%d, %d} -> {%d, %d} -> {%d, %d}\n", f->cen.x, f->cen.y, f->length1, f->length2,
            f->p1.x, f->p1.y,
            f->p2.x, f->p2.y,
            f->p3.x, f->p3.y,
            f->p4.x, f->p4.y
        );
    }
}

void find_colour(CvScalar hsv, Face* face) {
    int hue = hsv.val[0];
    int sat = hsv.val[1];
    int val = hsv.val[2];
    
    //printf("HSV = [%d %d %d] ", hue, sat, val);
    int r=0, g=0, b=0;
    if(val < 20) {
        face->colour = UNKNOWN;
    }
    else if( (sat < 20 && val > 100) || (sat < 65 && val > 120 && val < 150) || (val > 200 && sat < 130) || (val > 150 && val < 200 && sat < 150)) {
        face->colour = WHITE;
    } else if((hue < 9 || hue > 140) && ((val < 180 && sat < 200) || (val < 120))) {
        face->colour = RED;
    } else if( hue < 17) {
        face->colour = ORANGE;
    } else if ( hue < 40) {
        face->colour = YELLOW;
    } else if (hue < 100) {
        face->colour = GREEN;
    } else if (hue < 130) {
        face->colour = BLUE;
    } else {
        face->colour = UNKNOWN;
    }
    
    face->rgb = get_colour(face->colour);
}

CvScalar get_colour(int i) {
    int r, g, b;
    switch(i) {
        case WHITE:
            r = 255;
            g = 255;
            b = 255;
            break;
        case RED:
            r = 255;
            g = 0;
            b = 0;
            break;
        case ORANGE:
            r = 255;
            g = 150;
            b = 10;
            break;
        case YELLOW:
            r = 230;
            g = 230;
            b = 0;
            break;
        case GREEN:
            r = 0;
            g = 255;
            b = 0;
            break;
        case BLUE:
            r = 0;
            g = 0;
            b = 255;
            break;
    }
    
    return cvScalar(b, g, r);
}

bool compare_pixels(CvScalar target, CvScalar hsv) {
    int tHUE = target.val[0]; int cHUE = hsv.val[0];
    int tSAT = target.val[1]; int cSAT = hsv.val[1];
    int tINT = target.val[2]; int cINT = hsv.val[2];
    
    if(tHUE > 140) {
        tHUE = 0;
    }
    
    if(tHUE < 20 && cHUE > 140) {
        cHUE = 0;
    }
    
    if(tSAT < 20 && ((tINT > 200 && abs(tSAT - cSAT) < 30 && abs(tINT - cINT) < 10) || abs(tSAT - cSAT) < 10)
       ) {
        // if internsity is very high and saturation is very low
        // then the colour is probably white
        // so hue is irrelevant
        return true;
    }
    
    if((abs(tHUE - cHUE) < 10) && abs(tSAT - cSAT) < 50 && abs(tINT - cINT) < 30) {
        return true;
    }
    
    if(abs(tSAT - cSAT) < 5 && abs(tINT - cINT) < 5) {
        return true;
    }
    
    if(abs(tHUE - cHUE) < 5 && abs(tINT - cINT) < 5) {
        return true;
    }
    
    
    return false;
}

// wrap the other compare_pixels function
bool compare_pixels(CvScalar target, CvPoint* i) {
    int hue = ((uchar*)(HSV->imageData + HSV->widthStep*i->y))[i->x*3];
    int sat = ((uchar*)(HSV->imageData + HSV->widthStep*i->y))[i->x*3+1];
    int val = ((uchar*)(HSV->imageData + HSV->widthStep*i->y))[i->x*3+2];
    return compare_pixels(target, cvScalar(hue, sat, val));
}
