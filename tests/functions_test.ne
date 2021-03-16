
extern fun ftoi(float f) int

fun test(int arg, float hello) int {
    return arg + 10 - ftoi(hello)
}

fun main() int {
    int world = 6
    float hello = 1.0
    assert test(world, hello) == 15
    return 0
}
