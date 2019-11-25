#split data to training and evaluation sets
import pandas as pd
import random
import math

csv_file = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/csv_files/merged.csv"
df = pd.read_csv(csv_file,index_col="filename")
number_of_rows = df.shape[0]
#80% as training, 20% as evaluation
number_of_training = math.ceil(number_of_rows * 0.8)
list = list(range(0,number_of_rows))
random.shuffle(list)
training_set = list[0:number_of_training]
validation_set = list[number_of_training:]
print("number_of_rows", number_of_rows)
print("len(training_set)", len(training_set))
print("len(validation_set)", len(validation_set))
df_training = df.iloc[training_set]
df_validation = df.iloc[validation_set]
df_training.to_csv("/4t/yangchihyuan/TransmittedImages/ShuffleNet/csv_files/training.csv")
df_validation.to_csv("/4t/yangchihyuan/TransmittedImages/ShuffleNet/csv_files/validation.csv")