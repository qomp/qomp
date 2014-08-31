import QtQuick 2.3
import QtQuick.Controls 1.2

ListView {
	id: root

	signal checkBoxClicked(var index)

	clip: true

//	snapMode: ListView.SnapToItem
	boundsBehavior: Flickable.StopAtBounds

	delegate: Rectangle {
		id: delegate

		height: 60 * scaler.scaleY
		width: parent.width
		color: "transparent"

		QompCheckBox {
			id: chkbx

			anchors.left: parent.left
			anchors.leftMargin: 10 * scaler.scaleX
			anchors.verticalCenter: parent.verticalCenter
			checked: model.state

			onClicked: {
				root.currentIndex = index
				root.positionViewAtIndex(index,ListView.Visible)
				root.checkBoxClicked(index)
			}
		}

		QompImage {
			id: image
			anchors.left: chkbx.right
			anchors.leftMargin: 5 * scaler.scaleX
			anchors.verticalCenter: parent.verticalCenter
			height: delegate.height * 0.9
			width: height
			source: "image://qomp/"+model.icon
		}

		Text {
			id: txt
			anchors.left: image.right
			anchors.leftMargin: 5 * scaler.scaleX
			anchors.right: parent.right
			anchors.rightMargin: 5 * scaler.scaleX
			elide: Text.ElideRight
			height: parent.height
			clip: true
			verticalAlignment: Text.AlignVCenter
			font.pixelSize: delegate.height / 2.5
			text: model.text
		}
	}
}
