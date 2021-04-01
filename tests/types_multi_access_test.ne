
type MyType {
    int i
}

type AnotherType {
    int j
    string s
    MyType hello
}

fun main() int {
    AnotherType a = AnotherType()
    assert a.j == 0
    assert a.hello.i == 0

    a.hello.i = 5
    assert a.hello.i == 5

    return 0
}
