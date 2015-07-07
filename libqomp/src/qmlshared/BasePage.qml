import QtQuick 2.5

FocusScope {
	id: root

	signal accepted()
	property bool status: false
	property alias content: contents.children
	property int bottomOffset: 0
	readonly property int bottomOffsetMargine: 10 * scaler.Y
	property alias title: pt.text

	Keys.onReleased: {
		if (event.key === Qt.Key_Back) {
			event.accepted = true
			root.status = false
			root.accepted()
		}
	}

	PageTitle { id: pt }

	Item {
		id: contents

		focus: true

		anchors.fill: parent
		anchors.topMargin: pt.height
		anchors.bottomMargin: bottomOffset
	}
}
