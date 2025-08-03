import QtQuick
import QtQuick.Controls
import QtQuick.Layouts // 布局组件需要导入此模块

Window {
    id: rootWindow
    width: 960
    height: 720
    visible: true
    title: qsTr("AI螺丝瑕疵检测系统 V1.0")
    color: "#1e2a38"

    // --- 关键：添加Connections组件 ---
    Connections {
        target: backend // 监听我们在main.cpp中注册的backend对象

        // 当C++的backend对象发射statusMessageChanged信号时，这个函数会被自动调用
        // 函数名规则: on + 信号名(首字母大写)
        // 信号的参数会按顺序成为JS函数的参数
        function onStatusMessageChanged(message) {
            statusLabel.text = message;
        }
        // 当C++发射imageReady信号时，这个函数会被调用
        function onImageReady(imageId) {
            // 1. 构建URL: "image://<provider_name>/<image_id>"
            // 2. 附加一个时间戳来强制刷新，避免QML使用缓存
            videoDisplay.source = "image://liveImage/" + imageId
                    + "?" + new Date().getTime();
        }
    }

    // 使用一个ColumnLayout作为根布局，使其内部组件垂直排列
    ColumnLayout {
        anchors.fill: parent // 填充整个窗口
        anchors.margins: 10  // 设置边距
        spacing: 10          // 设置子组件之间的间距

        // --- 1. 图像显示区 ---
        // 使用Frame组件来创建一个带边框和背景的容器
        Frame {
            id: videoFrame
            Layout.fillWidth: true  // 宽度填充父布局
            Layout.fillHeight: true // 高度填充父布局，权重为1（默认）
            padding: 0
            background: Rectangle {
                color: "#101a24" // 一个更深的背景色
            }

            // 将占位的Text替换为Image组件
            Image {
                id: videoDisplay
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit // 保持宽高比
                cache: false // 显式禁用缓存，增加刷新可靠性
            }
        }

        // --- 2. 结果展示区 ---
        Frame {
            id: resultFrame
            Layout.fillWidth: true
            Layout.preferredHeight: 150 // 固定高度
            background: Rectangle {
                color: "#2c3e50"
            }

            Label { // 使用Label代替Text，样式更统一
                id: statusLabel
                text: "准备就绪"
                color: "white"
                font.pixelSize: 18
                anchors.centerIn: parent
            }
        }

        // --- 3. 控制区 ---
        // 使用RowLayout使内部按钮水平排列
        RowLayout {
            id: controlBar
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            Layout.alignment: Qt.AlignHCenter // 整体居中对齐
            spacing: 20

            Button {
                id: startButton
                text: "开始检测"
                Layout.preferredWidth: 120
                Layout.preferredHeight: 40
                // 关键：按钮点击时，调用C++ backend对象的startScan方法
                onClicked: {
                    backend.startCamera();
                }
            }

            Button {
                id: stopButton
                text: "停止"
                Layout.preferredWidth: 120
                Layout.preferredHeight: 40
                onClicked: {
                    backend.stopCamera();
                }
            }
        }
    }
}
