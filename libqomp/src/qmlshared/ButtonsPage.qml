import QtQuick 2.3

BasePage {
	id: root

	bottomOffset: ok.height + ok.anchors.margins*2 + bottomOffsetMargine

	QompButton {
		id: ok

		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: 15 * scaler.scaleMargins
		text: qsTr("OK")
		onClicked: {
			root.status = true
			root.accepted()
		}
	}

	QompButton {
		id: cancel

		anchors.right: ok.left
		anchors.bottom: parent.bottom
		anchors.margins: 15 * scaler.scaleMargins
		text: qsTr("Cancel")
		onClicked: {
			root.status = false
			root.accepted()
		}
	}
}
