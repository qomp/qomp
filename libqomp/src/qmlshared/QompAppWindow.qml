import QtQuick 2.3
import QtQuick.Controls 1.2


ApplicationWindow {
	id: root

	visible: true

	color: "lightblue"

	Component.onCompleted:  {
		scaler.scaleX = root.width / 480
		scaler.scaleY = root.height / 800
		scaler.scaleFont = (400 + root.width * root.height * 0.00015) / 457
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
					if(contents.active) {
						root.clear()
						contents.active = false
						image.visible = true
					}
					Qt.quit()
				}
			}
		}

		Loader {
			id: contents

			anchors.fill: parent
			sourceComponent: stack
			focus: true
			active: false
		}

		Component {
			id: stack

			StackView {
				id: sv
				property int animDuration: 200

				delegate: StackViewDelegate {
					function transitionFinished(properties) {
						properties.exitItem.opacity = 1
					}

					pushTransition: StackViewTransition {
						id: trans
						PropertyAnimation {
							duration: sv.animDuration
							target: enterItem
							property: "opacity"
							from: 0
							to: 1
						}
						PropertyAnimation {
							duration: sv.animDuration
							target: exitItem
							property: "opacity"
							from: 1
							to: 0
						}
					}
				}
			}
		}

		Image {
			id: image

			width: parent.width * 0.9
			height: width
			anchors.verticalCenter: parent.verticalCenter
			fillMode: Image.PreserveAspectFit
			source: "qrc:///icons/icons/qomp.png"
			visible: false
		}

	}
	function addView(item) {
		if(!contents.active)
			contents.active = true

		contents.item.push({item: item, destroyOnPop: true/* ,immediate: true*/})
		contents.item.currentItem.focus = true
	}

	function removeView() {
		var item = contents.item.pop({/*immediate: true*/})
		contents.item.currentItem.focus = true
	}

	function clear() {
		contents.item.clear()
	}
}
