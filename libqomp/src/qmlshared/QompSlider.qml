import QtQuick 2.12


Item {
	id: root

	property real min: 0
	property real max: 0
	property real value: 0
	property alias pressed: ma.pressed
	readonly property real filled: value / (max - min)

	height: pos.height
	anchors.verticalCenter: parent.verticalCenter

	Rectangle {
		id: line

		anchors.centerIn: parent
		width: parent.width - pos.width
		height: 4
		radius: height / 2
		color: "lightgray"

		MouseArea {
			id: ma
			anchors.centerIn: parent
			width: root.width
			height: pos.height

			onPositionChanged: root.calculatePos(mapToItem(line, mouse.x,0).x)
			onPressed: root.calculatePos(mapToItem(line, mouse.x,0).x)
		}
	}

	Rectangle {
		id: pos

		anchors.verticalCenter: parent.verticalCenter
		height: 40 * scaler.scaleY
		width: height
		radius: height / 2
		color: "cornsilk"
		visible: root.max > root.min
		border.width: 1
		border.color: "gray"

		x: {
			var tmp = line.width * root.filled - width / 2
			if(tmp < -width / 2)
				tmp = -width / 2
			if(tmp > (line.width - width / 2))
				tmp = (line.width - width / 2)
			return tmp + line.x
		}
	}

	function calculatePos(x) {
		var pos = x / line.width
		pos = (max - min) * pos
		pos = Math.min(max, pos)
		pos = Math.max(min, pos)
		value = pos
	}
}
