
type MyType {
    int i
}

fun main() int {
    MyType t = MyType()

    t.i = 5
    assert t.i == 5

    t.i = t.i + t.i
    assert t.i == 10

    return 0
}
