import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

CheckBox {
	id: root

	style: CheckBoxStyle {
		id: style
		indicator: Rectangle {
			color: "white"

			implicitWidth: root.parent.height * 0.8
			implicitHeight: implicitWidth

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

		spacing:0
	}
}
