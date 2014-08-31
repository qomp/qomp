import QtQuick 2.3
import "qrc:///qmlshared"

Item {
	id: root

	property alias login: user.value
	property alias password: pass.value

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

			TextLineEdit {
				id: user
				text: qsTr("Login:")
				anchors.verticalCenter: parent.verticalCenter
				Keys.onReturnPressed: pass.focus = true
			}
		}

		OptionsEntry {

			TextLineEdit {
				id: pass
				text: qsTr("Password:")
				input.echoMode: TextInput.Password
				anchors.verticalCenter: parent.verticalCenter
			}
		}
	}
}
