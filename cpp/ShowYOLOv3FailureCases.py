from pydarknet import Detector, Image
import cv2
import os
from tqdm import tqdm, trange
import pandas as pd

if __name__ == "__main__":
    # net = Detector(bytes("cfg/densenet201.cfg", encoding="utf-8"), bytes("densenet201.weights", encoding="utf-8"), 0, bytes("cfg/imagenet1k.data",encoding="utf-8"))

    work_diretory = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/" 
    raw_image_directory = os.path.join(work_diretory, "raw_images")

    csv_file = os.path.join(work_diretory,"csv_files/merged.csv")
    YOLO_3_4_Py_directory = "/4t/yangchihyuan/YOLO3-4-Py"
    rendered_image_directory = os.path.join(work_diretory,"rendered_images/YOLOv3FailureCase")
 
    net = Detector(bytes(os.path.join(YOLO_3_4_Py_directory,"cfg/yolov3.cfg"), encoding="utf-8"), bytes(os.path.join(YOLO_3_4_Py_directory,"weights/yolov3.weights"), encoding="utf-8"), 0, bytes(os.path.join(YOLO_3_4_Py_directory,"cfg/coco.data"),encoding="utf-8"))

    df = pd.read_csv(csv_file,dtype={'filename':str})
    number_of_rows = df.shape[0]
    print(number_of_rows)
    for i in trange(number_of_rows):
        filename = df.loc[i,'filename']
        file_path = os.path.join(raw_image_directory,filename+".jpg")
        img = cv2.imread(file_path)
        img2 = Image(img)
        x = df.loc[i,'x']
        y = df.loc[i,'y']
        width = df.loc[i,'width']
        height = df.loc[i,'height']
        index = df.loc[i,'index']
        cv2.rectangle(img, (x, y), (x+width-1, y+height-1), (0, 0, 255), thickness=2)
        label = df.loc[i,'label']
        results = net.detect(img2)
#    print(results)
#        print(["x", x, "y", y, "width", width, "height", height])
        bFoundPerson = False
        for cat, score, bounds in results:
            yolo_center_x, yolo_center_y, w, h = bounds
            if str(cat.decode("utf-8")) == "person":
                bFoundPerson = True
#                print(["yolo_center_x", yolo_center_x, "yolo_center_y", yolo_center_y])
                cv2.rectangle(img, (int(yolo_center_x - w / 2), int(yolo_center_y - h / 2)), (int(yolo_center_x + w / 2), int(yolo_center_y + h / 2)), (255, 0, 0), thickness=2)
#                if yolo_center_x - w/2 <= x and x <= yolo_center_x + w/2 and yolo_center_y -h/2 <= y and yolo_center_y <=y+height:
#                    df.loc[i,'label'] = "1"
            cv2.rectangle(img, (int(yolo_center_x - w / 2), int(yolo_center_y - h / 2)), (int(yolo_center_x + w / 2), int(yolo_center_y + h / 2)), (255, 0, 0), thickness=2)
            cv2.putText(img,str(cat.decode("utf-8")),(int(yolo_center_x),int(yolo_center_y)),cv2.FONT_HERSHEY_COMPLEX,1,(255,255,0))
        if label == 1 and bFoundPerson == False:
            cv2.imwrite(os.path.join(rendered_image_directory,filename+"_"+str(index)+".jpg"),img)
