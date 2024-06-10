#remove corrupt images
import pandas as pd
from tqdm import tqdm, trange
import cv2
import os
from PIL import Image

list_file = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/filelist/0503_lab_all.txt"
raw_image_directory = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/raw_images"

with open(list_file ,'r') as f:
    content = f.readlines()
file_list = [x.strip() for x in content] #remove \n

number_of_rows = len(file_list)
print(number_of_rows)
#number_of_repeated = 0 
#list_corrupt = []
#list_good = []
for i in trange(number_of_rows):
#    bool_corrupt = False
    file_path = os.path.join(raw_image_directory,file_list[i])
    img = cv2.imread(file_path)
#    cv2.imshow("read file",img)
#    cv2.waitKey(1)    
#    image_file = tf.io.read_file(file_path)

#    try:
#        img = Image.open(file_path)
#    except IOError:
#        print(df.iloc[i]['filename'])
#    try:
#        img= np.array(img, dtype=np.float32)
#    except :
#        print('corrupt img',file_path)

#    img_cv = cv2.imread(file_path)
#    try:
#    img = tf.image.decode_jpeg(image_file, channels=3)
#        io.imread(file_path)
#        list_good.append(i)
#    except Exception as e:
#        print(e)
#        list_corrupt.append(i)

#print("number_of_good",len(list_good))
#print("number_of_corrupt",len(list_corrupt))

#df_good_file = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/csv_files/good2.csv"
#df_good = df.iloc[list_good]
#df_good.to_csv(df_good_file)
