import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

Rectangle {
	id: root

	property alias text: name.text
	signal triggered()
	readonly property int space: 20 * scaler.scaleX


	height: 80 * scaler.scaleY
	implicitWidth: name.contentWidth + name.padding * 2
	implicitHeight: height
	color: ma.pressed ? "dimgray" : Qt.lighter("black")
	Layout.fillWidth: true
	Layout.maximumWidth: Screen.desktopAvailableWidth - 20 * scaler.scaleX
	clip: true

	Behavior on color {
		ColorAnimation {}
	}

	Text {
		id: name

		color: "white"

		height:  parent.height
		anchors.left: parent.left
		anchors.top: parent.top

		padding: root.space
		font.pixelSize: 30 * scaler.scaleY
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignLeft
		elide: Text.ElideRight
	}

	MouseArea {
		id: ma
		anchors.fill: parent

		propagateComposedEvents: true
		onClicked: {
			root.triggered()
			mouse.accepted = false
		}
	}
}
