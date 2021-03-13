
fun main() int {
    int[3] a
    assert a[0] == 0
    assert a[1] == 0
    assert a[2] == 0

    a[0] = 5
    assert a[0] == 5

    a[1] = a[0]
    assert a[1] == 5

    return 0
}
