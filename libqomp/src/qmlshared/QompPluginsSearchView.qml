import QtQuick 2.3

Rectangle  {
	id: root

	signal checkBoxClicked(var index)
	property var model;

	color: "aliceblue"

	Keys.onReleased: {
		if (event.key === Qt.Key_Back && head.visible) {
			event.accepted = true
			head.back()
		}
	}

	Rectangle {
		id: head

		property alias text: headerText.text
		property var rootIndex;
		property int oldIndex: 0;

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

		TickerLabel {
			id: headerText
			anchors.fill: parent
			anchors.leftMargin: backIco.width + 5 * scaler.scaleX
			font.pixelSize: parent.height / 2.5
			runnning: true
			color: "transparent"
		}

		Component.onCompleted: rootIndex = rootView.model.rootIndex

		MouseArea {
			anchors.fill:parent
			onClicked: head.back()
		}

		function back() {
			rootView.model.rootIndex = rootView.model.parentModelIndex()
			rootView.currentIndex = oldIndex
			rootView.positionViewAtIndex(oldIndex, ListView.Visible)
		}
	}

	ListView {
		id: rootView

		property int transDuration: 300
		property bool transEnabled: true

		anchors.fill: parent
		anchors.topMargin: head.visible ? head.height : 0
		boundsBehavior: Flickable.StopAtBounds

		highlightFollowsCurrentItem: true
		highlightMoveDuration: 0
		highlight: Rectangle { color: "#68828A" }

		clip: true

		model: VisualDataModel {
			id: dataModel
			model: root.model

			delegate: Rectangle {
				id: mainRow

				property var list: ListView.view

				height: 60 * scaler.scaleY
				width: parent.width

				color: "transparent"

				QompCheckBox {
					id: chkbx

					readonly property int toggleState: 2

					anchors.left: parent.left
					anchors.leftMargin: 10 * scaler.scaleX

					visible: model.state !== undefined
					checked: visible ? model.state : false

					onClicked: {
						root.checkBoxClicked(list.model.modelIndex(index))
						model.state = chkbx.toggleState
						list.currentIndex = index
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

				TickerLabel {
					id: txt
					anchors.left: image.right
					anchors.leftMargin: 5 * scaler.scaleX
					anchors.right: parent.right
					anchors.rightMargin: 5 * scaler.scaleX
					elide: Text.ElideRight
					height: parent.height
					font.pixelSize: parent.height / 2.5
					text: model.text
					runnning: index === list.currentIndex
					color: "transparent"
					defaultLabelAlingment: Text.AlignLeft
				}

				MouseArea {
					anchors.fill: parent
					anchors.leftMargin: chkbx.visible ? chkbx.width * 1.5 : 0
					onClicked: {
						list.currentIndex = index
						if(hasModelChildren) {
							head.oldIndex = index
							head.text = model.text
							list.model.rootIndex = list.model.modelIndex(index)
							root.checkBoxClicked(list.model.rootIndex)
						}
					}
				}
			}
		}
		populate: Transition {
			enabled: rootView.transEnabled
			NumberAnimation { properties: "x,y"; duration: rootView.transDuration }
		}
		remove: Transition {
			enabled: rootView.transEnabled
			NumberAnimation { property: "opacity"; to: 0; duration: rootView.transDuration }
		}
		add: Transition {
			enabled: rootView.transEnabled
			NumberAnimation { property: "opacity"; from: 0; to: 1; duration: rootView.transDuration }
		}
	}
}
