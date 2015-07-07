import QtQuick 2.5
import Qt.labs.folderlistmodel 2.1
import QtQuick.Dialogs 1.2

FocusScope {
	id: root

	signal accepted()
	signal rejected()

	property bool selectFolders: false
	property bool singleSelect: true
	property bool selectExisting: false
	property alias title: pt.text
	property alias folder: folderModel.folder
	property var filter: []
	property var files: {"":true}
	property string file: ""

	Keys.onReleased: {
		if (event.key === Qt.Key_Back) {
			event.accepted = true
			if(rootView.header !== null)
				__back()
			else
				root.rejected()
		}
	}

	FolderListModel {
		id: folderModel

		showDirsFirst: true
		showFiles: !root.selectFolders
		sortField: FolderListModel.Name
	}

	Timer {
		id: updateTimer
		interval: 100
		repeat: false
		onTriggered: __doUpdate()
	}

	Rectangle {
		id: background
		anchors.fill: parent
		color: "lightblue"
	}

	PageTitle { id: pt }

	LineEdit {
		id: le

		anchors {
			margins: 10 * scaler.scaleMargins
			top: pt.bottom
			left: parent.left
			right: parent.right
		}
		width: parent.width
		height: 50 * scaler.scaleY

		input.focus: false

		Keys.onReturnPressed: root.__checkAndAccept()
		onTextChanged: root.file = le.text.length > 0 ? __ensureTrailingSlash(fullPath.text) + le.text : ""
	}

	Line { anchors.bottom: rootView.top }

	ListView {
		id: rootView

		property int transDuration: 300
		property bool transEnabled: true
		property var lastIndex:[]

		anchors {
			top: filterBox.bottom
			bottom: fullPath.top
			left: parent.left
			right: parent.right
			topMargin: 10 * scaler.scaleY
			bottomMargin: 10 * scaler.scaleY
		}

		boundsBehavior: Flickable.StopAtBounds

		focus: true
		clip: true

		highlightFollowsCurrentItem: true
		highlightMoveDuration: 0
		highlight: Rectangle { color: "#68828A" }

		model: folderModel

		delegate: Rectangle {
			id: mainRow

			property var list: ListView.view

			height: 60 * scaler.scaleY
			width: parent.width

			color: "transparent"

			Loader {
				id: checkBocxLoader
				sourceComponent: checkBoxComp
				active: !fileIsDir && !root.singleSelect
				anchors.left: parent.left
				anchors.leftMargin: 5 * scaler.scaleX
				height: parent.height
				width: height
			}

			Component {
				id: checkBoxComp

				QompCheckBox {
					id: chkbx

					readonly property int toggleState: 2
					anchors.centerIn: parent
					checked: model.fileURL in root.files

					onCheckedChanged: {
						list.currentIndex = index
						var str = model.fileURL
						if(checked) {
							root.files[str] = true
						}
						else {
							delete root.files[str]
						}
					}
				}
			}

			QompImage {
				id: image
				anchors.left: checkBocxLoader.active ? checkBocxLoader.right : parent.left
				anchors.leftMargin: 5 * scaler.scaleX
				anchors.verticalCenter: parent.verticalCenter
				height: parent.height * 0.9
				width: height
				source: {
					if(fileIsDir)
						return "qrc:///filedialog-folder"

					if(/\.qomp$/.exec(fileName) !== null)
						return "qrc:///filedialog-playlist"

					if(/\.mp3$/.exec(fileName) !== null)
						return "qrc:///icons/tune"

					return "qrc:///filedialog-file"
				}
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
				text: fileName
				runnning: index === list.currentIndex
				color: "transparent"
				defaultLabelAlingment: Text.AlignLeft
			}

			MouseArea {
				anchors.fill: parent
				anchors.leftMargin: checkBocxLoader.active ? checkBocxLoader.width * 1.5 : 0
				onClicked: {
					list.currentIndex = index
					if(fileIsDir) {
						var l = list.lastIndex.length
						list.lastIndex[l] = index
						list.model.folder = model.fileURL
						list.currentIndex = 0
						list.positionViewAtBeginning()
						list.updateCurrentFile()
					}
				}
			}
		}

		populate: Transition {
			enabled: rootView.transEnabled
			NumberAnimation { property: "opacity"; from: 0; to: 1; duration: rootView.transDuration }
		}
		remove: Transition {
			enabled: rootView.transEnabled
			NumberAnimation { property: "opacity"; to: 0; duration: rootView.transDuration }
		}
		add: Transition {
			enabled: rootView.transEnabled
			NumberAnimation { property: "opacity"; from: 0; to: 1; duration: rootView.transDuration }
		}

		onCurrentIndexChanged: updateCurrentFile()

		function updateCurrentFile() {
			updateTimer.start()
		}

		Component.onCompleted: currentIndex = 0
	}

	TickerLabel {
		id: fullPath
		runnning: true
		anchors {
			left: parent.left
			right: parent.right
			bottom: ok.top
			margins: 5 * scaler.scaleMargins
		}
		height: 50 * scaler.scaleY
		color: "lightsteelblue"
		defaultLabelAlingment: Text.AlignLeft
	}

	Component {
		id: headerComp

		Rectangle {
			id: head

			width: rootView.width
			height: visible ? 60 * scaler.scaleY : 0

			color: backer.pressed ? "cornflowerblue" : background.color
			Behavior on color { ColorAnimation { duration: 200 } }

			QompImage {
				id: img
				source: "qrc:///filedialog-up"
				anchors.left: parent.left
				anchors.leftMargin: 15 * scaler.scaleX
				height: parent.height
				width: height

				scale: backer.pressed ? 0.9 : 1
				Behavior on scale { NumberAnimation { duration: 200 } }
			}

			Line { anchors.bottom: parent.bottom }

			MouseArea {
				id: backer
				anchors.fill:parent
				onClicked: root.__back()
			}
		}
	}

	QompComboBox {
		id: filterBox

		availableHeight: root.height - y - height - ok.height*2
		readOnly: true

		readonly property var filters: /\(([^\)]+)\)/

		anchors {
			top: le.bottom
			left: parent.left
			right: parent.right
			margins: 10 * scaler.scaleMargins
		}
		height: 50 * scaler.scaleY

		model: root.filter

		onTextChanged: {
			var str = filters.exec(text)
			var arr = str[1].split(/,\s*/)
			folderModel.nameFilters = arr
		}
	}

	QompButton {
		id: ok

		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: 15 * scaler.scaleMargins
		text: qsTr("OK")
		onClicked: root.__checkAndAccept()
	}

	QompButton {
		id: cancel

		anchors.right: ok.left
		anchors.bottom: parent.bottom
		anchors.margins: 15 * scaler.scaleMargins
		text: qsTr("Cancel")
		onClicked: root.rejected()
	}

	MessageDialog {
		id: confirm
		title: qsTr("File already exists")
		text: qsTr("Overwrite existing file?")
		standardButtons: StandardButton.Ok | StandardButton.Cancel
		onAccepted: root.accepted()
	}

	function __ensureTrailingSlash(str) {
		if(str.charAt(str.length-1) !== "/")
			str += "/"

		return str
	}

	function __doUpdate() {
		root.file = ""
		le.text = ""
		fullPath.text = String(folderModel.folder).replace("file://", "")

		rootView.header = String(folderModel.parentFolder).length > 0 ? headerComp : null

		if(!root.selectFolders && folderModel.count > 0 &&
				!folderModel.get(rootView.currentIndex, "fileIsDir"))
		{
			var n = folderModel.get(rootView.currentIndex, "fileName")
			n = (n === undefined) ? "" : n
			le.text = n //root.file will be updated at onTextChanged slot of le
		}
	}

	function __checkAndAccept() {
		Qt.inputMethod.hide()
		if(!selectFolders && !selectExisting && singleSelect
				&& file === String(folderModel.get(rootView.currentIndex, "filePath")))
			confirm.open()
		else
			root.accepted()
	}

	function __back() {
		rootView.model.folder = rootView.model.parentFolder
		var l = rootView.lastIndex.length
		if(l > 0) {
			rootView.currentIndex = rootView.lastIndex[l-1]
			rootView.lastIndex.length = l-1
			rootView.positionViewAtIndex(rootView.currentIndex, ListView.Contain)
		}
		else {
			rootView.currentIndex = 0
		}

		rootView.updateCurrentFile()
	}
}
