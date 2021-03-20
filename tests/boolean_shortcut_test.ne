
fun never_call() bool {
    assert false
    return false
}

fun main() int {
    assert true or never_call()

    return 0
}
