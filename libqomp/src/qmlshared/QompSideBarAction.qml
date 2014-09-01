import QtQuick 2.3

Rectangle {
	id: root

	signal triggered()
	property alias text: txt.text
	property bool checkable: false
	property bool checked: false
	property int textOffset: 15 * scaler.scaleX

	height: 80 * scaler.scaleY
	width: parent.width

	color: ma.pressed ? "lightblue" : "#68828A"

	Behavior on color {
		ColorAnimation { duration: 200 }
	}

	Text {
		id: txt
		anchors.fill: parent
		anchors.leftMargin: root.textOffset
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignLeft
		font.pixelSize: parent.height / 2.5
		color: ma.pressed ? "#B2B2B2" : "white"

		Behavior on color {
			ColorAnimation { duration: 200 }
		}
	}

	QompCheckBox {
		id: cb

		checkBoxHeight: root.height / 2
		anchors.right: parent.right
		anchors.rightMargin: 15 * scaler.scaleX
		anchors.verticalCenter: parent.verticalCenter
		visible: root.checkable
		checked: root.checked
	}

	MouseArea {
		id: ma
		anchors.fill: parent
		onClicked: {
			if(root.checkable)
				root.checked = !root.checked

			root.triggered()
		}
	}
}
