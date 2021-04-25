/*
Доказательство формулы log(n!) = O(n logn)
Очев. log(n!) = log(1) + .. + log(n) <= nlog(n)
С другой стороны log(n!) >= log(n * 1) + log((n-1)*2) + ... + log(n/2 * (n/2 + 1)) >= log(n) * n/2 >= 1/2 * nlog(n)
*/
