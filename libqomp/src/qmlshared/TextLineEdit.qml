import QtQuick 2.12

FocusScope {
	id: textlineedit

	property alias text: label.text;
	property alias value: lineedit.text;
	property alias input: lineedit.input
	property real textRatio: 1 / 3
	property alias textFontSize: label.font.pointSize

	height: 68 * scaler.scaleY

	anchors.left: parent.left
	anchors.right: parent.right

	Item {
		id: rectangle

		anchors.fill: parent

		Text {
			id: label
			wrapMode: Text.WrapAtWordBoundaryOrAnywhere
			verticalAlignment: Text.AlignVCenter
			horizontalAlignment: Text.AlignRight
			anchors {
				fill: parent
				margins: 8 * scaler.scaleMargins
				rightMargin: rectangle.width * (1 - textRatio)
			}

			font.pointSize: 18
		}

		LineEdit {
			id: lineedit

			anchors {
				fill: parent
				margins: 8 * scaler.scaleMargins
				leftMargin: rectangle.width * textRatio + 20
			}
			//focus: true
		}
	}
}
