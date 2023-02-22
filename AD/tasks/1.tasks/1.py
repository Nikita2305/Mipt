import numpy as np
import typing as tp

def matrix_multiplication(A: np.ndarray, B: np.ndarray) -> np.ndarray:    
    return  

x = np.ndarray((2,2))
x[0][0] = 0
x[0][1] = 1
x[1][0] = 2
x[1][1] = 3
import copy
y = copy.copy(x)
y[0][1] = 2
y[1][0] = 1
print(x, "\n", y)
print(x.flatten('C'))
print(y.flatten('F'))
