import QtQuick 2.3
import QtQuick.Controls 1.2

Menu {
	id: mainMenu

	signal clear()
	signal open()
	signal loadPlaylist()
	signal savePlaylist()

	property alias repeatAll: repAll.checked

	title: ""

	MenuItem {
		text: qsTr("Open")
		onTriggered: mainMenu.open()
	}
	MenuItem {
		id: repAll
		text: qsTr("Repeat All")
		checkable: true
	}
	MenuItem {
		text: qsTr("Clear Playlist")
		onTriggered: mainMenu.clear()
	}
//	MenuItem {
//		text: qsTr("Load Playlist")
//		onTriggered: mainMenu.loadPlaylist()
//	}
//	MenuItem {
//		text: qsTr("Save Playlist")
//		onTriggered: mainMenu.savePlaylist()
//	}
	MenuItem {
		text: qsTr("Exit")
		onTriggered: Qt.quit()
	}
}
