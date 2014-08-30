import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "qrc:///qmlshared"

Rectangle {
	id: root

	property alias model: view.model
	property alias page: curPage.text
	property alias auth: authState.text
	property alias nextEnabled: btnNext.enabled

	signal itemCheckClick(var index)
	signal actNext()

	color: "lightgray"

	QompPluginsSearchListView {
		id: view

		anchors.top: parent.top
		anchors.bottom: info.top
		anchors.left: parent.left
		anchors.right: parent.right

		onCheckBoxClicked: root.itemCheckClick(index)
	}

	Rectangle {
		id: info

		height: 80 * scaler.scaleY
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		color: "SkyBlue"

		Text {
			id: pageTxt

			anchors.left: parent.left
			anchors.top: parent.top
			height: parent.height/2
			width: Math.max(contentWidth, authTxt.contentWidth)
			anchors.leftMargin: 5 * scaler.scaleX
			font.pixelSize: height / 2
			verticalAlignment: Text.AlignVCenter
			text: qsTr("Current:")

		}

		Text {
			id: curPage

			anchors.left: pageTxt.right
			anchors.top: parent.top
			height: parent.height/2
			width: 80 * scaler.scaleX
			anchors.leftMargin: 5 * scaler.scaleX
			font.pixelSize: height / 2
			verticalAlignment: Text.AlignVCenter
			text: "0"
		}

		Item {
			anchors.left: curPage.right
			anchors.top: parent.top
			height: parent.height/2
			QompToolButton {
				id: btnNext

				anchors.centerIn: parent
				height: parent.height*0.9
				icon: "qrc:/icons/arrow-up"
				rotation: 90
				enabled: false

				onClicked: root.actNext()
			}
		}

		Text {
			id: authTxt

			anchors.left: parent.left
			anchors.bottom: parent.bottom
			height: parent.height/2
			width: Math.max(contentWidth, pageTxt.contentWidth)
			anchors.leftMargin: 5
			font.pixelSize: height / 2
			verticalAlignment: Text.AlignVCenter
			text: qsTr("Authorization: ")

		}

		Text {
			id: authState

			anchors.left: authTxt.right
			anchors.bottom: parent.bottom
			height: parent.height/2
			clip: true
			width: 200 * scaler.scaleX
			anchors.leftMargin: 5 * scaler.scaleX
			font.pixelSize: height / 2
			verticalAlignment: Text.AlignVCenter
		}
	}
}
