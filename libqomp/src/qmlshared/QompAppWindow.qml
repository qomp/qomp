import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Window 2.1

ApplicationWindow {
	id: root

	visible: false
	contentOrientation: Qt.PortraitOrientation

	color: "lightblue"

	onWidthChanged: updateScaler()
	onHeightChanged: updateScaler()

	function updateScaler() {
		var w = Math.min(width, height)
		var h = Math.max(width, height)
		var myDensity = 9.15
		var curDensity = Screen.pixelDensity
		scaler.scaleX = (w / curDensity) / (480 / myDensity)
		scaler.scaleY = (h / curDensity) / (800 / myDensity)
		scaler.scaleFont = (400 + width * height * 0.00015) / 457
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
						NumberAnimation {
							duration: sv.animDuration
							target: enterItem
							property: "opacity"
							from: 0
							to: 1
						}
						NumberAnimation {
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

		item.width = width
		item.height = height
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
