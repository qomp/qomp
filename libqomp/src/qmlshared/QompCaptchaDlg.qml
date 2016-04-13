import QtQuick 2.5

ButtonsPage {
	id: root

	property alias captcha: img.source
	property alias text: le.text

	title: qsTr("Enter the text from the image")

	QompImage {
		id: img

		cache: false
		anchors.centerIn: parent
		width: 400 * scaler.scaleX
	}

	LineEdit {
		id: le

		width: 300 * scaler.scaleX
		height: 80 * scaler.scaleY
		anchors.top: img.bottom
		anchors.margins: 10 * scaler.scaleMargins
		anchors.horizontalCenter: parent.horizontalCenter
	}
}
