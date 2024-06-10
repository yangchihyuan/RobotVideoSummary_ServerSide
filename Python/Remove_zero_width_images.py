#remove corrupt images
import pandas as pd
from tqdm import tqdm, trange
import cv2
import os
#import tensorflow as tf
from skimage import io

csv_file = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/csv_files/good.csv"

df = pd.read_csv(csv_file, index_col="filename")
number_of_rows = df.shape[0]
print(number_of_rows)
number_of_repeated = 0 
list_good = []
list_zero_width_height = []
for i in trange(number_of_rows):
    if df.iloc[i]["width"] > 0 and df.iloc[i]["height"] > 0:
        list_good.append(i)
    else:
        list_zero_width_height.append(i)

print("number_of_good",len(list_good))
print("number_of_zero_width_height",len(list_zero_width_height))

df_good_file = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/csv_files/good2.csv"
df_good = df.iloc[list_good]
df_good.to_csv(df_good_file)
