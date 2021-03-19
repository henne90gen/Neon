
type MyType {
    int i

    fun hello() int {
        return i + 10
    }
}

fun main() int {
    MyType t = MyType()
    t.i = 5
    assert t.i == 5

    return 0
}
