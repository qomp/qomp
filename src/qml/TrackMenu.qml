import QtQuick 2.5
import QtQuick.Controls 1.4

Menu {
	id: root

	signal toggle()
	signal download()
	signal remove()

	property alias canDownload: dwnld.visible

	MenuItem {
		text: qsTr("Play/Pause")
		onTriggered: root.toggle()
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
}
