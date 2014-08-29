import QtQuick 2.3
import QtQuick.Controls 1.2


ApplicationWindow {
	id: root

	signal exit()

	visible: true

	height: 600
	width: 300

	color: "lightblue"

	onClosing: {
		event.accepted = true
		root.exit()
	}

	Item {
		anchors.fill: parent

		Keys.onReleased: {
			if (event.key === Qt.Key_Back) {
				event.accepted = true;
				if (mainview.depth > 1) {
					removeView();	
				}
				else {
					root.exit()
				}
			}
		}

		StackView {
			id: contents

			property int animDuration: 200
			anchors.fill: parent

			delegate: StackViewDelegate {
				function transitionFinished(properties) {
					properties.exitItem.opacity = 1
				}

				pushTransition: StackViewTransition {
					id: trans
					PropertyAnimation {
						duration: contents.animDuration
						target: enterItem
						property: "opacity"
						from: 0
						to: 1
					}
					PropertyAnimation {
						duration: contents.animDuration
						target: exitItem
						property: "opacity"
						from: 1
						to: 0
					}
				}
			}
		}
	}
	function addView(item) {
		contents.push({item: item, destroyOnPop: true/* ,immediate: true*/})
		contents.currentItem.focus = true
	}

	function removeView() {
		var item = contents.pop({/*immediate: true*/})
		contents.currentItem.focus = true
	}

	function clear() {
		contents.clear()
	}
}
