import QtQuick 2.3
import QtQuick.Controls 1.2


ApplicationWindow {
	id: root

	visible: true

	color: "lightblue"

	Component.onCompleted: updateScaler()
	onWidthChanged: updateScaler()
	onHeightChanged: updateScaler()

	function updateScaler() {
		scaler.scaleX = mainwindow.width / 480
		scaler.scaleY = mainwindow.height / 800
		scaler.scaleFont = (400 + mainwindow.width * mainwindow.height * 0.00015) / 457
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
					Qt.quit()
				}
			}
		}

		Image {
			id: image

			width: parent.width * 0.9
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

			StackView {
				id: sv
				property int animDuration: 200

				anchors.fill: parent

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
	}

	function addView(item) {
		if(!contents.active) {
			contents.active = true
			image.visible = false
		}

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

	function beforeClose() {
		if(contents.active) {
			root.clear()
			image.visible = true
			contents.active = false
		}
	}
}
