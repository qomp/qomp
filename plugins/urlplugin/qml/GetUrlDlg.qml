import QtQuick 2.3
import QtQuick.Controls 1.2
import "qrc:///qmlshared"

Rectangle {
	id: root

	signal accepted()
	property bool status: false
	property alias url: textUrl.text

	color: "transparent"

	Keys.onReleased: {
		if (event.key === Qt.Key_Back) {
			event.accepted = true
			root.status = false
			root.accepted()
		}
	}

	TextField {
		id: textUrl

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		width: parent.width * 0.8

		Keys.onReturnPressed: {
			root.status = true
			root.accepted()
		}
	}

	QompButton {
		id: ok

		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: 15 * scaler.scaleMargins
		text: qsTr("OK")
		onClicked: {
			root.status = true
			root.accepted()
		}
	}

	QompButton {
		id: cancel

		anchors.right: ok.left
		anchors.bottom: parent.bottom
		anchors.margins: 15 * scaler.scaleMargins
		text: qsTr("Cancel")
		onClicked: {
			root.status = false
			root.accepted()
		}
	}
}
