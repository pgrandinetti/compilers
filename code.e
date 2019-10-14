readInt N;
totSum = 0;
i = 0;
while (i <= N)
    j = 2;
    isPrime = True;
    while ((j <= i / 2) && isPrime)
        if (i % j == 0)
            writeOut "%s is not prime", i;
            isPrime = False;
        ;
        j = j + 1;
    ;
    if (isPrime)
        totSum = totSum + i;
    ;
    i = i + 1;
;
writeOut "Total primes sum until %s is %s", N, totSum;
