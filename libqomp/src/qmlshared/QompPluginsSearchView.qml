import QtQuick 2.3
import QtQuick.Controls 1.2

Rectangle  {
	id: root

	signal checkBoxClicked(var index)
	property var model;

	color: "aliceblue"

	Keys.onReleased: {
		if (event.key === Qt.Key_Back && head.visible) {
			event.accepted = true
			rootView.model.rootIndex = rootView.model.parentModelIndex()
		}
	}

	Rectangle {
		id: head

		property alias text: headerText.text
		property var rootIndex;

		color: "steelblue"

		anchors.top: parent.top
		width: parent.width
		height: 60 * scaler.scaleY
		visible: rootView.model.rootIndex !== rootIndex

		Rectangle {
			id: backIco
			height: parent.height
			width: height
			color: "lightblue"

			Image {
				anchors.fill: parent
				anchors.margins: parent.height * 0.1
				fillMode: Image.PreserveAspectFit
				source: "qrc:///icons/arrow-left"
			}
		}

		Text {
			id: headerText
			anchors.fill: parent
			anchors.leftMargin: backIco.width + 5 * scaler.scaleX
			verticalAlignment: Text.AlignVCenter
			horizontalAlignment: Text.AlignHCenter
			clip: true
			font.pixelSize: parent.height / 2.5
		}

		Component.onCompleted: rootIndex = rootView.model.rootIndex

		MouseArea {
			anchors.fill:parent
			onClicked: {
				rootView.model.rootIndex = rootView.model.parentModelIndex()
			}
		}
	}

	ListView {
		id: rootView

		anchors.fill: parent
		anchors.topMargin: head.visible ? head.height : 0
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		model: VisualDataModel {
			id: dataModel
			model: root.model

			delegate: Rectangle {
				id: mainRow

				property QtObject list: ListView.view

				height: 60 * scaler.scaleY
				width: parent.width

				color: "transparent"

				QompCheckBox {
					id: chkbx

					readonly property int toggleState: 2

					anchors.verticalCenter: parent.verticalCenter
					anchors.left: parent.left
					anchors.leftMargin: 10 * scaler.scaleX

					visible: model.state !== undefined
					checked: visible ? model.state : false

					onClicked: {
						var curRoot = list.model.rootIndex
						var cur = index
						root.checkBoxClicked(list.model.modelIndex(index))
						model.state = chkbx.toggleState //cause changing root index
						//restore view's position
						list.model.rootIndex = curRoot
						list.currentIndex = cur
						list.positionViewAtIndex(cur, ListView.Visible)
					}
				}

				QompImage {
					id: image
					anchors.left: chkbx.visible ? chkbx.right : parent.left
					anchors.leftMargin: 5 * scaler.scaleX
					anchors.verticalCenter: parent.verticalCenter
					height: parent.height * 0.9
					width: height
					source: model.icon
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
					font.pixelSize: parent.height / 2.5
					text: model.text
				}

				MouseArea {
					anchors.fill: parent
					anchors.leftMargin: chkbx.visible ? chkbx.width * 1.5 : 0
					enabled: hasModelChildren
					onClicked: {
						head.text = model.text
						list.model.rootIndex = list.model.modelIndex(index)
						root.checkBoxClicked(list.model.rootIndex)
					}
				}
			}
		}
	}
}
