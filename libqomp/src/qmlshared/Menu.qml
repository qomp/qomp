import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

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
		id: flick

		clip: true
		contentWidth: contents.width
		contentHeight: contents.height
		anchors.centerIn: parent
		height: Math.min(root.height - 20 * scaler.scaleY, contents.height)
		width:  Math.min(root.width  - 20 * scaler.scaleX, contents.width)
		boundsBehavior: Flickable.StopAtBounds
		pressDelay: 200
		focus: true

		Rectangle {
			color: "lightgrey"

			width: contents.width
			height: contents.height

			ColumnLayout {
				id: contents

				readonly property int itemCount: children.length
				readonly property int itemHeight: itemCount > 0 ? height / itemCount : 0
				readonly property double visibleItemsCount: itemHeight > 0 ? flick.height / itemHeight : 0

				anchors.centerIn: parent
				spacing: 1
			}
		}

		onDragStarted: positioner.show()
		onDragEnded: positioner.hide()
	}

	Rectangle {
		id: positioner

		readonly property int space: 20 * scaler.scaleY
		readonly property int itemDelta: (contents.itemCount - contents.visibleItemsCount) > 0 ?
										(flick.height - space * 2 - height) / (contents.itemCount - contents.visibleItemsCount) : 0
		width: 5 * scaler.scaleX
		height: contents.visibleItemsCount > 0 ? (flick.height - space * 2) * contents.visibleItemsCount / contents.itemCount : 0
		x: flick.x + flick.width - 10 * scaler.scaleX
		y: contents.itemHeight > 0 ? flick.y + space + itemDelta * flick.contentY / contents.itemHeight : flick.y
		color: "silver"
		visible: opacity > 0
		opacity: 0

		Behavior on opacity {
			NumberAnimation {}
		}

		function show() {
			if (itemDelta > 0)
				opacity = 0.8
		}

		function hide() {
			opacity = 0;
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
