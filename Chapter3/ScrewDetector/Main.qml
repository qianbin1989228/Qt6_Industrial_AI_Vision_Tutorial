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

            // 这里暂时用一个文本来占位，后续将替换为真实的视频画面
            Text {
                text: "实时视频显示区"
                color: "#555"
                font.pixelSize: 24
                anchors.centerIn: parent
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

            Text {
                text: "检测结果列表区"
                color: "#7f8c8d"
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
            }

            Button {
                id: stopButton
                text: "停止"
                Layout.preferredWidth: 120
                Layout.preferredHeight: 40
            }
        }
    }
}
