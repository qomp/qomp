import QtQuick 2.9
import QtQuick.Layouts 1.3
import "qrc:///qmlshared"

QompBaseDialog {
	id: root

	property alias title: titleTxt.text
	property alias artist: artistTxt.text
	property alias album: albumTxt.text
	property alias bitrate: bitrateTxt.text
	property alias cover: img.source
	property alias description: descrTxt.text
	property alias genre: gnrTxt.text

	preferredWidth: parent.height > parent.width ? parent.width * 0.9 : parent.width * (img.visible ? 0.8 : 0.7)
	preferredHeight: parent.height > parent.width ? parent.height * (img.visible ? 0.7 : 0.5) : parent.height * 0.7

	content: GridLayout {
		id: gr

		anchors.fill: parent
		anchors.margins: 15 * scaler.scaleMargins
		columns: (parent.height >= parent.width || !img.visible) ? 1 : 2
//		rows: (parent.height < parent.width || !img.visible) ? 1 : 2
		rowSpacing: 10 * scaler.scaleMargins
		columnSpacing: rowSpacing
		clip: true

		QompImage {
			id: img
			anchors.margins: 0
			height: 256 * scaler.scaleY
			width: height
			visible: status == Image.Ready
			Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
			Layout.preferredWidth: width
			Layout.preferredHeight: height
		}

		GridLayout {
			columns: 2
			columnSpacing: 10 * scaler.scaleX
			rowSpacing: 5 * scaler.scaleY
			Layout.alignment: ( (parent.height >= parent.width && img.visible) ? Qt.AlignTop : Qt.AlignVCenter ) | Qt.AlignHCenter
			Layout.fillWidth: true

			Text {
				id: ttle
				text: qsTr("Title") + ":"
				font.pixelSize: 26 * scaler.scaleFont
				font.bold: true
				visible: titleTxt.visible
			}

			Text {
				id: titleTxt
				font.pixelSize: ttle.font.pixelSize
				wrapMode: Text.WordWrap
				Layout.fillWidth: true
				visible: text.length > 0
			}

			Text {
				text: qsTr("Artist") + ":"
				font.pixelSize: ttle.font.pixelSize
				font.bold: true
				visible: artistTxt.visible
			}

			Text {
				id: artistTxt
				font.pixelSize: ttle.font.pixelSize
				wrapMode: Text.WordWrap
				Layout.fillWidth: true
				visible: text.length > 0
			}

			Text {
				text: qsTr("Album") + ":"
				font.pixelSize: ttle.font.pixelSize
				font.bold: true
				visible: albumTxt.visible
			}

			Text {
				id: albumTxt
				font.pixelSize: ttle.font.pixelSize
				wrapMode: Text.WordWrap
				Layout.fillWidth: true
				visible: text.length > 0
			}

			Text {
				text: qsTr("Description") + ":"
				font.pixelSize: ttle.font.pixelSize
				font.bold: true
				visible: descrTxt.visible
			}

			Text {
				id: descrTxt
				font.pixelSize: ttle.font.pixelSize
				wrapMode: Text.WordWrap
				Layout.fillWidth: true
				visible: text.length > 0
			}

			Text {
				text: qsTr("Genre") + ":"
				font.pixelSize: ttle.font.pixelSize
				font.bold: true
				visible: gnrTxt.visible
			}

			Text {
				id: gnrTxt
				font.pixelSize: ttle.font.pixelSize
				wrapMode: Text.WordWrap
				Layout.fillWidth: true
				visible: text.length > 0
			}

			Text {
				text: qsTr("Bitrate") + ":"
				font.pixelSize: ttle.font.pixelSize
				font.bold: true
				visible: bitrateTxt.visible
			}

			Text {
				id: bitrateTxt
				font.pixelSize: ttle.font.pixelSize
				wrapMode: Text.WordWrap
				Layout.fillWidth: true
				visible: text.length > 0
			}
		}
	}
}
