import json
import typing as tp

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

from datetime import datetime
from matplotlib.figure import Figure


class YouTube2:
    def __init__( # task0
        self,
        trends_df_path: str="RUvideos_short.csv",
        categories_df_path: str="RU_category_id.json"
    ):
        dp = lambda x: datetime.strptime(x, "%y.%d.%m")
        self.trends_df = pd.read_csv(trends_df_path, parse_dates=["trending_date"], date_parser=dp)

        with open(categories_df_path) as json_file:
            json_data = json.load(json_file)
          
        self.categories_df = pd.DataFrame(columns=['id', 'name'])

        for item in json_data['items']:
            self.categories_df = self.categories_df.append(
                {'id': int(item['id']),
                'name': item['snippet']['title']},
                ignore_index=True
            )
          
        self.categories_df['id'] = self.categories_df['id'].astype(int)

    def task1(self) -> pd.DataFrame:
        self.trends_df = self.trends_df.merge(self.categories_df, left_on='category_id', right_on='id')
        return self.trends_df
    

    def task2(self) -> pd.DataFrame:
        self.ptable = pd.pivot_table(self.trends_df, values='views', index='name', columns='trending_date', aggfunc=np.sum)
        return self.ptable


    def task3(self) -> Figure:
        plt.figure(figsize=(7, 7))
        axes = sns.heatmap(self.ptable * 1e-6, annot=True)
        plt.title('Распределение просмотров (в миллионах)')
        plt.tick_params(axis='x', labelrotation=30)
        labels = [item.get_text() for item in axes.get_xticklabels()]
        labels = map(lambda x: x.split('T')[0], labels)
        axes.set_xticklabels(labels) 
        return plt.gcf()


    def task4(self) -> pd.DataFrame:
        self.ptable = pd.pivot_table(self.trends_df, values='views', index='name', columns='trending_date', aggfunc=np.sum, margins=True, margins_name='Всего просмотров')
        return self.ptable

    def task5(self) -> Figure:
        plt.figure(figsize=(9, 9))
        axes = sns.heatmap(self.ptable * 1e-6, annot=True, vmax=15)
        plt.title('Распределение просмотров (в миллионах)')
        plt.tick_params(axis='x', labelrotation=30)
        labels = [item.get_text() for item in axes.get_xticklabels()]
        labels = map(lambda x: x.split(' ')[0], labels)
        axes.set_xticklabels(labels)
        axes.set_xlabel("Дата")
        axes.set_ylabel("Категория")
        plt.subplots_adjust(left=0.25, right=0.95, bottom=0.25, top=0.95)
        return plt.gcf()

yt = YouTube2()
print(yt.task1())
print(yt.task2())
yt.task3()
plt.savefig("task3.jpg")
print(yt.task4())
yt.task5()
plt.savefig("task5.jpg")
plt.show()

