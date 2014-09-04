import QtQuick 2.3

TickerLabel {
	id: root

	height: 70 * scaler.scaleY
	width: parent.width
	anchors.top: parent.top
	color: "steelblue"
	font.pixelSize: height / 2.5
}
