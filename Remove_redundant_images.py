#remove redundant images
import pandas as pd
from tqdm import tqdm, trange
import cv2
import os

csv_file = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/bounding_boxes/merged.csv"
raw_image_directory = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/raw_images"

df = pd.read_csv(csv_file)
number_of_rows = df.shape[0]
print(number_of_rows)
number_of_repeated = 0 
list_repeated = []
list_unique = [0]
for i in trange(number_of_rows-1):
    bool_repeated = False
    if df.iloc[i+1]['x'] == df.iloc[i]['x'] and df.iloc[i+1]['y'] == df.iloc[i]['y'] and df.iloc[i+1]['width'] == df.iloc[i]['width'] and df.iloc[i+1]['height'] == df.iloc[i]['height']:
        image_i = cv2.imread(os.path.join(raw_image_directory,str(df.iloc[i]['filename']))+".jpg")
        image_iplus1 = cv2.imread(os.path.join(raw_image_directory,str(df.iloc[i+1]['filename']))+".jpg")
        difference = cv2.subtract(image_i, image_iplus1)
        b, g, r = cv2.split(difference)
        if cv2.countNonZero(b) == 0 and cv2.countNonZero(g) == 0 and cv2.countNonZero(r) == 0:
            number_of_repeated = number_of_repeated+1
            dict1 = {'filename': df.iloc[i+1]['filename']}
            list_repeated.append(dict1)
            bool_repeated = True

    if bool_repeated == False:
        list_unique.append(i+1)

print("number_of_repeated",number_of_repeated)
print("number_of_unique",len(list_unique))
output_file = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/repeated.csv"
df_repeated = pd.DataFrame(list_repeated)
df_repeated.to_csv(output_file)

df_unique_file = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/bounding_boxes/unique.csv"
df_unique = df.iloc[list_unique]
df_unique.to_csv(df_unique_file)
