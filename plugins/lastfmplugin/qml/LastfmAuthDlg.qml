import QtQuick 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import "qrc:///qmlshared"

ButtonsPage {
	id: root

	signal doAuth()

	Component.onCompleted: question.open()

	MessageDialog {
		id: question

		text: qsTr("Do you want to go to Last.Fm authorization page?")
		standardButtons: StandardButton.Ok | StandardButton.Cancel
		onAccepted: {
			root.showWidgets()
			root.doAuth()
		}
		onRejected: root.accepted()
	}

	content: [
		Text {
			id: txt
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.margins: 30 * scaler.scaleMargins

			text: qsTr("After completion of the authorization process in the browser, click OK button")
			wrapMode: Text.WordWrap
			font.pixelSize: 32 * scaler.scaleFont
			horizontalAlignment: Text.AlignHCenter
			visible: false
		},

		BusyIndicator {
			id: busy
			anchors.bottom: parent.bottom
			anchors.top: txt.bottom
			anchors.horizontalCenter: parent.horizontalCenter
			width: height
			running: visible
			visible: false
		}
	]

	function showWidgets() {
		busy.visible = true
		txt.visible = true
	}
}
