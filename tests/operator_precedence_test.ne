
fun main() int {
    int a = 5 + 7 * 9
    assert a == 68

    a = (5 + 7) * 9
    assert a == 108

    a = 5 + 7 * 9 + 2 * 5
    assert a == 78

    assert 7 < 8 and 9 >= 9

    return 0
}
