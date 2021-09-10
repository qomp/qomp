import QtQuick 2.9
import "qrc:///qmlshared"

Item {
	id: root

	property alias login: user.text
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
				id: txt
				anchors.left: parent.left
				width: parent.width / 2
				clip: true
				anchors.verticalCenter: parent.verticalCenter
				horizontalAlignment: Text.AlignRight
				wrapMode: Text.WordWrap
				text: qsTr("Authorized User Name:")
				font.pointSize: 18
			}

			Text {
				id: user

				anchors.verticalCenter: parent.verticalCenter
				anchors.left: txt.right
				clip: true
				anchors.leftMargin: 10 * scaler.scaleX
				font.pointSize: 18
			}
		}

		OptionsEntry {
			QompButton {

				width: parent.width * 0.7
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
				text: qsTr("Update Authorization")
				onClicked: root.clicked()
			}
		}
	}
}
