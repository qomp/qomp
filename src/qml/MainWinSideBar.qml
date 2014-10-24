import QtQuick 2.3
import "qrc:///qmlshared"

QompSideBar {
	id: root

	signal clear()
	signal open()
	signal loadPlaylist()
	signal savePlaylist()
	signal options()

	property alias repeatAll: repAll.checked
	property var model: []

	contents: [
		QompSideBarAction {
			text: qsTr("Open")
			enabled: false
		},
		Line{ height: 2 },
		Item {
			id: tunesMenu

			width: parent.width
			height: 0

			Instantiator {
				id: creator

				model: root.model
				QompSideBarAction {
					property QtObject qompAct: modelData
					textOffset: 55 * scaler.scaleX
					text: modelData.text
					onTriggered: {
						root.open();
						qompAct.triggered()
					}

					Line{}
				}
				onObjectAdded: {
					object.parent = tunesMenu
					object.y = object.height * index
					tunesMenu.height = tunesMenu.height + object.height
				}
				onObjectRemoved: {
					tunesMenu.height = tunesMenu.height - object.height
				}
			}
		},Line{ height: 2 },
		QompSideBarAction {
			id: repAll
			text: qsTr("Repeat All")
			checkable: true
		},
		Line{ height: 2 },
		QompSideBarAction {
			text: qsTr("Clear Playlist")
			onTriggered: root.clear()
		},
		Line{ height: 2 },
//		QompSideBarAction {
//			text: qsTr("Load Playlist")
//			onTriggered: root.loadPlaylist()
//		},Line{ height: 2 },
//		QompSideBarAction {
//			text: qsTr("Save Playlist")
//			onTriggered: root.savePlaylist()
//		},Line{ height: 2 },
		QompSideBarAction {
			text: qsTr("Options")
			onTriggered: root.options()
		},
		Line{ height: 2 },
		QompSideBarAction {
			text: qsTr("Exit")
			onTriggered: Qt.quit()
		}
	]
}
