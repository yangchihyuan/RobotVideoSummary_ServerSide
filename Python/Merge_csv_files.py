from os import listdir
import os
import pandas as pd
from tqdm import tqdm

source_file = ['0312.csv','0503_lab.csv']
csv_directory = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/csv_files"
output_file = 'merged.csv'
dtype = {"label":str}
df0 = pd.read_csv(os.path.join(csv_directory,source_file[0]),dtype=dtype)
df1 = pd.read_csv(os.path.join(csv_directory,source_file[1]),dtype=dtype)
frames = [df0,df1]
df_merge = pd.concat(frames,ignore_index=True)
df_merge.to_csv(os.path.join(csv_directory, output_file),index=False)
