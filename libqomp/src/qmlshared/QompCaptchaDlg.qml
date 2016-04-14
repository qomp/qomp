import QtQuick 2.5

ButtonsPage {
	id: root

	property alias captcha: img.source
	property alias text: le.text
	property double aspect: 0.6 //imageHeight / imageWidth

	title: qsTr("Enter the text from the image")

	Image {
		id: img

		cache: false
		anchors.centerIn: parent

		width: root.width > root.height ? height / root.aspect : root.width / 1.5
		height: root.width > root.height ? root.height / 3 : width * root.aspect
	}

	LineEdit {
		id: le

		width: img.width
		height: 80 * scaler.scaleY
		anchors.top: img.bottom
		anchors.margins: 10 * scaler.scaleMargins
		anchors.horizontalCenter: parent.horizontalCenter
	}

}
