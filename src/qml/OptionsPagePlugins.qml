import QtQuick 2.3
import "qrc:///qmlshared"

Item {
	id: root

	height: col.height
	Column {
		id: col
		spacing: 0

		onChildrenChanged: {
			if(children.length > 0) {
				height += children[children.length - 1].height
			}
		}
		width: parent.width
	}

	Component {
		id: creator

		OptionsEntry {
			property alias enabled: enbl.checked
			property alias name: name.text
			property alias version: ver.text
			property alias description: descr.text

			QompCheckBox {
				id: enbl

				checkBoxHeight: parent.height / 2
				anchors.left: parent.left
			}

			Item {
				id: txt
				anchors.fill: parent
				anchors.leftMargin: enbl.width
				clip: true

				Text {
					id: name
					anchors.top: parent.top
					height: parent.height * 3 / 5
					font.pixelSize: 22 * scaler.scaleFont
					verticalAlignment: Text.AlignBottom
				}
				Text {
					id: ver
					anchors.top: parent.top
					anchors.left: name.right
					height: name.height
					anchors.leftMargin: 10 * scaler.scaleX
					font.pixelSize: name.font.pixelSize
					verticalAlignment: name.verticalAlignment
				}
				Text {
					id: descr
					anchors.top: name.bottom
					height: parent.height * 2 / 5
					font.pixelSize: 18 * scaler.scaleFont
					verticalAlignment: Text.AlignTop
					wrapMode: Text.WordWrap
				}
			}
		}
	}

	function addPlugin(name, descr, version, enabled) {
		creator.createObject(col, {"enabled": enabled, "name": name,
								 "version": version, "description": descr})
	}

	function clearPlugins() {
		col.children = []
		col.height = 0;
	}

	function getSettings() {
		var list = []
		for(var i = 0; i < col.children.length; ++i) {
			list[i] = {"name": col.children[i].name, "enabled": col.children[i].enabled}
		}
		return list;
	}
}
