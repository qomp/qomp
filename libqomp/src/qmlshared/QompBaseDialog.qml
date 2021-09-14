import QtQuick 2.12
import "qrc:///qmlshared"

Item {
	id: root

	property alias content: placeHolder.children
	property alias preferredWidth: placeHolder.width
	property alias preferredHeight: placeHolder.height

	anchors.fill: parent

	visible: opacity > 0
	focus: visible
	opacity: 0
	Behavior on opacity { NumberAnimation {} }

	Keys.onReleased: {
		if (event.key === Qt.Key_Back && visible) {
			event.accepted = true
			root.hide()
		}
	}

	Rectangle {
		id: fader

		anchors.fill: parent
		color: "black"
		opacity: 0.7
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			root.hide()
		}
	}

	Rectangle {
		id: placeHolder

		anchors.centerIn: parent
		width: parent.height > parent.width ? parent.width * 0.9 : parent.width * 0.7
		height: parent.height > parent.width ? parent.height * 0.7 : parent.height * 0.9
		radius: 8 * scaler.scaleMargins
	}


	function open() {
		opacity = 1
	}

	function hide() {
		opacity = 0
	}
}
