import time, os, sys, math
from machine import UART
from machine import FPIOA
from media.sensor import * #导入sensor模块，使用摄像头相关接口
from media.display import * #导入display模块，使用display相关接口
from media.media import * #导入media模块，使用meida相关接口
#串口设置
fpioa = FPIOA()
fpioa.set_function(11,FPIOA.UART2_TXD)
fpioa.set_function(12,FPIOA.UART2_RXD)
uart=UART(UART.UART2,115200) #设置串口号2和波特率

#固定代码
lcd_width=640
lcd_height=480
sensor=Sensor(width=1280,height=960)
sensor.reset()
#framesize数字越大，越显示的近，帧率越低
sensor.set_framesize(sensor.VGA)
sensor.set_pixformat(Sensor.GRAYSCALE)
#在mipi上显示那么frmesize需要匹配display显示
Display.init(Display.ST7701, width=lcd_width, height=lcd_height, to_ide=True)
MediaManager.init() #初始化media资源管理器
sensor.run()
clock=time.clock()
#以下是巡线关键代码
#黑线阈值
GRAYSCALE_THRESHOLD = [(0, 64)]
#采样图像为VGA 640*480，列表把roi把图像分成3个矩形，越靠近的摄像头视野（通常为图像下方）的矩形权重越大。
ROIS = [ # [ROI, weight]
        (0,420,640,60,0.7),
        (0,280,640,60,0.5), # 可以根据不同机器人情况进行调整。
        (0,140,640,60,0.3),
        (0,  0,640,60,0.1),

       ]
weight_sum = 0
for r in ROIS: weight_sum += r[4] # r[4] 为矩形权重值.
while True:
    clock.tick()
    img=sensor.snapshot()
    #权重值之和
    centroid_sum = 0
    for r in ROIS:
        #找到具体色块,找到当前roi矩形范围内的具体符合黑线的色块，其中合并相邻且重叠的色块
        blobs=img.find_blobs(GRAYSCALE_THRESHOLD,roi=r[0:4],merge=True)
        if blobs:
            #检测到符合条件的色块时，比较色块最多的地方，定义为主路径，作用为过滤阴影
            largest_blobs=max(blobs,key=lambda b: b.pixels())#lambda 参数:返回值
            #开始绘画矩形，和中心点十字箭头
            img.draw_rectangle(largest_blobs.rect())
            img.draw_cross(largest_blobs.cx(),
                           largest_blobs.cy())
            centroid_sum+=largest_blobs.cx()*r[4]
    center_pos = (centroid_sum / weight_sum) # 确定直线的中心.
    #角度转换
    angle=0
    angle = -math.atan((center_pos-320)/240) #采用图像为VGA时候使用
    angle = math.degrees(angle)#最终为具体角度
    #lcd显示角度值，左半屏为正值
    img.draw_string_advanced(2,2,20, str('%.1f' % angle), color=(255,255,255))
    #串口传递角度给单片机
    angle_int = int(angle * 100)  # 放大100倍转为整数
    uart.write(angle_int.to_bytes(2, 'little', True))  # 小端2字节
    print("Turn Angle: %f" % angle)
    Display.show_image(img) #显示图片
    print(clock.fps()) #打印FPS
