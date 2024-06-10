from pydarknet import Detector, Image
import cv2
import os
from tqdm import tqdm, trange
import pandas as pd

if __name__ == "__main__":
    # net = Detector(bytes("cfg/densenet201.cfg", encoding="utf-8"), bytes("densenet201.weights", encoding="utf-8"), 0, bytes("cfg/imagenet1k.data",encoding="utf-8"))

    work_diretory = "/4t/yangchihyuan/TransmittedImages/ShuffleNet/" 
    raw_image_directory = os.path.join(work_diretory, "raw_images")

    csv_file = os.path.join(work_diretory,"csv_files/0503_lab_YOLO_check.csv")
    csv_output_file = os.path.join(work_diretory,"csv_files/0503_lab_YOLO_check_output.csv")
    YOLO_3_4_Py_directory = "/4t/yangchihyuan/YOLO3-4-Py"
    rendered_image_directory = os.path.join(work_diretory,"rendered_images")
 
    net = Detector(bytes(os.path.join(YOLO_3_4_Py_directory,"cfg/yolov3.cfg"), encoding="utf-8"), bytes(os.path.join(YOLO_3_4_Py_directory,"weights/yolov3.weights"), encoding="utf-8"), 0, bytes(os.path.join(YOLO_3_4_Py_directory,"cfg/coco.data"),encoding="utf-8"))

    df = pd.read_csv(csv_file,dtype={'filename':str})
    number_of_rows = df.shape[0]
    print(number_of_rows)
    for i in trange(number_of_rows):
        file_path = os.path.join(raw_image_directory,df.iloc[i]['filename'])+".jpg"
        img = cv2.imread(file_path)
        img2 = Image(img)
        x = df.iloc[i]['x']
        y = df.iloc[i]['y']
        width = df.iloc[i]['width']
        height = df.iloc[i]['height']
        index = df.iloc[i]['index']
        cv2.rectangle(img, (x, y), (x+width-1, y+height-1), (0, 0, 255), thickness=2)

        # r = net.classify(img2)
        results = net.detect(img2)
#    print(results)
#        print(["x", x, "y", y, "width", width, "height", height])
        for cat, score, bounds in results:
            yolo_center_x, yolo_center_y, w, h = bounds
            if str(cat.decode("utf-8")) == "person":
#                print(["yolo_center_x", yolo_center_x, "yolo_center_y", yolo_center_y])
                cv2.rectangle(img, (int(yolo_center_x - w / 2), int(yolo_center_y - h / 2)), (int(yolo_center_x + w / 2), int(yolo_center_y + h / 2)), (255, 0, 0), thickness=2)
                if yolo_center_x - w/2 <= x and x <= yolo_center_x + w/2 and yolo_center_y -h/2 <= y and yolo_center_y <=y+height:
                    df.loc[i,'label'] = "1"
#                    print("label as 1")
                else:
                    df.loc[i,'label'] = "0"
#                    print("label as 0")
            cv2.rectangle(img, (int(yolo_center_x - w / 2), int(yolo_center_y - h / 2)), (int(yolo_center_x + w / 2), int(yolo_center_y + h / 2)), (255, 0, 0), thickness=2)
            cv2.putText(img,str(cat.decode("utf-8")),(int(yolo_center_x),int(yolo_center_y)),cv2.FONT_HERSHEY_COMPLEX,1,(255,255,0))

#                cv2.imshow("output", img)
    # img2 = pydarknet.load_image(img)

#                cv2.waitKey(0)
        cv2.imwrite(os.path.join(rendered_image_directory,df.iloc[i]['filename']+"_"+str(index)+".jpg"), img)

    df.to_csv(csv_output_file, index=False)