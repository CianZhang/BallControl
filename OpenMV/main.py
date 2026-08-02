import sensor, image, time
import ustruct
from pyb import Servo, LED, Pin, UART

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.VGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking
sensor.set_auto_exposure(False,exposure_us=50000) # 曝光速度

last_points_x = 0
last_points_y = 0
dx = 0
dy = 0

clock = time.clock()

uart = UART(1,115200) # 定义串口1变量，波特率115200
uart.init(115200, bits=8, parity=None, stop=1) # 无检查位，一个停止位

roi = (110,30,420,420)

def preprocess_image(img):
    #img.mean(1) # 均值滤波，参数为滤波核大小
    #img.gaussian(1) # ⾼斯滤波
    #img.median(3) # 中值滤波
    return img

def find_roi(img):
    rects = find_rects([(0,130)],pixels_threshold = 18000)
    if rects:
        for rect in rects:
            if rect.w()*rect.h()>18000:
               roi = (rect.x(),rect.y(),rect.w(),rect.h())
while(True):
    clock.tick()
    img = sensor.snapshot().lens_corr(1.6)
    #img = preprocess_image(img)
    blobs = img.find_blobs([(0,100,-128,127,30,127)],roi =roi)
    if blobs:
        for blob in blobs:
            dx = blob.cx() - last_points_x
            dy = blob.cy() - last_points_y
            last_points_x = blob.cx()
            last_points_y = blob.cy()
            img.draw_cross(320,240,10,thickness = 1)
            img.draw_rectangle(roi,color = 10,thickness = 1)
            img.draw_circle(blob.cx(),blob.cy(),10,color = 10)
            #img.draw_cross(c.cx(),c.cy(),10,thickness = 1)
            if dx >= 0:
                if dy >= 0:
                    data = ustruct.pack("bbhhhhbbb",
                                        0xAA,
                                        0xAE,
                                        int(blob.cx()),
                                        int(blob.cy()),
                                        int(dx),
                                        int(dy),
                                        0xB1, # dx>=0
                                        0xC1, # dy>=0
                                        0xAC)
                else:
                    dy = -dy
                    data = ustruct.pack("bbhhhhbbb",
                                        0xAA,
                                        0xAE,
                                        int(blob.cx()),
                                        int(blob.cy()),
                                        int(dx),
                                        int(dy),
                                        0xB1, # dx>=0
                                        0xC2, # dy<0
                                        0xAC)
            else:
                dx = -dx
                if dy >= 0:
                    data = ustruct.pack("bbhhhhbbb",
                                        0xAA,
                                        0xAE,
                                        int(blob.cx()),
                                        int(blob.cy()),
                                        int(dx),
                                        int(dy),
                                        0xB2, # dx<0
                                        0xC1, # dy>=0
                                        0xAC)
                else:
                    dy = -dy
                    data = ustruct.pack("bbhhhhbbb",
                                        0xAA,
                                        0xAE,
                                        int(blob.cx()),
                                        int(blob.cy()),
                                        int(dx),
                                        int(dy),
                                        0xB2, # dx<0
                                        0xC2, # dy<0
                                        0xAC)
            uart.write(data)
            print(blob.cx(),blob.cy(),dx,dy)
    #print(clock.fps())
    img.binary([(0,100,-128,127,30,127)])
