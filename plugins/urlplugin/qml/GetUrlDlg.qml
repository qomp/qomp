import QtQuick 2.5
import "qrc:///qmlshared"

ButtonsPage {
	id: root

	property alias url: textUrl.value

	content: TextLineEdit {
		id: textUrl

		text: qsTr("URL:")
		anchors.verticalCenter: parent.verticalCenter
		anchors.rightMargin: 10 * scaler.scaleX

		focus: true

		Keys.onReturnPressed: {
			Qt.inputMethod.hide()
			root.status = true
			root.accepted()
		}
	}
}
