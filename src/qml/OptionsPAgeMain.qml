import QtQuick 2.3
import "qrc:///qmlshared"

Item {
	id: root

	property alias autoStartPlay: autoplay.checked

	height: col.height
	Column {
		id: col
		spacing: 0

		Component.onCompleted: {
			for(var i = 0; i < children.length; ++ i) {
				height += children[i].height
			}
		}
		width: parent.width

		OptionsEntry {

			QompCheckBox {
				id: autoplay

				checkBoxHeight: parent.height / 2
				anchors.left: parent.left
			}

			Text {
				anchors.fill: parent
				anchors.leftMargin: autoplay.width
				text: qsTr("Automatically start playback")
				font.pixelSize: 22 * scaler.scaleFont
				wrapMode: Text.WordWrap
				verticalAlignment: Text.AlignVCenter
			}
		}
	}
}
