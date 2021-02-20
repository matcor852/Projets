import QtQuick.Window 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0


Window {
    id: root
    width: 640
    height: 480
    visible: true

    Connections {
        target: lpServeur
        function onRoomInfos(owner,nbPlayers,playersName,dareList,truthList)
        {
            roomOwnerDisplay.text = owner;
            nbPlayersDisplay.text = nbPlayers;
            namePlayersDisplay.text = playersName;
            dareDisplay.text = dareList;
            truthDisplay.text = truthList;
        }
        function onNewRoom(roomId)
        {
            roomDisplayModel.append({room: roomId})
        }
        function onLessRoom(roomId)
        {
            for(var i=roomDisplayModel.count-1; i>=0; --i)
            {
                if(parseInt(roomDisplayModel.get(i).room) === parseInt(roomId))
                {
                    roomDisplayModel.remove(i);
                    return;
                }
            }
        }
    }
    ListModel {
        id: roomDisplayModel
    }

    Text {
        id: networkInfos
        text: lpServeur.updatedNetworkInfos()
        height: parent.height/25
        width: parent.width
        anchors.top: parent.top
    }
    ListView {
        id: list
        anchors.top: networkInfos.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: parent.width * 1/3
        model: roomDisplayModel
        delegate: Component {
            Item {
                width: root.width * 1/3
                height: 40
                Column {
                    Text { text: '\n\t' + room }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: list.currentIndex = index
                }
            }
        }
        highlight: Rectangle {
            color: 'grey'
        }
        focus: true
        onCurrentItemChanged:
        {
            if(!roomDisplayModel.count > 0)
            {
                roomOwnerDisplay.text = "";
                nbPlayersDisplay.text = "";
                namePlayersDisplay.text = "";
                dareDisplay.text = "";
                truthDisplay.text = "";
                return;
            }
            lpServeur.roomSelected(roomDisplayModel.get(list.currentIndex).room)

        }
    }
    Item {
        id: roomContentDisplay
        anchors.top: networkInfos.bottom
        anchors.bottom: parent.bottom
        anchors.left: list.right
        width: parent.width * 2/3
        Row {
            id: r0
            topPadding: 10
            bottomPadding: 10
            anchors.top: parent.top
            Text {text: "owner : "}
            Text {
                id: roomOwnerDisplay
                text: ""
            }
        }
        Row {
            id: r1
            topPadding: 10
            bottomPadding: 10
            anchors.top: r0.bottom
            Text {text: "nbPlayers : "}
            Text {
                id: nbPlayersDisplay
                text: ""
            }
        }
        Row {
            id: r2
            topPadding: 10
            bottomPadding: 10
            anchors.top: r1.bottom
            Text {text: "Players : "}
            Text {
                id: namePlayersDisplay
                wrapMode: TextInput.WrapAnywhere
                text: ""
            }
        }
        Row {
            id: r3
            topPadding: 10
            bottomPadding: 10
            anchors.top: r2.bottom
            Text {text: "_dareAppeared : "}
            Text {
                id: dareDisplay
                width: maximumWidth
                wrapMode: TextInput.WrapAnywhere
                text: ""
            }
        }
        Row {
            id: r4
            topPadding: 10
            bottomPadding: 10
            anchors.top: r3.bottom
            Text {text: "_truthAppeared : "}
            Text {
                id: truthDisplay
                width: maximumWidth
                wrapMode: TextInput.WrapAnywhere
                text: ""
            }
        }
    }
}
