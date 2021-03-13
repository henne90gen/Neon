def main():
    s0 = ""
    for i in range(50_000):
        s1 = "Hello World!"
        s2 = " "
        s3 = "Goodbye World!"
        s4 = " This is nuts!"
        s0 = s0 + s1 + s2 + s3 + s4
    print(s0)


if __name__ == "__main__":
    main()

# 10_000
# python  0.18s user  0.26s system 92% cpu  0.475 total
# ./main  0.58s user  0.38s system 97% cpu  0.985 total
# ./main  1.30s user  2.07s system 98% cpu  3.427 total

# 20_000
# python  0.58s user  1.27s system 97% cpu  1.901 total
# ./main  2.09s user  2.01s system 98% cpu  4.151 total

# 50_000
# python  3.55s user  8.71s system 99% cpu 12.340 total
# ./main 19.72s user 17.44s system 99% cpu 37.380 total
