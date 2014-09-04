import QtQuick 2.3
import QtQuick.Controls 1.2

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
		//running: visible
	}

	//		SequentialAnimation {
	//			id:colorAnim

	//			property color from: "lightsteelblue"
	//			property color to: "#6A7685"

	//			loops: Animation.Infinite
	//			running: false

	//			ColorAnimation {
	//				duration: 300

	//				target: root
	//				from: colorAnim.from
	//				to: colorAnim.to
	//			}

	//			ColorAnimation {
	//				duration: 300

	//				target: root
	//				property: "color"
	//				to: colorAnim.from
	//				from: colorAnim.to
	//			}

	//			onRunningChanged: if(!running) root.color = from
	//		}

	states: [
		State {
			name: "busy"
			when: root.busy

			PropertyChanges {
				target: busyInd
				visible: true
			}

//			PropertyChanges {
//				target: colorAnim
//				running: true
//			}
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
