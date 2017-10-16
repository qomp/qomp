import QtQuick 2.9
import QtQuick.Window 2.3

Window {
	id: root

	signal confirmExit()

	contentOrientation: Qt.PortraitOrientation
	visibility: Window.Hidden

	color: "lightblue"

	onWidthChanged: updateScaler()
	onHeightChanged: updateScaler()

	function updateScaler() {
		scaler.updateScales()
	}

	Timer {
		id: timer

		running: false
		repeat: false
		interval: 2000
	}

	Item {
		anchors.fill: parent

		Keys.onReleased: {
			if (event.key === Qt.Key_Back) {
				event.accepted = true;
				if (contents.active && contents.item.depth > 1) {
					root.removeView();
				}
				else {
					if(timer.running) {
						timer.stop()
						Qt.quit()
					}
					else {
						root.confirmExit()
						timer.start()
					}
				}
			}
		}

		Image {
			id: image

			width: Math.min(parent.width * 0.9, 400);
			height: width
			anchors.centerIn: parent
			fillMode: Image.PreserveAspectFit
			source: "qrc:///icons/icons/qomp.png"
			visible: true
		}

		Loader {
			id: contents

			anchors.fill: parent
			sourceComponent: stack
			focus: true
			active: false
			opacity: active ? 1 : 0
			visible: opacity > 0

			Behavior on opacity {
				NumberAnimation {}
			}
		}

		Component {
			id: stack

			QompStackView {
				id: sv
				anchors.fill: parent
			}
		}
	}

	function addView(item) {
		if(!contents.active) {
			contents.active = true
			image.visible = false
			contents.update()
		}

		contents.item.push(item)
	}

	function removeView() {
		contents.item.pop()
	}

	function clear() {
		contents.item.clear()
	}

	function beforeClose() {
		if(contents.active) {
			clear()
			image.visible = true
			contents.active = false
		}
	}
}
