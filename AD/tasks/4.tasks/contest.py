import numpy as np
import matplotlib.pyplot as plt
# -- В прод:

# from coin import coin, uniform_pdf, norm_pdf, expon_pdf

# -- Для запуска локально:

import scipy.stats as sps

coin = sps.bernoulli(p=0.5).rvs
uniform_pdf = sps.uniform.pdf
norm_pdf = sps.norm.pdf
expon_pdf = sps.expon.pdf


def uniform(size=1, precision=30):
    shape = (precision, *((size,) if isinstance(size, int) else size))
    return (coin(size=shape) * np.logspace(np.ones(size), np.ones(size)*precision, num=precision, base=0.5)).sum(0)


def plot_uniform_density(size=200):
    values = uniform(size)
    fig = plt.figure(figsize=(9, 9))
    ax = plt.axes()
    ax.set_title("Равномерное распределение")
    grid = np.linspace(-0.25, 1.25, 500)
    ax.hist(values, bins=10, density=True, alpha=0.4, label='Гистограмма выборки', color='blue')
    ax.scatter(x=values, y=[0]*size, alpha=0.4, label='Распределение', color='yellow', s=75)
    ax.plot(grid, uniform_pdf(grid), label='Плотность равномерного распр.', lw=2, color='red')
    ax.legend(fontsize=14, loc=1)
    return plt.gcf()


def plot_uniform_different_precision(size=100):
    fig = plt.figure(figsize=(15, 3))
    plt.suptitle("Равн. распределение с разной точностью", fontsize=18)
    
    for i, precision in enumerate([1, 2, 3, 5, 10, 30]):
        ax = plt.subplot(3, 2, i + 1)
        ax.set_ylabel(f"Точность {precision}", rotation=0, labelpad=50)
        
        plt.scatter(
          uniform(size, precision),
          np.zeros(size),
          alpha=0.4
        )

        plt.yticks([])
        if i < 4:
            plt.xticks([])

    plt.subplots_adjust(left=0.1, wspace=0.3)

    return plt.gcf()


def normal(size=1, loc=0, scale=1, precision=30):
    norm = np.sin(2 * np.pi * uniform(size, precision)) * np.sqrt(-2 * np.log(uniform(size, precision)))
    return loc + scale * norm


def plot_normal_density(size=200):
    values = normal(size)
    fig = plt.figure(figsize=(9, 9))
    ax = plt.axes()
    ax.set_title("Нормальное распределение")
    grid = np.linspace(-3, 3, 500)
    ax.hist(values, bins=10, density=True, alpha=0.4, label='Гистограмма выборки', color='blue')
    ax.scatter(x=values, y=[0]*size, alpha=0.4, label='Распределение', color='yellow', s=75)
    ax.plot(grid, norm_pdf(grid), label='Плотность нормального распр.', lw=2, color='red')
    ax.legend(fontsize=14, loc=1)
    return plt.gcf()


def expon(size=1, lambd=1, precision=30):
    f = lambda x: -np.log(-x+1)/lambd
    return f(uniform(size, precision))


def plot_expon_density(size=100):
    values = expon(size)
    fig = plt.figure(figsize=(9, 9))
    ax = plt.axes()
    ax.set_title("Экспоненциальное распределение")
    grid = np.linspace(-0.5, 5, 500)
    ax.hist(values, bins=10, density=True, alpha=0.4, label='Гистограмма выборки', color='blue')
    ax.scatter(x=values, y=[0]*size, alpha=0.4, label='Распределение', color='yellow', s=75)
    ax.plot(grid, expon_pdf(grid), label='Плотность экспоненциального распр.', lw=2, color='red')
    ax.legend(fontsize=14, loc=1)
    return plt.gcf()

plot_uniform_density().show()
plot_uniform_different_precision().show()
plot_normal_density().show()
plot_expon_density().show() 
plt.show()       
