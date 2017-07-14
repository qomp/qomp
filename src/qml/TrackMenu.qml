import QtQuick 2.5
import QtQuick.Controls 1.4

Menu {
	id: root

	signal toggle()
	signal download()
	signal remove()
	signal tuneInfo()
	signal copyUrl()
	signal openUrl

	property alias canDownload: dwnld.visible
	property alias hasUrl: cpUrl.visible
	property alias hasDirectUrl: opnUrl.visible

	MenuItem {
		text: qsTr("Play/Pause")
		onTriggered: root.toggle()
	}

	MenuItem {
		id: cpUrl
		text: qsTr("Copy URL")
		onTriggered: root.copyUrl()
	}

	MenuItem {
		id: opnUrl
		text: qsTr("Open At Browser")
		onTriggered: root.openUrl()
	}

	MenuItem {
		id: dwnld
		text: qsTr("Download")
		onTriggered: root.download()
	}

	MenuItem {
		text: qsTr("Remove")
		onTriggered: root.remove()
	}

	MenuItem {
		text: qsTr("Track information")
		onTriggered: root.tuneInfo()
	}
}
