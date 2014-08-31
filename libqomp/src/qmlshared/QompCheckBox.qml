import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

CheckBox {
	id: root

	property int checkBoxHeight: parent.height * 0.8
	property int spacing: 0

	style: CheckBoxStyle {
		id: style
		indicator: Rectangle {
			id: checkRect
			color: "white"

			implicitWidth: implicitHeight
			implicitHeight: root.checkBoxHeight

			border.width: 2
			border.color: root.checked? "midnightblue" : "black"
			radius: 4 * scaler.scaleMargins

			QompImage {
				anchors.fill: parent
				anchors.margins: 5 * scaler.scaleMargins
				visible: root.checked
				source: "qrc:///icons/check"
			}
		}

		spacing: root.spacing
	}
}
