#Check labels and raw images
import pandas as pd
from tqdm import tqdm, trange
import cv2
import os
from skimage import io
import shutil


work_diretory = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/" 
csv_file = os.path.join(work_diretory,"csv_files/0503_lab_labeled.csv")
raw_image_directory = os.path.join(work_diretory, "raw_images")
image_label_check_directory = os.path.join(work_diretory, "image_label_check")
positive_directory = os.path.join( image_label_check_directory, "positive")
negative_directory = os.path.join( image_label_check_directory, "negative")
no_label_directory = os.path.join( image_label_check_directory, "no_label")

shutil.rmtree(image_label_check_directory)
os.makedirs(positive_directory, exist_ok=True)
os.makedirs(negative_directory, exist_ok=True)
os.makedirs(no_label_directory, exist_ok=True)

df = pd.read_csv(csv_file, dtype={'filename':str, 'index': int, 'x': int, 'y': int, 'width': int, 'height': int})
number_of_rows = df.shape[0]
for i in trange(number_of_rows):
    filename = df.loc[i,'filename']+".jpg"
    x = df.loc[i,'x']
    y = df.loc[i,'y']
    width = df.loc[i,'width']
    height = df.loc[i,'height']
    index = df.loc[i,'index']
    filename_output = df.loc[i,'filename'] + '_' + str(index) + ".jpg"
    image = cv2.imread(os.path.join(raw_image_directory,filename))
    cv2.rectangle(image, (x,y), (x+width-1,y+height-1), (0,0,255), 3)
    if df.iloc[i]['label'] == 0:
        target_directory = negative_directory
    elif df.iloc[i]['label'] == 1:
        target_directory = positive_directory
    else:
        target_directory = no_label_directory
    cv2.imwrite(os.path.join(target_directory,filename_output),image)

