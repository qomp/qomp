import QtQuick 2.5
import QtQuick.Window 2.2

Item {
	id: root

	signal activated()

	property bool readOnly: false
	property alias model: drop.model
	property alias text: line.text
	property bool expanded: false
	property int availableHeight: 100
	readonly property int maxDropHeight: availableHeight - mapToItem(parent, 0, drop.y).y

	Keys.onReleased: {
		if (event.key === Qt.Key_Back && root.expanded) {
			event.accepted = true
			root.expanded = false
		}
	}

	LineEdit {
		id: line

		anchors.fill: parent
		input.readOnly: root.readOnly

		Item {
			anchors.right: parent.right
			height: parent.height
			width: height

			Rectangle {
				id: expander

				anchors.fill: parent
				anchors.margins: line.border.width
				radius: line.radius

				color: ma.pressed ? "lightblue" : "white"
				Behavior on color { ColorAnimation { duration: 200 } }

				QompImage {
					source: "qrc:///icons/arrow-down"
					anchors.fill: parent
					anchors.margins: 10 * scaler.scaleMargins
				}
			}

			Rectangle {
				anchors.left: parent.left
				height: parent.height - line.border.width * 2
				anchors.verticalCenter: parent.verticalCenter
				width: 2
				color: "gray"
			}

			MouseArea {
				id: ma

				anchors.fill: parent
				onClicked: root.expanded = true
			}
		}
	}

	MouseArea {
		id: hideArea

		onClicked: root.expanded = false
	}

	ListView {
		id: drop

		readonly property int delegateHeight: 60 * scaler.scaleY

		anchors.top: line.bottom
		width: line.width
		height: Math.min(count * delegateHeight, root.maxDropHeight)

		model: []

		visible: opacity > 0
		opacity: root.expanded ? 1 : 0
		Behavior on opacity { NumberAnimation { } }

		clip: true
		boundsBehavior: Flickable.StopAtBounds
		focus: root.expanded

		delegate: Rectangle {
			id: deleg

			width: parent.width - 2
			anchors.horizontalCenter: parent.horizontalCenter
			height: drop.delegateHeight

			color: selector.pressed ? "cornflowerblue" : "#324A76"
			Behavior on color { ColorAnimation {} }

			Text {
				anchors.fill: parent
				anchors.leftMargin: 15 * scaler.scaleX
				verticalAlignment: Text.AlignVCenter
				font.pixelSize: height / 2.5
				color: "white"
				text: modelData
			}

			Line {
				anchors.bottom: parent.bottom
			}

			MouseArea {
				id: selector

				anchors.fill: parent

				onClicked: {
					root.activated()
					root.expanded = false
					root.text = modelData
				}
			}
		}
	}

	onExpandedChanged: {
		if(expanded) {
			hideArea.enabled = true
			hideArea.width = Screen.width
			hideArea.height = Screen.height
			var o = mapFromItem(null, 0, 0)
			hideArea.y = o.y
			hideArea.x = o.x

			drop.positionViewAtBeginning()
		}
		else {
			hideArea.enabled = false
		}
	}

	onModelChanged: if(model.length > 0) text = model[0]
}

