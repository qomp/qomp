import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

Rectangle {
	id: root

	property alias text: name.text
	signal triggered()


	height: 80 * scaler.scaleY
	implicitWidth: name.contentWidth + name.padding * 2
	implicitHeight: height
	color: Qt.lighter("black")
	Layout.fillWidth: true
	Layout.maximumWidth: Screen.desktopAvailableWidth - 20 * scaler.scaleX
	clip: true

	Text {
		id: name

		color: "white"

		height:  parent.height
		anchors.left: parent.left
		anchors.top: parent.top

		padding: 10 * scaler.scaleX
		font.pixelSize: 35 * scaler.scaleY
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignLeft
		elide: Text.ElideRight
	}

	MouseArea {
		anchors.fill: parent

		propagateComposedEvents: true
		onClicked: {
			root.triggered()
			mouse.accepted = false
		}
	}
}
