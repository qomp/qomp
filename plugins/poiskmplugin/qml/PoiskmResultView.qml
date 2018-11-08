import QtQuick 2.9
import "qrc:///qmlshared"

Item {
	id: root

	property alias model: view.model
	property alias page: curPage.text
	property alias nextEnabled: btnNext.enabled

	readonly property int fontSize: 25 * scaler.scaleY

	signal itemCheckClick(var index)
	signal actNext()
	signal selectAllClicked(var select)

	Row {
		id: checkboxRow

		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
			margins: 10 * scaler.scaleMargins
		}
		height: 60 * scaler.scaleY

		QompCheckBox {
			id: selecAll

			anchors.verticalCenter: parent.verticalCenter
			onCheckedChanged: selectAllClicked(checked)
		}

		Text {
			text: qsTr("Select all")
			anchors.verticalCenter: parent.verticalCenter
			height: parent.height
			anchors.leftMargin: 10 * scaler.scaleX
			font.pixelSize: fontSize
			verticalAlignment: Text.AlignVCenter
		}
	}

	QompPluginsSearchView {
		id: view

		anchors.top: checkboxRow.bottom
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
			height: parent.height
			width: contentWidth
			anchors.leftMargin: 10 * scaler.scaleX
			font.pixelSize: fontSize
			verticalAlignment: Text.AlignVCenter
			text: qsTr("Current:")

		}

		Text {
			id: curPage

			anchors.left: pageTxt.right
			anchors.verticalCenter: parent.verticalCenter
			height: parent.height
			width: 80 * scaler.scaleX
			anchors.leftMargin: 5 * scaler.scaleX
			font.pixelSize: fontSize
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
