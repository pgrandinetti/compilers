readIn N;
current = 2;
fib = [1, 1];
while (current < N)
    new = fib[0] + fib[1];
    fib = [fib[1], new];
    current = current + 1;
;
writeOut "The @th number in Fibonacci series is @", N, fib[1];

