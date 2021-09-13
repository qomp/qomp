import QtQuick 2.9
import "qrc:///qmlshared"

Item {
	id: root

	property alias login: user.text
	property alias password: password.text
	property alias status: authStatus.text
	property alias result: user.text
	signal clicked()

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
				anchors.left: txt.right
				clip: true
				anchors.leftMargin: 10 * scaler.scaleX
				font.pointSize: 8
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
				input.echoMode: TextInput.Password
				anchors.verticalCenter: parent.verticalCenter
			}

			Text {
				anchors {
					top: pass.bottom
				}

				anchors.left: parent.left
				clip: true
				anchors.leftMargin: 10 * scaler.scaleX
				text: qsTr("*qomp doesn't save your password")
			}
		}

		OptionsEntry {
			QompButton {

				width: parent.width * 0.7
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
				text: qsTr("Update Authentication")
				onClicked: root.clicked()
			}
		}

		OptionsEntry {
			Text {
				id: result

				anchors.verticalCenter: parent.verticalCenter
				anchors.left: txt.right
				clip: true
				anchors.leftMargin: 10 * scaler.scaleX
				font.pointSize: 18
			}
		}
	}
}
