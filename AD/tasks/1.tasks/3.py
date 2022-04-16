import numpy as np
import typing as tp

def find_max_sum_segment(array: tp.List[int], k: int) -> int:    
    A = np.tile(array, len(array)).reshape((len(array), len(array)))
    A = np.triu(A)
    A = np.tril(A, k - 1)
    A = np.sum(A,axis=-1)
    A = A[:(-(k - 1) if k > 1 else len(A))]
    return A.max()

print(find_max_sum_segment([1,2,3,4],1))
