import QtQuick 2.3

Rectangle {
	id: root

	property color baseColor: "dimgray"
	property color highlightColor: "whitesmoke"
	property int speed: 1600
	property int count: 16

	smooth: true
	antialiasing: true
	color: "transparent"

	Repeater {
		id: rep
		property int cur: 0

		model: root.count
		anchors.centerIn: parent
		height: parent.height / 1.3
		width: height

		Rectangle {
			id: shape

			property SequentialAnimation anim;
			color: root.baseColor

			height:rep.height*Math.PI/root.count
			width: height
			radius: height / 2

			anchors.centerIn: parent
			anchors.verticalCenterOffset: -rep.height/2 * Math.cos( (2*Math.PI / root.count) * index)
			anchors.horizontalCenterOffset: -rep.height/2 * Math.sin( (2*Math.PI / root.count) * index)

			states: [
				State {
					name: "active"
					when: index === rep.cur

					PropertyChanges {
						target: shape
						color: root.highlightColor
					}
				},
				State {
					name: "inactive"
					when: index !== rep.cur

					PropertyChanges {
						target: shape
						color: root.baseColor
					}
				}
			]

			transitions: [
				Transition {
					from: "inactive"
					to: "active"
					ColorAnimation { duration: 20 }
				},
				Transition {
					from: "active"
					to: "inactive"
					ColorAnimation { duration: root.speed / 4 }
				}
			]
		}
	}

	Timer {
		id: anim

		interval: root.speed / root.count
		repeat: true
		triggeredOnStart : true
		running: root.visible

		onTriggered: {
			var cur = rep.cur
			if(--cur < 0)
				cur = root.count

			rep.cur = cur
		}
	}
}

