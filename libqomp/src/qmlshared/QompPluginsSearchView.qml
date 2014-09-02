import QtQuick 2.3
import QtQuick.Controls 1.2

Item  {
	id: root

	signal checkBoxClicked(var index)
	property var model;

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

		Image {
			id: backIco
			height: parent.height
			width: height
			fillMode: Image.PreserveAspectFit
			source: "qrc:///icons/arrow-down"
			rotation: 90
		}

		Text {
			id: headerText
			anchors.fill: parent
			anchors.leftMargin: backIco.width + 5 * scaler.scaleY
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
			model: root.model

			delegate: Rectangle {
				id: mainRow

				property QtObject list: ListView.view

				height: 60 * scaler.scaleY
				width: parent.width

				color: "transparent"

				Loader {
					id: checkBoxLoader
					anchors.left: parent.left
					anchors.leftMargin: 10 * scaler.scaleX
					height: parent.height
					sourceComponent: checkBoxComponent
					active: model.state !== undefined
				}

				Component {
					id: checkBoxComponent

					QompCheckBox {
						id: chkbx

						readonly property int toggleState: 2
						anchors.verticalCenter: parent.verticalCenter

						checked: model.state

						onClicked: {
							if(!hasModelChildren) {
								list.currentIndex = index
								list.positionViewAtIndex(index,ListView.Visible)
							}
							root.checkBoxClicked(list.model.modelIndex(index))
							model.state = chkbx.toggleState
						}
					}
				}

				QompImage {
					id: image
					anchors.left: checkBoxLoader.active ? checkBoxLoader.right : parent.left
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
					anchors.leftMargin: checkBoxLoader.active ? checkBoxLoader.width * 1.5 : 0
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
