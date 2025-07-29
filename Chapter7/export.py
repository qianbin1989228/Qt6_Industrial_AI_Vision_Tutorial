from ultralytics import YOLO

if __name__ == '__main__':
    # 1. 加载我们自己训练好的模型
    # 路径需要根据实际训练结果的输出目录进行修改
    model = YOLO('./runs/detect/train/weights/best.pt')

    # 2. 导出为ONNX格式
    # opset是ONNX的版本，12或更高通常有较好的兼容性
    success_path = model.export(format='onnx', opset=12)

    if success_path:
        print("模型成功导出为ONNX格式:", success_path)