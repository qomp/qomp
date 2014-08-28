import QtQuick 2.3
import QtQuick.Controls 1.2


ApplicationWindow {
	id: root

	signal exit()

	visible: true

	height: 600
	width: 300

	Component.onCompleted: {
		//add empty rect to view
		root.addView(Qt.createQmlObject("import QtQuick 2.3; Item{}", root))
	}

	color: "lightblue"

	onClosing: root.exit()

	Item {
		anchors.fill: parent

		Keys.onReleased: {
			if (event.key === Qt.Key_Back) {
				if (mainview.depth > 2) { // first view - is emty rect
					removeView();
					event.accepted = true;
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
		contents.push({item: item})
		contents.currentItem.focus = true
	}

	function removeView() {
		var item = contents.pop({})
		item.destroy(contents.animDuration * 2)
		contents.currentItem.focus = true
	}

	function clear() {
		contents.clear()
	}
}
