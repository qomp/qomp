import QtQuick 2.5

Item {
	id: root

	property var artistsModel;
	property var albumsModel;
	property var tracksModel
	property alias currentTab: itemView.currentIndex
	property var currentModel: {
						if(itemView && itemView.currentItem) {
							return itemView.currentItem.model
						}
						return undefined
					}

	signal itemCheckClick(var index)
	signal tabChanged(int tab)

	onCurrentTabChanged: tabChanged(currentTab)

	Rectangle {
		id: tabs
		anchors.top: root.top
		width: root.width
		height: 60 * scaler.scaleY

		color: "cornflowerblue"

		Row {
			id: row

			spacing: Math.ceil(1 * scaler.scaleX)

			Repeater {
				model: [qsTr("Artists"), qsTr("Albums"), qsTr("Tracks")]

				Rectangle {
					property bool current: root.currentTab === index

					height: tabs.height
					width: tabs.width / 3
					color: current ? "lightslategray" : "lavender"

					Text {
						anchors.centerIn: parent
						font.pixelSize: parent.height / 3
						text: modelData
						color: current ? "white" : "black"
					}

					MouseArea {
						anchors.fill: parent
						onClicked: root.currentTab = index
					}
				}
			}
		}
	}

	Component {
		id: view

		QompPluginsSearchView {
			width: root.width
			height: root.height - tabs.height

			color: VisualItemModel.index === root.currentTab ? "aliceblue" : "lightblue"
			Behavior on color {
				ColorAnimation { duration: itemView.highlightMoveDuration }
			}

			onCheckBoxClicked: root.itemCheckClick(index)
		}
	}

	VisualItemModel {
		id: itemModel

		Component.onCompleted: {
			itemModel.children = [
					view.createObject(root, { "model": Qt.binding(function() { return root.artistsModel } ) }),
					view.createObject(root, { "model": Qt.binding(function() { return root.albumsModel  } ) }),
					view.createObject(root, { "model": Qt.binding(function() { return root.tracksModel  } ) }) ]
		}
	}

	ListView {
		id: itemView

		anchors.fill: parent
		anchors.topMargin: tabs.height

		model: itemModel
		snapMode: ListView.SnapOneItem
		boundsBehavior: Flickable.StopAtBounds
		highlightRangeMode: ListView.StrictlyEnforceRange
		highlightMoveDuration: 500
		clip: true
		orientation: Qt.Horizontal

		rebound: Transition {
			NumberAnimation {
				properties: "x, y"
				easing.type: Easing.InOutQuad
				duration: itemView.highlightMoveDuration
			}
		}

		onCurrentItemChanged: if(currentItem) currentItem.forceActiveFocus()
	}
}
