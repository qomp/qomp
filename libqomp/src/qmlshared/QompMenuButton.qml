import QtQuick 2.3

Rectangle {
	id: root

	signal clicked()

	property int lineH: root.height * 0.1
	property bool expanded: false

	width: root.lineH* 3 + icon.width + 15 * scaler.scaleX

	color: ma.pressed ? "#2A4E6C" : "steelblue"
	Behavior on color {
		ColorAnimation { duration: 100 }
	}

	Image {
		id: icon
		source: "qrc:///icons/icons/qomp.png"
		fillMode: Image.PreserveAspectFit
		height: root.height * 0.7
		anchors.right: parent.right
		anchors.rightMargin: 5 * scaler.scaleX
		anchors.verticalCenter: parent.verticalCenter
	}

	Column {
		anchors.left: parent.left
		anchors.right: icon.left
		anchors.top: parent.top
		anchors.topMargin: (root.height - root.height * 0.5) / 2
		spacing: (root.height * 0.5 - root.lineH* 3) / 2

		Repeater {

			model: 3
			Rectangle {

				height: root.lineH
				width: height * (root.expanded ? 1.5 : 3)
				color: "lightgray"

				Behavior on width {
					NumberAnimation { duration: 200 }
				}
			}
		}
	}

	MouseArea {
		id: ma
		anchors.fill: parent
		onPressed: {
			root.clicked()
			root.expanded = !root.expanded
		}
	}
}
