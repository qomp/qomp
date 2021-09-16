import QtQuick 2.12
import "qrc:///qmlshared"

Item {
	id: root

	property alias login: user.value
	property alias password: pass.value
	property alias status: authStatus.text
	property alias result: result.text
	signal doAuth()

	property bool busy: false

	height: col.height

	Column {
		id: col

		width: parent.width
		spacing: 0

		Component.onCompleted: {
			for(var i = 0; i < col.children.length; ++ i) {
				col.height += col.children[i].height
			}
		}

		OptionsEntry {
			Text {
				id: authStatus

				anchors.verticalCenter: parent.verticalCenter
				anchors.leftMargin: 10 * scaler.scaleX
				clip: true
				font.pointSize: 18
				text: qsTr("Not authenticated")
			}
		}

		OptionsEntry {

			TextLineEdit {
				id: user
				text: qsTr("User name")
				anchors.verticalCenter: parent.verticalCenter
				Keys.onReturnPressed: pass.focus = true
			}
		}

		OptionsEntry {

			TextLineEdit {
				id: pass
				text: qsTr("Password*")
				anchors.verticalCenter: parent.verticalCenter
				input.echoMode: TextInput.Password
			}
		}

		OptionsEntry {

			Text {
				anchors {
					top: parent.top
					left: parent.left
					margins: 10 * scaler.scaleX
				}
				font.pointSize: 10
				font.bold: true
				text: qsTr("*qomp doesn't save your password")
			}

			Text {
				id: result

				anchors {
					left: parent.left
					bottom: parent.bottom
					margins: 10 * scaler.scaleX
				}
				clip: true
				font.pointSize: 18

				onTextChanged: root.busy = false
			}

			QompBusyIndicator {
				id: busyInd

				anchors {
					verticalCenter: parent.verticalCenter
					right: parent.right
					margins: 10 * scaler.scaleX
				}
				height: parent.height / 2
				width: height
				visible: false
			}
		}

		OptionsEntry {

			QompButton {
				id: btAuth
				width: parent.width * 0.7
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
				text: qsTr("Update Authentication")
				onClicked: {
					Qt.inputMethod.commit()
					Qt.inputMethod.hide()
					root.doAuth()
					root.busy = true
				}
			}
		}
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
