import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

ButtonsPage {
	id: root

	signal doSearch()
	signal editTextChanged()

	property alias model: items.model
	property alias serchText: items.editText
	readonly property alias pluginContent: placeholder
	property bool busy: false
	property bool waitForSuggestions: false

	Keys.onReleased: {
		if (event.key === Qt.Key_Back) {
			event.accepted = true
			root.status = false
			root.accepted()
		}
	}

	content: Item {
		id: mainRect

		anchors.fill: parent

		Rectangle {
			id: toprect

			anchors.top: parent.top
			width: parent.width
			height: 80 * scaler.scaleY

			color: "transparent"

			QompToolButton {
				id: search

				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				anchors.leftMargin: 10 * scaler.scaleX
				anchors.rightMargin: 10 * scaler.scaleX

				icon: "qrc:///icons/search"

				focus: true
				onClicked: {
					Qt.inputMethod.hide()
					root.waitForSuggestions = false
					root.doSearch()
				}
			}

			ComboBox {
				id: items

				property bool inserting: false

				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.right: search.left
				anchors.margins: 10 * scaler.scaleMargins

				editable: true
				clip: true

				onEditTextChanged: {
					if(!inserting) {
						if(editText.length > 2)
							root.editTextChanged()
					}
					else
						inserting = false
				}

				onActivated: inserting = true

				BusyIndicator {
					id: sugIndicator

					anchors.right: parent.right
					anchors.rightMargin: 50 * scaler.scaleX
					anchors.verticalCenter: parent.verticalCenter
					height: parent.height * 0.8
					width: height

					visible: root.waitForSuggestions
					running: visible
				}
			}
		}

		Item {
			id: placeholder

			anchors.top: toprect.bottom
			anchors.bottom: parent.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			onChildrenChanged: {
				for(var i=0; i<children.length; ++i) {
					children[i].anchors.fill = placeholder
				}
			}
		}

		//		Rectangle {
		//			id: buttonrect

		//			anchors.bottom: parent.bottom
		//			anchors.left: parent.left
		//			anchors.right: parent.right

		//			height: 100 * scaler.scaleY
		//			color: "transparent"

		//			}

		//			BusyIndicator {
		//				id: busyInd

		//				anchors.left: parent.left
		//				anchors.verticalCenter: parent.verticalCenter
		//				height: Math.min(parent.height * 0.7, btncancel.x - x)
		//				width: height

		//				visible:false
		//				running: visible
		//			}

		Menu {
			id: suggestions

			Instantiator {
				id: creator

				model:[]

				MenuItem {
					text: modelData
					onTriggered: {
						items.inserting = true
						items.editText = text
						items.inserting = false
					}
				}
				onObjectAdded: suggestions.insertItem(index, object)
				onObjectRemoved: suggestions.removeItem(object)
			}
		}

		MessageDialog {
			id: alertDlg
			visible: false
			icon: StandardIcon.Warning
		}

		SequentialAnimation {
			id:colorAnim

			property color from: "lightsteelblue"
			property color to: "#6A7685"

			loops: Animation.Infinite
			running: false

			ColorAnimation {
				duration: 300

				target: root
				from: colorAnim.from
				to: colorAnim.to
			}

			ColorAnimation {
				duration: 300

				target: root
				property: "color"
				to: colorAnim.from
				from: colorAnim.to
			}

			onRunningChanged: if(!running) root.color = from
		}
	}

	function showAlert(title, text) {
		alertDlg.title = title
		alertDlg.text = text
		alertDlg.open()
	}

	function doSuggestions(list) {
		if(root.waitForSuggestions) {
			root.waitForSuggestions = false
			creator.active = false
			creator.model = list
			creator.active = true
			suggestions.popup()
		}
	}

	states: [
		State {
			name: "busy"
			when: root.busy

			//			PropertyChanges {
			//				target: busyInd
			//				visible: true
			//			}

			PropertyChanges {
				target: colorAnim
				running: true
			}
		}
	]
}
