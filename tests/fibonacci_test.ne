
fun fib(int num) int {
    if num == 1 {
        return 1
    }
    if num == 2 {
        return 1
    }
    return fib(num - 1) + fib(num - 2)
}

fun main() int {
    assert fib(1) == 1
    assert fib(2) == 1
    assert fib(3) == 2
    assert fib(4) == 3
    assert fib(5) == 5

    return 0
}
