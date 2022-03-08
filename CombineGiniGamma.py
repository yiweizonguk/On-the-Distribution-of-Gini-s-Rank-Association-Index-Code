import pandas as pd
import os
import numpy as np
# This code is to combine the calculated results from GiniGamma.cpp

def MySum(x):
    # sum large integers
    s = 0
    for a in x:
        s = s + int(a)
    return str(s)


def GetSumm(path, n):
    """
    combine the results on each cpu for the final results
    sum the polynomials output from each CPU
    :param path: the path of cpu output files
    :param n: the size of matrix A
    """

    resPath = os.path.join(path, str(n))
    D = int(n**2 / 2)

    # read the cpu output files, use string to represent large integers
    res = []
    for file in os.listdir(resPath):
        try:
            res.append(pd.read_csv(os.path.join(resPath, file), header=None, sep=" ", dtype={1: str}))
        except:
            print(file)
            pass
    res = pd.concat(res, axis=0)

    # compute the coef of each degree
    res.columns = ["Deg", "Coef"]
    res = res.groupby("Deg").apply(lambda x: MySum(x["Coef"]))
    res = res.to_frame(name="Count")
    res = res[res["Count"] != '0'].copy()
    res["Count"] = [str('%100.0f' % (int(x) / 2**(n-1))).replace(" ", "") for x in res["Count"]]
    res.index = [int(x + D) for x in res.index]

    # compute the probability and cum probability
    res["CumCount"] = [MySum(res["Count"].values[:(i+1)]) for i in range(len(res))]
    totalCount = MySum(res["Count"])
    res["Prob"] = [int(x) / int(totalCount) for x in res["Count"]]
    res["CDF"] = [int(x) / int(totalCount) for x in res["CumCount"]]
    res.index.name = "DGPulsD"
    res.reset_index(inplace=True)
    res["GiniGamma"] = res["DGPulsD"] / D - 1
    res.to_excel(os.path.join(path, f"{n}.xlsx"), index=False)
    return res


if __name__ == "__main__":
    path = "write your path here"
    GetSumm(path, n=1)
    GetSumm(path, n=2)
    GetSumm(path, n=3)
    GetSumm(path, n=4)
    GetSumm(path, n=5)
    GetSumm(path, n=6)
    GetSumm(path, n=7)
    GetSumm(path, n=8)
    GetSumm(path, n=9)
    GetSumm(path, n=10)
    GetSumm(path, n=11)
    GetSumm(path, n=12)
    GetSumm(path, n=13)
    GetSumm(path, n=14)
    GetSumm(path, n=15)
    GetSumm(path, n=16)
    GetSumm(path, n=17)
    GetSumm(path, n=18)
    GetSumm(path, n=19)
    GetSumm(path, n=20)
    GetSumm(path, n=21)
    GetSumm(path, n=22)
    GetSumm(path, n=23)
    GetSumm(path, n=24)
    GetSumm(path, n=25)
    GetSumm(path, n=26)
    GetSumm(path, n=27)
    GetSumm(path, n=28)