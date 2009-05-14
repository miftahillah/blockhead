bool compare_pixels(CvScalar target, CvPoint* i);
bool compare_pixels(CvScalar target, CvScalar hsv);
void find_colour(CvScalar hsv);

// define colours such that those opposite are 3 spaces apart
/*Green -> blue
Red -> orange
yellow -> white
*/
#define RED 0
#define YELLOW 1
#define GREEN 2
#define ORANGE 3
#define WHITE 4
#define BLUE 5

#define UNKNOWN 6