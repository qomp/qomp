import QtQuick 2.9

Item {
	id: root

	property int checkBoxHeight: height * 0.8
	property bool checked: false

	signal clicked()

	height: parent.height
	width: height

	Rectangle {
		id: checkRect
		color: "white"

		height: root.checkBoxHeight
		width: height
		anchors.centerIn: parent

		border.width: 2
		border.color: root.checked? "midnightblue" : "black"
		radius: 4 * scaler.scaleMargins

		QompImage {
			anchors.fill: parent
			anchors.margins: 5 * scaler.scaleMargins
			visible: root.checked
			source: "qrc:///icons/check"
		}
	}

	MouseArea {
		id: ma

		anchors.fill: parent
		onClicked: root.clicked()
	}

	onClicked: checked = !checked
}
