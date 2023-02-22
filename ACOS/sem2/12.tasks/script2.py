import primes

for i in range(1, 100):
    print(i, primes.factor_out(i))

try:
    print(primes.factor_out(-1))
except Exception as ex:
    print(type(ex))

try:
    print(primes.factor_out(0.1))
except Exception as ex:
    print(type(ex))


