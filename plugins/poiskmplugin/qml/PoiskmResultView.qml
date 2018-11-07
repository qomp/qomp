import QtQuick 2.9
import "qrc:///qmlshared"

Item {
	id: root

	property alias model: view.model
	property alias page: curPage.text
	property alias nextEnabled: btnNext.enabled

	signal itemCheckClick(var index)
	signal actNext()

	QompPluginsSearchView {
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
			anchors.verticalCenter: parent.verticalCenter
			height: parent.height/2
			width: contentWidth
			anchors.leftMargin: 5 * scaler.scaleX
			font.pixelSize: height / 2
			verticalAlignment: Text.AlignVCenter
			text: qsTr("Current:")

		}

		Text {
			id: curPage

			anchors.left: pageTxt.right
			anchors.verticalCenter: parent.verticalCenter
			height: parent.height/2
			width: 80 * scaler.scaleX
			anchors.leftMargin: 5 * scaler.scaleX
			font.pixelSize: height / 2
			verticalAlignment: Text.AlignVCenter
			text: "0"
		}

		Item {
			anchors.left: curPage.right
			anchors.verticalCenter: parent.verticalCenter
			height: parent.height
			QompToolButton {
				id: btnNext

				anchors.centerIn: parent
				height: parent.height*0.7
				icon: "qrc:/icons/arrow-up"
				rotation: 90
				enabled: false

				onClicked: root.actNext()
			}
		}
	}
}
