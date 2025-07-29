from ultralytics import YOLO

if __name__ == '__main__':
    # 1. 加载一个预训练模型
    # yolov8n.pt 是最小的模型，适合快速教学和CPU训练
    model = YOLO('yolov8n.pt')

    # 2. 开始训练
    # data: 指向我们的数据集配置文件
    # epochs: 训练轮次，表示要将整个数据集看多少遍。对于教学，设置一个较小的值。
    # imgsz: 训练时图像的尺寸
    # device: 明确指定使用CPU进行训练
    results = model.train(data='./dataset/temp_labelme_dataset/YOLODataset/dataset.yaml',
                          epochs=50,
                          imgsz=640,
                          device='cpu')

    print("训练完成！模型保存在:", results.save_dir)