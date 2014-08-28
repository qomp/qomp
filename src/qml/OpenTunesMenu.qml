import QtQuick 2.3
import QtQuick.Controls 1.2

Menu {
	id: root

	property var model: []

	Instantiator {
		id: creator

		model: root.model
		MenuItem {
			property QtObject qompAct: modelData
			text: modelData.text
			onTriggered: qompAct.triggered()
		}
		onObjectAdded: root.insertItem(index, object)
		onObjectRemoved: root.removeItem(object)
	}
}
