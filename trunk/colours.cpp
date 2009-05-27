#include "cubefinder.h"
#include "colours.h"


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
