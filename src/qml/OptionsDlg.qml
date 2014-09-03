import QtQuick 2.3
import QtQuick.Controls 1.2
import "qrc:///qmlshared"

BasePage {
	id: root

	property alias pages: placeholder
	property alias qompVer: aboutdlg.title

	title: qsTr("Options")

	content:  [

		Flickable {
			id: flic

			anchors.fill: parent
			contentHeight: placeholder.height + edge.height + about.height + space.height
			contentWidth: width

			clip: true
			boundsBehavior: Flickable.StopAtBounds

			Column {
				id: placeholder
				width: parent.width
				spacing: 0

				onChildrenChanged: {
					if(children.length > 0) {
						var i = children[children.length - 1]
						i.anchors.left = placeholder.left
						i.anchors.right = placeholder.right
						i.anchors.margins = 10 * scaler.scaleMargins
					}
				}
			}
			Line  {
				id: edge
				anchors.top: placeholder.bottom
				width: parent.width - 10 * scaler.scaleMargins
				anchors.horizontalCenter: parent.horizontalCenter
				height: 2
			}

			Rectangle {
				id: about

				anchors.left: parent.left
				anchors.right: parent.right
				anchors.top: edge.bottom
				anchors.margins: 10 * scaler.scaleMargins
				height: 80 * scaler.scaleY

				color: ma.pressed ? "lightsteelblue" : "lightblue"

				Behavior on color {
					ColorAnimation { duration: 100 }
				}

				Text {
					anchors.fill: parent
					text: qsTr("About qomp")
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
					font.pixelSize: height / 3
					font.bold: true
					color: ma.pressed ? "white" : "black"

					Behavior on color {
						ColorAnimation { duration: 100 }
					}
				}

				MouseArea {
					id: ma
					anchors.fill: parent
					onClicked: {
						aboutdlg.open()
					}
				}
			}
			Item {
				id: space
				height: 50
			}
		}
	]

	AboutDlg {
		id: aboutdlg
	}

	Component {
		id: comp

		Item {
			property alias text: txt.text
			height: 80 * scaler.scaleY

			Text {
				id: txt
				anchors.fill: parent
				anchors.margins: 10 * scaler.scaleX
				verticalAlignment: Text.AlignBottom
				horizontalAlignment: Text.AlignLeft
				font.pixelSize: parent.height / 2.5
				font.bold: true
			}
			Line {
				anchors.bottom: parent.bottom
			}
		}
	}

	function addPage(_title, _item) {
		var i = comp.createObject(placeholder, {"text": _title})
		_item.parent = placeholder
		placeholder.height += i.height + _item.height
	}
}
