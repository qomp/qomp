import QtQuick 2.5
import "qrc:///qmlshared"

Item {
	id: root

	property ListView list: ListView.view
	property bool busy: false
	property bool playing: false
	readonly property int animDuration: 100
	property bool current: model.current === undefined ? false : model.current
	property bool held: drager.pressed

	signal activated()
	signal longTap()

	height: 100 * scaler.scaleY

	MouseArea {
		anchors.fill: parent
		anchors.leftMargin: drager.width * 1.2
		onPressed: list.currentIndex = index
		onDoubleClicked: root.activated()
		onPressAndHold: root.longTap()
	}

	Rectangle {
		id: place
		color: "transparent"
		anchors.fill: parent
		anchors.margins: 2 * scaler.scaleMargins
		border.width: 2
		border.color: "midnightblue"
		visible: root.held
	}

	Rectangle {
		id: content

		color: root.held ? "lightsteelblue" : (list.currentIndex === index ? "#68828A" : "lightblue")
		Behavior on color { ColorAnimation { duration: root.animDuration } }

		width: root.width
		height: root.height

		Behavior on x { NumberAnimation { duration: root.animDuration } }

		Drag.source: drager
		Drag.hotSpot.x: width / 2
		Drag.hotSpot.y: height / 2
		Drag.active: root.held

		states: [
			State {
				name: "drag"
				when: root.held

				AnchorChanges {
					target: content
					anchors {
						verticalCenter: undefined
						horizontalCenter: undefined
					}
				}
				ParentChange {
					target: content
					parent: root.list.parent
					x: 15 * scaler.scaleX
					y: mapToItem(root.list.parent, 0, 0).y + 15 * scaler.scaleY
				}
			},
			State {
				name: "normal"
				when: !root.held

				ParentChange {
					target: content
					parent: root
				}
				AnchorChanges {
					target: content
					anchors {
						horizontalCenter: root.horizontalCenter
						verticalCenter: root.verticalCenter
					}
				}
			}
		]

		transitions: Transition {
			from: "drag"
			to: "normal"
			SequentialAnimation {
				ParentAnimation {}
				AnchorAnimation { duration: root.animDuration }
			}
		}

		MouseArea {
			id: drager

			property int itemIndex: index

			anchors.left: content.left
			width: 40 * scaler.scaleX
			height: content.height

			drag.target: root.held ? content : undefined
			drag.axis: Drag.YAxis

			Column {
				id: col
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.6
				spacing: 5

				Repeater {
					model: 3
					Rectangle {
						width: parent.width
						height: col.spacing
						color: "gray"
					}
				}
			}
		}

		Item {
			id: textPart

			anchors.top: content.top
			anchors.left: drager.right
			anchors.right: (stateLoader.active) ? stateLoader.left : content.right
			anchors.bottom: content.bottom

			Text {
				id: mainText

				height: parent.height / 2
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.margins: 5 * scaler.scaleMargins

				text: {
					var txt = model.title
					if (txt === undefined || txt.length === 0)
						txt = model.url
					if(txt === undefined || txt.length === 0)
						txt = model.file
					if(txt === undefined)
						txt = ""

					return txt
				}

				clip: true
				elide: Text.ElideMiddle
				font.bold: root.current
				font.pixelSize: parent.height / 4
				horizontalAlignment: Text.AlignLeft
				verticalAlignment: Text.AlignVCenter
			}

			Text {
				id: addText

				anchors.bottom: parent.bottom
				anchors.top: mainText.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.margins: mainText.anchors.margins

				text: model.artist + '  ' + model.duration
				horizontalAlignment: Text.AlignLeft
				verticalAlignment: Text.AlignTop
				clip: true
				font.pixelSize: parent.height / 5
			}
		}

		Loader {
			id: stateLoader
			sourceComponent: statesComp
			active: root.current
			anchors.right: parent.right

			anchors.verticalCenter: parent.verticalCenter
			height: content.height * 0.7
			width: height
		}

		Component {
			id: statesComp

			Item {
				property alias image: image
				property alias busy: busy

				QompImage {
					id: image

					source: "qrc:///icons/tune"
					opacity: 0
					anchors.fill: parent

					Behavior on opacity { NumberAnimation { duration: root.animDuration } }
				}

				QompBusyIndicator {
					id: busy

					opacity: 0
					visible: opacity > 0
					anchors.fill: parent

					Behavior on opacity { NumberAnimation { duration: root.animDuration } }
				}
			}
		}

		Line {
			anchors.bottom: parent.bottom
			height: 1
		}
	}

	DropArea {
		anchors.fill: parent
		anchors.margins: 5 * scaler.scaleMargins

		onEntered: {
			var ind1 = drag.source.itemIndex
			var ind2 = drager.itemIndex
			if(ind1 !== ind2)
				root.list.model.move(ind1, ind2)
		}
	}

	states: [
		State {
			name: "playing"
			when: root.current && root.playing && !root.busy

			PropertyChanges {
				target: stateLoader.item.image
				opacity: 1
			}
			PropertyChanges {
				target: stateLoader.item.busy
				opacity: 0
			}
		},
		State {
			name: "busy"
			when: root.current && root.busy

			PropertyChanges {
				target: stateLoader.item.image
				opacity: 0
			}
			PropertyChanges {
				target: stateLoader.item.busy
				opacity: 1
			}
		},
		State {
			name: "normal"
			when: root.current && !root.busy && !root.playing

			PropertyChanges {
				target: stateLoader.item.image
				opacity: 0
			}
			PropertyChanges {
				target: stateLoader.item.busy
				opacity: 0
			}
		}
	]
}
