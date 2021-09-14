import QtQuick 2.12

BasePage {
	id: root

	property bool busy: false

	bottomOffset: ok.height + ok.anchors.margins*2 + bottomOffsetMargine

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

	QompBusyIndicator {
		id: busyInd

		anchors.left: parent.left
		anchors.bottom: parent.bottom
		anchors.margins: 15 * scaler.scaleMargins
		height: Math.min(cancel.height, cancel.x - x)
		width: height

		visible: false
	}

	states: [
		State {
			name: "busy"
			when: root.busy

			PropertyChanges {
				target: busyInd
				visible: true
			}
		},
		State {
			name: "normal"
			when: !root.busy

			PropertyChanges {
				target: busyInd
				visible: false
			}
		}
	]
}
