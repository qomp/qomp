import QtQuick 2.5

Rectangle {
	id: lineedit

	property alias text: textInput.text
	property alias input: textInput

	color: "white"
	radius: 5 * scaler.scaleMargins
	border.width: 2
	border.color: textInput.activeFocus ? "DodgerBlue" : "black"
	clip: true

	TextInput {
		id: textInput

		focus: true
		anchors.margins: 8 * scaler.scaleMargins
		anchors.fill: parent
//		inputMethodComposing: true
		selectByMouse: true
		verticalAlignment: TextInput.AlignVCenter
		horizontalAlignment: TextInput.AlignLeft

		font.pixelSize: 24 * scaler.scaleY
		//font.pointSize: 20
	}
}
