import numpy as np
import typing as tp


def find_nearest_points(A: np.ndarray, B: np.ndarray, k: int) -> np.ndarray:
    A = np.expand_dims(A, 0)
    B = np.expand_dims(B, 1)
    C = np.square(A - B)
    C = np.sum(C, axis=2)
    C = np.sqrt(C)
    C = C.argsort()
    C = C[:,:k]
    return C + 1
