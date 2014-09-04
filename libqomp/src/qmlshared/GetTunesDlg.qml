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

	property bool waitForSuggestions: false

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

				QompBusyIndicator {
					id: sugIndicator

					anchors.right: parent.right
					anchors.rightMargin: 50 * scaler.scaleX
					anchors.verticalCenter: parent.verticalCenter
					height: parent.height * 0.8
					width: height

					visible: root.waitForSuggestions
					//running: visible
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
}
