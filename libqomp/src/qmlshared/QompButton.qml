import QtQuick 2.9

Rectangle {
	id: root

	property bool enabled: true
	property alias text: label.text
	readonly property int animDuration: 50

	signal clicked()

	width: 140 * scaler.scaleX
	height: 72 * scaler.scaleY
	radius: 6 * scaler.scaleMargins
	border.width: 2

	gradient: Gradient {
		GradientStop {
			id: gradientStop1
			position: 0
			color: "#f0f0f0"

			Behavior on color {
				ColorAnimation { duration: root.animDuration }
			}
		}

		GradientStop {
			id: gradientStop2
			position: 1
			color: "#d9d9d9"

			Behavior on color {
				ColorAnimation { duration: root.animDuration }
			}
		}
	}

	Text {
		id: label

		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter
		anchors.centerIn: parent
		font.pixelSize: root.height / 3
		color: "black"

		Behavior on color {
			ColorAnimation { duration: root.animDuration }
		}
	}

	MouseArea {
		id: ma
		width: root.width * 1.2
		height: root.height * 1.2
		anchors.centerIn: parent
		onClicked: if(root.enabled) root.clicked()
	}

	states: [
		State {
			name: "Normal"
			when: root.enabled && !ma.pressed

			PropertyChanges {
				target: gradientStop1
				color: "#f0f0f0"
			}
			PropertyChanges {
				target: gradientStop2
				color: "#d9d9d9"
			}
			PropertyChanges {
				target: label
				color: "black"
			}
			PropertyChanges {
				target: root
				scale: 1
			}
		},
		State {
			name: "Pressed"
			when: root.enabled && ma.pressed

			PropertyChanges {
				target: gradientStop1
				color: "#8faed5"
			}
			PropertyChanges {
				target: gradientStop2
				color: "#358bf3"
			}
			PropertyChanges {
				target: label
				color: "MidnightBlue"
			}
			PropertyChanges {
				target: root
				scale: 0.98
			}
		},
		State {
			name: "Disabled"
			when: !root.enabled

			PropertyChanges {
				target: rect
				border.color: "gray"
			}
			PropertyChanges {
				target: label
				color: "gray"
			}
		}
	]

	Behavior on scale {
		NumberAnimation { duration: root.animDuration }
	}
}

