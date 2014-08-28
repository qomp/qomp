import QtQuick 2.3
import QtQuick.Controls 1.2
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

	Component.onCompleted: root.nextEnabled = false

	ListView {
		id: view

		anchors.top: parent.top
		anchors.bottom: info.top
		anchors.left: parent.left
		anchors.right: parent.right

		clip: true

//		snapMode: ListView.SnapToItem
		boundsBehavior: Flickable.StopAtBounds

		delegate: Rectangle {
			id: delegate

			height: 60
			width: parent.width
			color: "transparent"

			CheckBox {
				id: chkbx

				anchors.left: parent.left
				anchors.leftMargin: 10
				anchors.verticalCenter: parent.verticalCenter
				checked: model.state

				onClicked: {
					view.currentIndex = index
					view.positionViewAtIndex(index,ListView.Visible)
					root.itemCheckClick(index)
				}
			}

			QompImage {
				id: image
				anchors.left: chkbx.right
				anchors.leftMargin: 10
				anchors.verticalCenter: parent.verticalCenter
				height: delegate.height * 0.9
				width: height
				source: "image://qomp/"+model.icon
			}

			Text {
				id: txt
				anchors.left: image.right
				anchors.right: parent.right
				height: parent.height
				clip: true
				verticalAlignment: Text.AlignVCenter
				font.pixelSize: 18
				text: model.text
			}
		}
	}

	Rectangle {
		id: info

		height: 80
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
			anchors.leftMargin: 5
			font.pixelSize: height / 2
			verticalAlignment: Text.AlignVCenter
			text: qsTr("Current Page:")

		}

		Text {
			id: curPage

			anchors.left: pageTxt.right
			anchors.top: parent.top
			height: parent.height/2
			width: 80
			anchors.leftMargin: 5
			font.pixelSize: height / 2
			verticalAlignment: Text.AlignVCenter
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
			text: qsTr("Authorization:")

		}

		Text {
			id: authState

			anchors.left: authTxt.right
			anchors.bottom: parent.bottom
			height: parent.height/2
			clip: true
			width: 200
			anchors.leftMargin: 5
			font.pixelSize: height / 2
			verticalAlignment: Text.AlignVCenter
		}
	}
}
