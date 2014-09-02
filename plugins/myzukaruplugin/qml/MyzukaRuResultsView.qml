import QtQuick 2.3
import "qrc:///qmlshared"

Rectangle {
	id: root

	property alias artistsModel: view.model
	signal itemCheckClick(var index)

	color: "lightgray"

	QompPluginsSearchView {
		id: view

		anchors.fill: parent

		onCheckBoxClicked: root.itemCheckClick(index)
	}
}
