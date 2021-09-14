import QtQuick 2.12

Item {
	id: root

	property int depth: content.children.length
	property QtObject currentItem: content.children.length > 0 ?
						content.children[content.children.length - 1] : null

	Item {
		id: content
		anchors.fill: parent
	}

	ParallelAnimation {
		id: anim

		readonly property int animDuration: 300
		property QtObject enterItem
		property QtObject exitItem
		property bool deleteExit: false

		NumberAnimation {
			duration: anim.animDuration
			target: anim.enterItem
			property: "opacity"
			from: 0
			to: 1
		}
		NumberAnimation {
			duration: anim.animDuration
			target: anim.exitItem
			property: "opacity"
			from: 1
			to: 0
		}

		onRunningChanged: {
			if(!running) {
				if(deleteExit) {
					destoryItem(exitItem)
					exitItem = null
				}
				enterItem.focus = true
			}
		}
	}

	function push(item) {
		item.visible = Qt.binding(function() { return item.opacity > 0 })

		var doAnim = false
		if(depth > 0) {
			anim.enterItem = item
			anim.exitItem = content.children[depth - 1]
			doAnim = true
			anim.deleteExit = false
			item.opacity = 0
		}

		item.parent = content
		item.height = Qt.binding(function() { return content.height })
		item.width = Qt.binding(function() { return content.width })
		if(doAnim)
			anim.start()
	}

	function pop() {
		if(depth < 2)
			return

		anim.enterItem = content.children[depth - 2]
		anim.exitItem = content.children[depth - 1]
		anim.deleteExit = true
		anim.start()
	}

	function clear() {
		for(var i = depth - 1; i >= 0; --i) {
			var it = content.children[i]
			destoryItem(it)
		}
	}

	function destoryItem(item) {
		item.width = 0
		item.height = 0
		item.visible = false
		item.destroy()
	}
}
