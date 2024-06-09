#Check labels and raw images
import pandas as pd
from tqdm import tqdm, trange
import cv2
import os
from skimage import io
import shutil


work_diretory = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/" 
csv_file = os.path.join(work_diretory,"csv_files/0503_lab.csv")
image_label_check_directory = os.path.join(work_diretory, "image_label_check")
positive_directory = os.path.join( image_label_check_directory, "positive")
negative_directory = os.path.join( image_label_check_directory, "negative")

positive_files = [f for f in os.listdir(positive_directory) if os.path.isfile(os.path.join(positive_directory, f))]
negative_files = [f for f in os.listdir(negative_directory) if os.path.isfile(os.path.join(negative_directory, f))]

df = pd.read_csv(csv_file, dtype={"filename": str, 'index': int, 'x': int, 'y': int, 'width': int, 'height': int})
number_of_rows = df.shape[0]
for i in trange(number_of_rows):
    filename = df.loc[i,'filename'] + '_' + str(df.loc[i,'index']) + '.jpg'
    if filename in positive_files:
        df.loc[i,'label'] = "1"
    elif filename in negative_files:
        df.loc[i,'label'] = "0"

df.to_csv(csv_file, index=False)
