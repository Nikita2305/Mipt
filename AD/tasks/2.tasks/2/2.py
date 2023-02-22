from datetime import datetime
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from matplotlib.figure import Figure
import typing as tp


class YouTube:
    def __init__(self, path_to_df: str = "RUvideos_short.csv"):
        dp = lambda x: datetime.strptime(x, "%y.%d.%m")
        self.df = pd.read_csv(path_to_df, parse_dates=["trending_date"], date_parser=dp)

    def task1(self) -> pd.DataFrame:
        return self.df

    def task2(self) -> pd.DataFrame:
        self.df = self.df[["trending_date", "category_id", "views", "likes", "dislikes", "comment_count"]]
        self.df["trending_date"] = self.df["trending_date"].apply(lambda x: x.day)
        return self.df

    def task3(self) -> Figure:
        with sns.axes_style('whitegrid'):
            plt.figure(figsize=(9, 9)) 
            plt.title("Views distribution")
            sns.boxplot(data=self.df, x='trending_date', y='views')
        return plt.gcf()

    def task4(self) -> Figure:
        with sns.axes_style('whitegrid'):
            plt.figure(figsize=(7, 7))
            plt.title("Распределение просмотров без выбосов")
            ax = sns.boxplot(data=self.df, x='trending_date', y='views', showfliers=False, showmeans=True) 
            ax.set_xlabel("Дата, ноябрь 2017")
            ax.set_ylabel("Просмотры")
            ax.set_yticks(np.arange(0, 500000, 25000))
        return plt.gcf()

    def task5(self) -> Figure:
        with sns.axes_style('whitegrid'): 
            p = sns.jointplot(data=self.df, x='views', y='likes', kind='scatter')
            p.fig.suptitle("Views/likes dependence")
        return plt.gcf()

    def task6(self) -> Figure:
        df = self.df
        X = df['views']
        df.drop(df[X > X.quantile(.95)].index, inplace=True)
        Y = df['likes']
        df.drop(df[Y > Y.quantile(.95)].index, inplace=True)
        with sns.axes_style('whitegrid'): 
            p = sns.jointplot(data=df, x='views', y='likes', kind='reg', height=7)
            p.fig.suptitle("Зависимость просмотров и лайков")
            p.fig.axes[0].set_yticks(np.arange(0, 30000, 5000))
            ax = p.fig.axes[0]
            ax.set_xlabel("Просмотры")
            ax.set_ylabel("Лайки")
            ax.set_yticklabels(ax.get_yticks(), rotation = 75)
        return plt.gcf()

yt = YouTube()
print(yt.task1().head(5))
print(yt.task2().head(5))
yt.task3()
plt.savefig("task3.jpg")
yt.task4()
plt.savefig("task4.jpg")
yt.task5()
plt.savefig("task5.jpg")
yt.task6()
plt.savefig("task6.jpg")
plt.show()
