import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

Item {
	id: root

	signal doSearch()
	signal accepted()
	signal rejected()

	property string title;
	property bool status: false
	property alias model: items.model
	property alias serchText: items.editText
	readonly property alias content: placeholder
	property bool busy: false

	Keys.onReleased: {
		if (event.key === Qt.Key_Back) {
			root.rejected()
		}
	}

	Rectangle {
		id: mainRect

		anchors.fill: parent
		color: "lightsteelblue"

		Rectangle {
			id: toprect

			anchors.top: parent.top
			width: parent.width
			height: 80

			color: "transparent"

			QompToolButton {
				id: search

				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				anchors.leftMargin: 10
				anchors.rightMargin: 10

				icon: "qrc:///icons/search"

				onClicked: {
					Qt.inputMethod.hide()
					root.doSearch()
				}
			}

			ComboBox {
				id: items

				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.right: search.left
				anchors.margins: 10

				editable: true
			}
		}

		Item {
			id: placeholder

			anchors.top: toprect.bottom
			anchors.bottom: buttonrect.top
			anchors.left: parent.left
			anchors.right: parent.right
			onChildrenChanged: {
				for(var i=0; i<children.length; ++i) {
					children[i].anchors.fill = placeholder
				}
			}
		}

		Rectangle {
			id: buttonrect

			anchors.bottom: parent.bottom
			anchors.left: parent.left
			anchors.right: parent.right

			height: 100
			color: "transparent"

			QompButton {
				id: btnok

				anchors.bottom: parent.bottom
				anchors.right: parent.right
				anchors.margins: 15

				text: qsTr("OK")

				onClicked: {
					root.status = true
					root.accepted()
				}
			}

			QompButton {
				id: btncancel

				anchors.bottom: parent.bottom
				anchors.right: btnok.left
				anchors.margins: 15

				text: qsTr("Cancel")

				onClicked: {
					root.status = false
					root.rejected()
				}
			}

			BusyIndicator {
				id: busyInd

				anchors.left: parent.left
				anchors.verticalCenter: parent.verticalCenter
				height: Math.min(parent.height * 0.7, btncancel.x - x)
				width: height

				visible:false
				running: visible
			}
		}

		Behavior on color {
			ColorAnimation { duration: 300 }
		}
	}

	states: [
		State {
			name: "busy"
			when: root.busy === true

			PropertyChanges {
				target: busyInd
				visible: true
			}

			PropertyChanges {
				target: mainRect
				color: Qt.darker("lightsteelblue")
			}
		}
	]

	MessageDialog {
		id: alertDlg
		visible: false
		icon: StandardIcon.Warning
	}

	function showAlert(title, text) {
		alertDlg.title = title
		alertDlg.text = text
		alertDlg.open()
	}
}
