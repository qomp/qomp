import QtQuick 2.12

Rectangle {
	id: root

	signal triggered()
	property alias text: txt.text
	property bool checkable: false
	property bool checked: false
	property int textOffset: 25 * scaler.scaleX

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

	Loader {
		id: cbLoader
		sourceComponent: cbComp
		anchors.right: parent.right
		anchors.rightMargin: 5 * scaler.scaleX
		width: height
		height: parent.height
		active: root.checkable
	}

	Component {
		id: cbComp

		QompCheckBox {
			checkBoxHeight: root.height / 2
			anchors.centerIn: parent
			checked: root.checked
		}
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
