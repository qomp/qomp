import QtQuick 2.9

Item {
	id: root

	property bool expanded: false
	property alias contents: col.children

	width: parent.width
	visible: false

	Rectangle {
		id: fader

		color: "black"
		anchors.fill: parent
		opacity: 0

		MouseArea {
			anchors.fill: parent
		}
	}

	Rectangle {
		id: slide

		width: parent.width * ((parent.width < parent.height) ? 0.85 : 0.6)
		height: parent.height
		color: "#68828A"

		x: -width
		opacity: 1

		Flickable {
			id: flic

			anchors.fill: parent
			contentWidth: width
			contentHeight: col.height

			clip: true
			boundsBehavior: Flickable.StopAtBounds

			Column {
				id: col
				width: parent.width
			}
		}

		states: [
			State {
				name: "expanded"
				when: root.expanded
				PropertyChanges {
					target: root
					visible: true

				}
			},
			State {
				name: "collapsed"
				when: !root.expanded
				PropertyChanges {
					target: root
					visible: false

				}
			}
		]

		transitions: [
			Transition {
				from: "collapsed"
				to: "expanded"
				SequentialAnimation {
					PropertyAction { target: root; property: "visible"; value: true }
					ParallelAnimation {
						NumberAnimation { target: slide
							property: "x"; duration: 500;
							from: -slide.width; to: 0;
							easing.type: Easing.InOutQuad
						}
						NumberAnimation { target: fader;
							property: "opacity"; duration: 500;
							from: 0; to: 0.6;
							easing.type: Easing.InOutQuad
						}
					}
				}

			},
			Transition {
				from: "expanded"
				to: "collapsed"
				SequentialAnimation {
					ParallelAnimation {
						NumberAnimation { target: slide
							property: "x"; duration: 500;
							from: 0; to: -slide.width;
							easing.type: Easing.InOutQuad
						}
						NumberAnimation { target: fader;
							property: "opacity"; duration: 500;
							from: 0.6; to: 0;
							easing.type: Easing.InOutQuad
						}
					}
					PropertyAction { target: root; property: "visible"; value: false }
				}
			}
		]
	}
}
