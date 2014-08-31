import QtQuick 2.3

Rectangle {
	id: root

	property alias text: txt.text

	height: 80 * scaler.scaleY
	width: parent.width
	anchors.top: parent.top
	color: "steelblue"

	Text {
		id: txt
		anchors.fill: parent
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter
		font.pixelSize: parent.height / 2.5
	}
}
