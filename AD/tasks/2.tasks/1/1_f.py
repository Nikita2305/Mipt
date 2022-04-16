import pandas as pd
import matplotlib.pyplot as plt
import typing as tp
from matplotlib.axes import Axes

class CatExam:
    def __init__(self, path_to_df: str="cat_exam_data.csv"): # task0
        self.df = pd.read_csv(path_to_df)

    def task1(self) -> pd.DataFrame:
        return self.df.head(5)
    
    def task2(self) -> tp.List[str]:
        series = self.df.isna().sum()
        return list(series[series > 0].keys())

    def task3(self) -> pd.DataFrame:
        self.df = self.df.dropna()
        return self.df
    
    def task4(self) -> pd.DataFrame:
        return self.df.describe()
    
    def task5(self) -> int:
        return len(self.df[self.df['test_score'] == 100])
    
    def task6(self) -> pd.DataFrame:
        df = self.df[self.df['test_score'] == 100].groupby(['school', 'number_of_students'], as_index=False).count().rename(columns={'test_score': 'cnt_100'}, inplace=False).sort_values(by='cnt_100',ascending=False).reset_index()
        del df['index']
        return df

    def task7(self) -> pd.DataFrame:
        return self.df.groupby(['school', 'number_of_students'], as_index=False).mean().sort_values(by='test_score', ascending=False).reset_index().reindex(columns=['school', 'test_score', 'number_of_students'])[:10]
    
    def task8(self) -> pd.DataFrame:
        return self.df.groupby(['school', 'number_of_students'], as_index=False).mean().sort_values(by='test_score', ascending=True).reset_index().reindex(columns=['school', 'test_score', 'number_of_students'])[:10]

    def task9(self) -> Axes:
        plt.hist(self.df[self.df.number_of_students > 1000]['test_score'].values, bins=10, alpha=0.5, label='Huge schools')
        plt.hist(self.df[self.df.number_of_students <= 1000]['test_score'].values, bins=10, alpha=0.5, label='Small schools')
        plt.legend(loc='upper left')
        plt.title('Grades distribution')
        plt.xlabel('Grade')
        plt.ylabel('Frequency')
        return plt.gca()
