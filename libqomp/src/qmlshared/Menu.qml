import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3

FocusScope {
	id: root

	default property alias menuItems: contents.data

	width: Screen.desktopAvailableWidth
	height: Screen.desktopAvailableHeight
	x: 0; y: 0
	visible: opacity > 0
	focus: visible
	opacity: 0

	Keys.onReleased: {
		if (event.key === Qt.Key_Back && visible) {
			event.accepted = true;
			root.hide()
		}
	}

	Rectangle {
		id: fader

		anchors.fill: parent
		color: "black"
		opacity: 0.7
	}

	MouseArea {
		anchors.fill: parent
		onClicked: root.hide()
	}


	Flickable {
		clip: true
		contentWidth: contents.width
		contentHeight: contents.height
		anchors.centerIn: parent
		height: Math.min(root.height - 20 * scaler.scaleY, contents.height)
		width:  Math.min(root.width  - 20 * scaler.scaleX, contents.width)
		boundsBehavior: Flickable.StopAtBounds
		pressDelay: 100

		Rectangle {
			color: "white"

			width: contents.width
			height: contents.height

			ColumnLayout {
				id: contents

				anchors.centerIn: parent
				spacing: 1
				focus: true
			}
		}
	}

	Behavior on opacity {
		NumberAnimation {}
	}

	function insertItem(index, object) {
		if(index >= menuItems.length) {
			menuItems.push(object)
		}
		else {
			var list = []

			for(var i = 0; i < menuItems.length; i++) {
				if(i === index)
					list.push(object)

				list.push(menuItems[i])
			}
			menuItems = list
		}
	}

	function removeItem(object) {
		var list = []
		for(var i = 0; i < menuItems.length; i++) {
			if(menuItems[i] !== object)
				list.push(menuItems[i])
		}
		menuItems = list
	}

	function popup() {
		root.opacity = 1
		root.forceActiveFocus()
	}

	function hide() {
		root.opacity = 0
	}
}
