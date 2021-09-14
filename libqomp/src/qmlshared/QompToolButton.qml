import QtQuick 2.12

Rectangle {
	id: root

	signal clicked()

	property bool enabled: true
	property alias icon: image.source

	height: 72 * scaler.scaleY
	width: height


	border.width: 2
	border.color: "black"
	radius: 5 * scaler.scaleMargins

	MouseArea {
		id: ma
		anchors.fill: parent
		onClicked: if (root.enabled) root.clicked()
	}

	gradient: Gradient {
		GradientStop {
			id: gradientStop1
			position: 0
			color: "#f0f0f0"

			Behavior on color {
				ColorAnimation { duration: 50 }
			}
		}

		GradientStop {
			id: gradientStop2
			position: 1
			color: "#d9d9d9"

			Behavior on color {
				ColorAnimation { duration: 50 }
			}
		}
	}

	QompImage {
		id: image

		anchors.fill: parent
	}

	states: [
		State {
			name: "normal"
			when: !ma.pressed

			PropertyChanges {
				target: gradientStop1
				color: "#f0f0f0"
			}

			PropertyChanges {
				target: gradientStop2
				color: "#d9d9d9"
			}

			PropertyChanges {
				target: root
				scale: 1
			}
		},
		State {
			name: "Pressed"
			when: ma.pressed && root.enabled

			PropertyChanges {
				target: gradientStop1
				color: "#8faed5"
			}

			PropertyChanges {
				target: gradientStop2
				color: "#358bf3"
			}

			PropertyChanges {
				target: root
				scale: 0.98
			}
		},

		State {
			name: "disabled"
			when: root.enabled

			PropertyChanges {
				target: root
				border.color: "gray"
			}
		}
	]

	Behavior on scale {
		NumberAnimation { duration: 50 }
	}
}
