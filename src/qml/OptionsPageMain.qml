import QtQuick 2.3
import "qrc:///qmlshared"
import net.sourceforge.qomp 1.0
import QtQuick.Dialogs 1.2

Item {
	id: root

	property alias autoStartPlay: autoplay.checked
	property alias metadataEncoding: encoding.value
	property alias langs: lang.model
	property alias curLang: lang.text

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

		OptionsEntry {

			TextLineEdit {
				id: encoding

				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Default Metadata Encoding:")
				input.inputMethodHints: Qt.ImhPreferUppercase | Qt.ImhNoPredictiveText
				textRatio: 2 / 3
			}
		}

		OptionsEntry {

			z: 1000 //combobox dropdown should be on top

			Text {
				id: langTxt

				text: qsTr("Language:")
				width: parent.width * 2 / 3
				height: parent.height
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignRight
				font.pixelSize: 22 * scaler.scaleFont
			}

			QompComboBox {
				id: lang
				anchors.fill: parent
				anchors.margins: 20 * scaler.scaleY
				anchors.leftMargin: langTxt.width + 16 * scaler.scaleMargins
				anchors.rightMargin: 8 * scaler.scaleMargins
				availableHeight: 250 * scaler.scaleY
				model: []
				readOnly: true
			}
		}

		OptionsEntry {

			QompButton {
				anchors.centerIn: parent
				text: qsTr("Check for updates")
				width: parent.width * 0.7
				onClicked: uc.startCheck(false)
			}
		}
	}

	UpdatesChecker {
		id: uc

		onFinished: {
			if(hasUpdate) {
				md.standardButtons = StandardButton.Yes | StandardButton.No
				md.title = qsTr("New version is available")
				md.text = qsTr("Do you want to go to the download page?")
			}
			else {
				md.standardButtons = StandardButton.Ok
				md.title = ""
				md.text = qsTr("There is no updates found.")
			}
			md.open()
		}
	}

	MessageDialog {
		id: md

		onYes: Qt.openUrlExternally("http://sourceforge.net/projects/qomp/files/")
	}
}
