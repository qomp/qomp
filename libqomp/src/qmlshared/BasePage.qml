import QtQuick 2.3

Rectangle {
	id: root

	signal accepted()
	property bool status: false
	property alias content: contents.children
	property int bottomOffset: 0
	readonly property int bottomOffsetMargine: 10 * scaler.Y

	color: "transparent"

	Keys.onReleased: {
		if (event.key === Qt.Key_Back) {
			event.accepted = true
			root.status = false
			root.accepted()
		}
	}

	Item {
		id: contents

		anchors.fill: parent
		anchors.bottomMargin: bottomOffset
	}
}
