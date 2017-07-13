import QtQuick 2.5
import "qrc:///qmlshared"

Rectangle {
	id: root

	signal actNext()
	signal actPrev()
	signal actStop()
	signal actPlay()
	signal actRepeat(bool rep)
	signal actShuffle(bool shuf)
	signal positionChanged(int pos)

	signal actClearPlaylist()
	signal actLoadPlaylist(string str)
	signal actSavePlaylist(string str)
	signal actDoOpenMenu()
	signal actDoOptions()

	signal itemActivated(int index)

	signal actToggle(int index)
	signal actRemove(int index)
	signal actDownload(int index, string dir)

	property string title: Qt.application.name

	property alias currentDuration: position.max
	property alias currentPosition: position.value
	property alias currentDurationText: totalDurTxt.text
	property alias currentPositionText: curPosTxt.text
	property alias playlistModel: playlist.model

	property var  pluginsActions: []
	property bool playing: false
	property bool busy: false
	property bool repeat: false
	property bool shuffle: false
	property string totalDuration;
	property string currentFolder

	color: "lightblue"

	Keys.onReleased: {
		if (event.key === Qt.Key_Menu) {
			event.accepted = true
			menuButton.expanded = true
		}
		if (event.key === Qt.Key_Back && menuButton.expanded ) {
			event.accepted = true
			menuButton.expanded = false
		}
	}

	PageTitle {
		id: title

		property bool shouldAnimate: {
			if(!root.playing || playlist.currentTrackText.length === 0)
				return false
			return true
		}

		text: shouldAnimate ? playlist.currentTrackText : root.title
		defaultLabelAlingment: shouldAnimate ? Text.AlignLeft : Text.AlignHCenter
		runnning: shouldAnimate
		textOffset: menuButton.width

		QompMenuButton {
			id: menuButton
			height: title.height
			anchors.left: title.left
			anchors.top: title.top

			onClicked: if(!sideBarLoader.active) sideBarLoader.active = true
		}
	}

	ListView {
		id: playlist

		property string currentTrackText;

		focus: true

		anchors.top: title.bottom
		width: parent.width
		anchors.bottom: controls.top

		snapMode: ListView.SnapToItem
		clip: true
		boundsBehavior: Flickable.StopAtBounds

		highlightFollowsCurrentItem: false

		model: []

		delegate: PlayListDelegate {
			property variant curData: model

			busy: root.busy && root.visible
			playing: root.playing
			width: playlist.width

			onLongTap: {
				if(!trackMenu.active)
					trackMenu.active = true

				trackMenu.item.canDownload = model.canDownload
				trackMenu.item.popup()
			}
			onActivated: root.itemActivated(index)
			onCurrentChanged: if(current) playlist.currentTrackText = Qt.binding(function(){ return model.text })
		}

		move: Transition {
			SequentialAnimation {
				NumberAnimation {
					property: "opacity"
					from: 1
					to: 0
					duration: 100
				}
				PropertyAnimation {
					properties: "x, y"
					duration: 300
				}
				NumberAnimation {
					property: "opacity"
					from: 0
					to: 1
					duration: 100
				}
			}
		}

		displaced: Transition {
			SequentialAnimation {
				NumberAnimation {
					property: "opacity"
					from: 1
					to: 0.5
					duration: 100
				}
				PropertyAnimation {
					properties: "x, y"
					duration: 300
				}
				NumberAnimation {
					property: "opacity"
					from: 0.5
					to: 1
					duration: 100
				}
			}
		}
	}

	Rectangle {
		id: controls
		height: 150 * scaler.scaleY
		anchors.bottom: parent.bottom
		color: "SteelBlue"
		width: parent.width
		anchors.horizontalCenter: parent.horizontalCenter

		Item {
			id: posItem
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			height: controls.height / 5
			anchors.margins: 10 * scaler.scaleMargins

			Text {
				id: curPosTxt
				height: parent.height
				anchors.left: parent.left
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignLeft
				width: 60 * scaler.scaleX
				font.pixelSize: parent.height / 1.5
				color: "white"
			}

			Text {
				id: totalDurTxt
				height: parent.height
				anchors.right: parent.right
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignRight
				width: curPosTxt.width
				font.pixelSize: curPosTxt.font.pixelSize
				color: "white"
			}

			QompSlider {
				id: position

				anchors.left: curPosTxt.right
				anchors.right: totalDurTxt.left
				anchors.margins: 10 * scaler.scaleMargins

				onValueChanged: if (pressed) root.positionChanged(Math.round(value))
			}
		}

		Item {
			anchors.top: posItem.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: parent.bottom
			anchors.margins: 10 * scaler.scaleMargins

			Row {
				anchors.horizontalCenter: parent.horizontalCenter
				height: parent.height

				QompToolButton {
					id: prev
					icon: "qrc:///icons/prev"
					anchors.verticalCenter: parent.verticalCenter
					onClicked: root.actPrev()
				}
				QompToolButton {
					id: play
					icon:root.playing ? "qrc:///icons/pause" : "qrc:///icons/play"
					anchors.verticalCenter: parent.verticalCenter
					onClicked: root.actPlay()
				}
				QompToolButton {
					id: stop
					icon: "qrc:///icons/stop"
					anchors.verticalCenter: parent.verticalCenter
					onClicked: root.actStop()
				}
				QompToolButton {
					id: next
					icon: "qrc:///icons/next"
					anchors.verticalCenter: parent.verticalCenter
					onClicked: root.actNext()
				}
			}
		}
	}

	Loader {
		id: sideBarLoader

		anchors.top: title.bottom
		anchors.bottom: parent.bottom
		width: parent.width

		sourceComponent: sideBarComp
		active: false
	}

	Component {
		id: sideBarComp

		MainWinSideBar {
			model: root.pluginsActions
			expanded: menuButton.expanded
			repeatAll: root.repeat
			shuffle: root.shuffle

			onOpen: menuButton.expanded = false
			onClear: root.actClearPlaylist()
			onOptions: {
				menuButton.expanded = false
				root.actDoOptions()
			}
			onRepeatAllChanged: root.actRepeat(repeatAll)
			onShuffleChanged: root.actShuffle(shuffle)

			onLoadPlaylist: {
				menuButton.expanded = false
				fileDialog.active = true
				fileDialog.item.selectExisting = true
				fileDialog.item.title = qsTr("Select Playlist")
				fileDialog.item.selectFolders = false
				fileDialog.item.onDialogAccepted = function() {
					root.actLoadPlaylist(fileDialog.item.file)
				}
				fileDialog.item.filter = [qsTr("qomp playlist (*.qomp)"),
										  qsTr("Other playlists (*.m3u, *.m3u8, *.pls)")]
				fileDialog.item.visible = true
				fileDialog.item.forceActiveFocus()
			}
			onSavePlaylist: {
				menuButton.expanded = false
				fileDialog.active = true
				fileDialog.item.selectExisting = false
				fileDialog.item.title = qsTr("Select Playlist")
				fileDialog.item.selectFolders = false
				fileDialog.item.onDialogAccepted = function() {
					root.actSavePlaylist(fileDialog.item.file)
				}
				fileDialog.item.filter = [(qsTr("qomp playlist (*.qomp)"))]
				fileDialog.item.visible = true
				fileDialog.item.forceActiveFocus()
			}
		}
	}

	Loader {
		id: trackMenu

		sourceComponent: trackMenuComp
		active: false
	}

	Component {
		id: trackMenuComp

		TrackMenu {
			onToggle: root.actToggle(playlist.currentIndex)
			onRemove: root.actRemove(playlist.currentIndex)
			onDownload: {
				fileDialog.active = true
				fileDialog.item.forIndex = playlist.currentIndex
				fileDialog.item.title = qsTr("Select directory")
				fileDialog.item.selectFolders = true
				fileDialog.item.onDialogAccepted = function() {
					root.actDownload(fileDialog.item.forIndex, fileDialog.item.folder)
				}
				fileDialog.item.filter = []
				fileDialog.item.visible = true
				fileDialog.item.forceActiveFocus()
			}
			onTuneInfo: root.doTuneInfo()
		}
	}

	Loader {
		id: fileDialog

		anchors.fill: parent
		sourceComponent: fileDialogComp
		active: false
	}

	Component {
		id: fileDialogComp

		QompFileDlg {
			property var onDialogAccepted
			property int forIndex

			anchors.fill: parent
			folder: root.currentFolder
			singleSelect: true
			visible: false

			onRejected: fileDialog.active = false
			onAccepted: {
				root.currentFolder = folder
				onDialogAccepted()
				fileDialog.active = false
			}
		}
	}

	Loader {
		id: tuneInfoLoader
		sourceComponent: tuneInfoComp
		active: false
		anchors.fill: parent
	}

	Component {
		id: tuneInfoComp

		TuneInfoDlg {

		}
	}

	function enshureItemVisible(index) {
		playlist.currentIndex = index
		playlist.positionViewAtIndex(index, ListView.Contain)
	}

	function doTuneInfo() {
		if (!tuneInfoLoader.active)
			tuneInfoLoader.active = true

		var cur = playlist.currentItem.curData

		tuneInfoLoader.item.artist = cur.artist
		tuneInfoLoader.item.album = cur.album
		tuneInfoLoader.item.title = cur.title
		tuneInfoLoader.item.bitrate = cur.bitrate
		tuneInfoLoader.item.cover = cur.cover
//		console.log(cur.cover)
		tuneInfoLoader.item.open()
		tuneInfoLoader.item.forceActiveFocus()
	}
}
