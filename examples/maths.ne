
extern fun sin(float x) float
extern fun cos(float x) float
extern fun tan(float x) float
extern fun asin(float x) float
extern fun acos(float x) float
extern fun atan(float x) float

extern fun floor(float x) int
extern fun ceil(float x) int

fun toInt(float x) int {
    return floor(x)
}
