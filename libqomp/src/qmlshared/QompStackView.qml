import QtQuick 2.9

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
			if(!running && deleteExit) {
				exitItem.destroy()
				exitItem = null
			}
		}
	}

	function push(item) {
		item.visible = Qt.binding(function() { return item.opacity > 0 })

		var doAnim = false
		var l = content.children.length
		if(l > 0) {
			anim.enterItem = item
			anim.exitItem = content.children[l - 1]
			doAnim = true
			anim.deleteExit = false
			item.opacity = 0
		}

		item.parent = content
		item.height = Qt.binding(function() { return content.height })
		item.width = Qt.binding(function() { return content.width })
		if(doAnim)
			anim.start()

		item.focus = true
	}

	function pop() {
		var l = content.children.length
		if(l < 2)
			return

		anim.enterItem = content.children[l - 2]
		anim.exitItem = content.children[l - 1]
		anim.deleteExit = true
		anim.start()
		anim.enterItem.focus = true
	}

	function clear() {
		for(var i = content.children.length - 1; i >= 0; --i) {
			var it = content.children[i]
			it.width = 0
			it.height = 0
			it.visible = false
			it.destroy()
		}
	}
}
