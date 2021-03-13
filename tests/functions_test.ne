
fun test(int arg) int {
    return arg + 10
}

fun main() int {
    int world = 6
    assert test(world - 1) == 15
    return 0
}
