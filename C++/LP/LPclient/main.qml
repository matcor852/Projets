import QtQuick.Window 2.15
import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.15

Window {

    // @disable-check M16
    width: 640
    // @disable-check M16
    height: 480
    // @disable-check M16
    visible: true
    // @disable-check M16
    title: qsTr("LP")

    property string waitRoomTitleString: "Le début de la fin...\n{%1}"
    property bool owner: false
    property string disclaim:
"- Avoir 18 ans
- Être en pleine possession de mes moyens et de ma capacité à prendre des décisions rationnelles
- Accepter tout eventuel risque lié au jeu
- Renoncer à faire valoir toute revendication, de quelque nature qu'elle soit (accidents, blessures, dégâts sur les biens personnels ou autres) auprès des personnes ayant contribué au développement du jeu
- Avoir connaissance de la disponibilité de contenus pour adultes et de consommation d'alcool et y consentir
- Ne pas harceler, abuser, insulter, nuire, diffamer, calomnier, dénigrer, intimider ou discriminer pendant et à la suite de ce jeu
- Ne pas utiliser ce jeu à des fins obscènes ou immorales\n\n\n"

    function clearEntry() {
        owner = false;
        playersModel.clear();
        partyNumber.text = "";
        playerSelectedNameDisplay.text = "";
        playerSelectedGageDisplay.text = "";
        nextRoundButton.visible = false
        joinInfos.visible = false
    }


    Connections {
        target: lpClient
        function onAppendInfos(infos) {displayInfos.text += infos+"<br/>";displayInfos.cursorPosition = displayInfos.length}
        function onSwiper(nb) {
            view.setCurrentIndex(nb)
        }
        function onAuthented() {
            clearEntry();
            view.setCurrentIndex(2)
        }
        function onIntegrated(nbParty) {
            partyNumber.text = "";
            waitingRoomTitle.text = waitRoomTitleString.arg(nbParty);
            view.setCurrentIndex(3);
        }
        function onRefreshPlayers(imageData64,playersList,removeOrder) {
            if(removeOrder === true)
            {
                playersModel.clear();
            }
            playersModel.append({imgData: imageData64, playerName: playersList});
            nameDisplayerViewer.positionViewAtEnd();
        }
        function onNewAction(imageData64,player,action) {
            playerSelectedIconDisplay.source = (imageData64 !== "")?"data:;base64,"+imageData64:""
            playerSelectedNameDisplay.text = player;
            playerSelectedGageDisplay.text = action;
        }
        function onInheritedParty(owned) {
            owner = owned;
            nextRoundButton.visible = true
        }
    }

    ScrollView {
        anchors.top: view.bottom
        clip: true
        topPadding: parent.height/150
        width: parent.width
        height: parent.height*(1/4)
        background : Rectangle{
            anchors.fill: parent
            color: "black"
        }
        TextArea {
            id: displayInfos
            enabled: false
            text: qsTr("")
            textFormat: Text.RichText
            color: "white"
            font.pixelSize: 12
            wrapMode: Text.WordWrap
        }
    }

    SwipeView {
        id: view
        currentIndex: 0
        width: parent.width
        height: parent.height*(3/4)
        interactive: false

        Item {
            id: disclaimer
            height: view.height
            width: view.width
            Rectangle{
                anchors.fill: parent
                anchors.centerIn: parent
                color: "#333333"
                Text {
                    id: termsOfUseTitle
                    topPadding: parent.height/15
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Conditions d'utilisation\n")
                    font.pixelSize: 25
                    color: "white"
                }
                Text {
                    id: termsOfUsePrimary
                    anchors.top: termsOfUseTitle.bottom
                    leftPadding: width/20
                    rightPadding: width/11
                    text: qsTr("En jouant à ce jeu, je certifie et m'engage à :\n")
                    width: parent.width
                    font.pixelSize: 12
                    wrapMode: Text.WordWrap
                    color: "white"
                }
                Text {
                    id: termsOfUseContent
                    leftPadding: width/9
                    rightPadding: width/11
                    anchors.top: termsOfUsePrimary.bottom
                    text: qsTr(disclaim)
                    width: parent.width
                    font.pixelSize: 12
                    wrapMode: Text.WordWrap
                    color: "white"
                }
                Button {
                    id: confirmTermsOfUse
                    text: qsTr("Confirmer")
                    anchors.rightMargin: view.width/10
                    anchors.top: termsOfUseContent.bottom
                    anchors.right: parent.right
                    background: Rectangle {
                                anchors.fill: parent
                                color: confirmTermsOfUse.down ? "green" : "darkgrey"
                                border.color: "#26282a"
                                border.width: 1
                                radius: 10
                            }
                    onClicked: {
                        view.setCurrentIndex(1)
                    }
                }
                Button {
                    id: declineTermsOfUse
                    text: qsTr("Quitter")
                    anchors.top: termsOfUseContent.bottom
                    anchors.left: parent.left
                    background: Rectangle {
                                anchors.fill: parent
                                color: declineTermsOfUse.down ? "red" : "darkgrey"
                                border.color: "#26282a"
                                border.width: 1
                                radius: 10
                            }
                    onClicked: Qt.callLater(Qt.quit)
                }
            }
        }
        Item {
            id: accueil
            height: view.height
            width: view.width
            Rectangle{
                anchors.fill: parent
                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "darkred"
                    }

                    GradientStop {
                        position: 1
                        color: "#000000"
                    }
                }

                Button {
                    id: aboutLP
                    text: "<font color='red'><i>about</i></font>"
                    font.pixelSize: 10
                    width: (1/5)*view.width
                    background: Rectangle {
                        anchors.fill: parent
                        color: aboutLP.down? "black":"transparent"
                        border.width: 2
                        border.color: "red"
                        radius: 50
                    }
                    onClicked: aboutPopup.open()
                    Popup {
                        id: aboutPopup
                        anchors.centerIn: Overlay.overlay
                        width: (5/6)*view.width
                        modal: true
                        focus: true
                        background: Rectangle {
                            anchors.fill: parent
                            color: "red"
                            border.width: 0
                        }
                        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
                        contentItem: Text {
                            textFormat: Text.RichText
                            onLinkActivated: Qt.openUrlExternally(link)
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            text: "LP version 1.0<br><br>"+
                                  "Programme créé et compilé depuis Qt 5.15.2<br>"+
                                  "Ce programme est sous licence GNU General Public License Version 3,<br>"+
                                  "voir : <a href='https://www.gnu.org/licenses/gpl-3.0.html'>https://www.gnu.org/licenses/gpl-3.0.html</a> pour plus de détails.<br><br>"+
                                  "Ce logiciel utilise le GPL/LGPL Toolkit fourni par <a href='https://www.qt.io/developers'>https://www.qt.io/developers</a><br>"+
                                  "Voir : <a href='https://doc.qt.io/qt-5/licensing.html'>https://doc.qt.io/qt-5/licensing.html</a> pour les conditions de licence et toute autre information."

                        }
                    }
                }

                Text {
                    id: accueilTitle
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    topPadding: parent.height/15
                    bottomPadding: parent.height/50
                    text: qsTr("Les Problèmes")
                    font.pixelSize: 30
                    font.italic: true; font.bold: true;
                    enabled: false
                    color: "red"
                }
                Image {
                    id: accueilPict
                    anchors.top: accueilTitle.bottom
                    anchors.bottom: centerInfos.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    asynchronous: true
                    fillMode: Image.PreserveAspectFit
                    sourceSize.width: 1024
                    sourceSize.height: 1024
                    source: ""
                    MouseArea {
                        anchors.fill: parent
                        onClicked: iconFileDialog.open();
                    }
                }

                Column {
                    id: centerInfos
                    topPadding: parent.height/50
                    anchors.centerIn: parent
                    spacing: 10
                    GridLayout {
                        columns: 2
                        rows: 2
                        Label {text: "Pseudo : "; font.pixelSize: 15; font.bold: true; enabled: false; color: "red"}
                        TextField {id: pseudo; selectByMouse: true; maximumLength: 15; font.pixelSize: 15; font.italic: true;
                        color: "white"; placeholderText: "Enter Username";}
                        Label {text: "Password : "; font.pixelSize: 15; font.bold: true; enabled: false; color: "red"}
                        TextField {id: password; echoMode: TextInput.Password; passwordMaskDelay: 3000;
                        selectByMouse: true; maximumLength: 32; font.pixelSize: 15; font.italic: true; color: "white"; placeholderText: "Enter Password";}
                    }
                    Button {
                        id: login
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("<font color='red'> Connexion </font>")
                        font.family: "Arial"
                        font.pixelSize: 18
                        width: parent.width
                        background: Rectangle {
                            anchors.fill: parent
                            color: login.down ? "black":"transparent"
                            border.color: "red"
                            border.width: 3
                            radius: 10
                        }
                        onClicked:
                        {
                            if(accueilPict.source == "")
                            {
                                iconFileDialog.open();
                            }
                            else {lpClient.login(pseudo.text, password.text, accueilPict.source)}
                        }
                    }
                    FileDialog {
                        id: iconFileDialog
                        title: "choose your icon";
                        folder: shortcuts.pictures
                        modality: Qt.WindowModal
                        selectExisting: true
                        selectMultiple: false
                        nameFilters: ["Image files (*.jpg *.jpeg *.png *.heif *.bmp *.icns)"]
                        onAccepted: {
                            accueilPict.source = Qt.resolvedUrl(iconFileDialog.fileUrl);
                            iconFileDialog.close();
                            lpClient.login(pseudo.text, password.text, iconFileDialog.fileUrl);
                        }
                        onRejected: {iconFileDialog.close()}
                    }
                    Button {
                        id: registration
                        text: "<font color='darkred'><i><u>Inscription</u></i></font>"
                        font.pixelSize: 15
                        width: login.width
                        background: Rectangle {
                            anchors.fill: parent
                            color: registration.down? "black":"transparent"
                            border.width: 0
                        }
                        onClicked: registrationPopup.open()
                        Popup {
                            id: registrationPopup
                            anchors.centerIn: Overlay.overlay
                            //width: (4/6)*view.width
                            modal: true
                            focus: true
                            background: Rectangle {
                                anchors.fill: parent
                                color: "darkred"
                                border.width: 0
                            }
                            contentItem: Column {
                                anchors.centerIn: parent
                                spacing: 10
                                GridLayout {
                                    columns: 2
                                    rows: 6
                                    Label {text: "*Nom : "; color: "red"}
                                    TextField {id: nom; Layout.fillWidth: true; validator: RegExpValidator {regExp: /[^\W\d_]+/}
                                    maximumLength: 32; color: "white"}
                                    Label {text: "*Prénom : "; color: "red"}
                                    TextField {id: prenom; Layout.fillWidth: true; validator: RegExpValidator {regExp: /[^\W\d_]+/}
                                    maximumLength: 32; color: "white"}
                                    Label {text: "*Âge : "; color: "red"}
                                    TextField {id: age; Layout.fillWidth: true; validator: RegExpValidator {regExp: /[0-9]+/}
                                    maximumLength: 2; placeholderText: "18 ans minimum"; onTextChanged: {if(parseInt(age.text) < 18){age.color = "red"}else{age.color = "white"}}}
                                    Label {text: "*pseudo : "; color: "red"}
                                    TextField {id: pseudoR; Layout.fillWidth: true; validator: RegExpValidator {regExp: /[^\W\d_]+/}
                                    maximumLength: 15; placeholderText: "3-15 caractères"; onTextChanged: {if(pseudoR.text.length < 3 || pseudoR.text.length > 18){pseudoR.color = "red"}else{pseudoR.color = "white"}}}
                                    Label {text: "*Sexe : "; color: "red"}
                                    ComboBox {id: genre; model: ["Homme", "Femme"]; Layout.fillWidth: true;}
                                    Label {text: "*mot de passe : "; color: "red"}
                                    TextField {id: passwordR; Layout.fillWidth: true; echoMode: TextInput.Password; passwordMaskDelay: 3000;
                                    selectByMouse: true; maximumLength: 32; placeholderText: "6-32 caractères"; onTextChanged: {if(passwordR.text.length < 6 || passwordR.text.length > 32){passwordR.color = "red"}else{passwordR.color = "white"}}}
                                }
                                Button {
                                    id: confirmRegistration
                                    text: qsTr("<font color='red'> S'inscrire </font>")
                                    font.bold: true;
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    background: Rectangle {
                                        anchors.fill: parent
                                        color: confirmRegistration.down? "black":"transparent"
                                        border.width: 2
                                        border.color: "red"
                                        radius: 10
                                    }
                                    onClicked: {
                                        if(nom.length && prenom.length && age.length && pseudoR.length && passwordR.length)
                                        {
                                            lpClient.askAccountCreation(nom.text, prenom.text, parseInt(age.text), pseudoR.text, genre.currentText, passwordR.text)
                                            registrationPopup.close()
                                            nom.clear()
                                            prenom.clear()
                                            age.clear()
                                            pseudoR.clear()
                                            passwordR.clear()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        Item {
            id: roomChoice
            height: view.height
            width: view.width
            Rectangle{
                anchors.fill: parent
                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "darkred"
                    }

                    GradientStop {
                        position: 1
                        color: "#000000"
                    }
                }
                Button {
                    id: quit1
                    text: qsTr("<font color='red'> Quit </font>")
                    font.family: "Arial"
                    font.pixelSize: 10
                    width: (1/5)*view.width
                    background: Rectangle {
                        anchors.fill: parent
                        color: quit1.down ? "black":"transparent"
                        border.color: "red"
                        border.width: 3
                        radius: 20
                    }
                    onClicked: {
                        clearEntry();
                        lpClient.askQuit();
                    }
                }
                Column {
                    anchors.centerIn: parent
                    spacing: 20
                    Row {
                        Button {
                            id: joiningButton
                            text: qsTr("<font color='red'> Rejoindre une partie </font>")
                            font.family: "Arial"
                            width: view.width/2
                            background: Rectangle {
                                anchors.fill: parent
                                color: joiningButton.down ? "black":"transparent"
                                border.color: "red"
                                border.width: 3
                            }
                            onClicked: {
                                if(joinInfos.visible != true)
                                {
                                    joinInfos.visible = true
                                }
                            }
                        }
                        Button {
                            id: createButton
                            text: qsTr("<font color='red'> Créer une partie </font>")
                            font.family: "Arial"
                            width: view.width/2

                            background: Rectangle {
                                anchors.fill: parent
                                color: createButton.down ? "black":"transparent"
                                border.color: "red"
                                border.width: 3
                            }
                            onClicked: {
                                if(joinInfos.visible != false)
                                {
                                    joinInfos.visible = false
                                }
                                owner = true;
                                nextRoundButton.visible = true;
                                lpClient.askCreate();
                            }
                        }
                    }
                    Column {
                        id: joinInfos
                        visible: false
                        spacing: 20
                        Text {
                            text: qsTr("<font color='red'> Code : </font>")
                        }
                        TextInput {
                            id: partyNumber
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                            text: qsTr("")
                            color: "white"
                            validator: RegExpValidator {regExp: /[0-9]+/}
                            font.pixelSize: 15; font.bold: true;
                            maximumLength: 4
                        }
                        Button {
                            id: joinButton
                            text: qsTr("<font color='red'> Rejoindre </font>")
                            font.family: "Arial"
                            width: view.width/2
                            background: Rectangle {
                                anchors.fill: parent
                                color: joinButton.down ? "black":"transparent"
                                border.color: "red"
                                border.width: 3
                            }
                            onClicked: {
                                owner = false;
                                nextRoundButton.visible = false
                                lpClient.askJoin(partyNumber.text)
                            }
                        }
                    }
                }
            }
        }
        Item {
            id: roomJoin
            height: view.height
            width: view.width
            Rectangle{
                anchors.fill: parent
                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "darkred"
                    }
                    GradientStop {
                        position: 1
                        color: "black"
                    }
                }
                Button {
                    id: quit2
                    text: qsTr("<font color='red'> Quit </font>")
                    font.family: "Arial"
                    font.pixelSize: 10
                    width: (1/5)*view.width
                    background: Rectangle {
                        anchors.fill: parent
                        color: quit2.down ? "black":"transparent"
                        border.color: "red"
                        border.width: 3
                        radius: 20
                    }
                    onClicked: {
                        clearEntry();
                        lpClient.askQuit()
                    }
                }
                Text {
                    id: waitingRoomTitle
                    anchors.top: quit2.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Le début de la fin...\n{....}")
                    font.pixelSize: 30
                    font.italic: true; font.bold: true;
                    color: "red"
                }
                ScrollView {
                    id: scrollPlayerList
                    anchors.top: waitingRoomTitle.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    rightPadding: (1/5)*parent.width
                    topPadding: parent.height/25
                    width: parent.width/2
                    height: (3/4)*parent.height
                    ListView {
                        id: nameDisplayerViewer
                        clip: true                  //////////////
                        model: ListModel {
                            id: playersModel
                            ListElement {imgData: ""; playerName: ""}
                        }
                        delegate: RowLayout {
                            Image {
                                source: (imgData != "")?"data:;base64,"+imgData:""
                                asynchronous: true
                                fillMode: Image.PreserveAspectFit
                                sourceSize.width: 40
                                sourceSize.height: 40
                            }
                            Text {
                                text: playerName
                                font.pixelSize: 15
                                color: "red"
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                }
            }
        }
        Item {
            id: roomParty
            height: view.height
            width: view.width
            Rectangle{
                anchors.fill: parent
                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "darkred"
                    }

                    GradientStop {
                        position: 1
                        color: "#000000"
                    }
                }
                Button {
                    id: quit3
                    text: qsTr("<font color='red'> Quit </font>")
                    font.family: "Arial"
                    font.pixelSize: 10
                    width: (1/5)*view.width
                    background: Rectangle {
                        anchors.fill: parent
                        color: quit3.down ? "black":"transparent"
                        border.color: "red"
                        border.width: 3
                        radius: 20
                    }
                    onClicked: {
                        clearEntry();
                        lpClient.askQuit();
                    }
                }
                Button {
                    id: toggleToWaitRooom
                    text: "→    ←"

                    anchors.horizontalCenter: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height * 2/5
                    font.pixelSize: 25
                    Material.foreground: toggleToWaitRooom.down ? "red":"black"
                    background: Rectangle {
                        anchors.fill: parent
                        color: toggleToWaitRooom.down ? "black":"red"
                        radius: 30
                    }
                    onClicked: {
                        if(view.currentIndex === 4)
                            view.setCurrentIndex(3);
                        else
                            view.setCurrentIndex(4)
                    }
                }
                RowLayout {
                    id: playerSelectedInfos
                    anchors.horizontalCenter: parent.horizontalCenter
                    Image {
                        id: playerSelectedIconDisplay
                        source: ""
                        asynchronous: true
                        fillMode: Image.PreserveAspectFit
                        sourceSize.width: 40
                        sourceSize.height: 40
                    }
                    Text {
                        id: playerSelectedNameDisplay
                        text: ""
                        font.pixelSize: 30
                        font.bold: true;
                        color: "red"
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
                Text {
                    id: playerSelectedGageDisplay
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: playerSelectedInfos.bottom
                    topPadding: parent.height/20
                    width: parent.width/1.5
                    text: qsTr("")
                    wrapMode: Text.WordWrap
                    textFormat: Text.RichText
                    font.pixelSize: 20
                    font.italic: true;
                    color: "red"
                }
                Button {
                    id: nextRoundButton
                    text: qsTr("<font color='red'> Continuer </font>")
                    font.family: "Arial"
                    width: view.width/3
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    background: Rectangle {
                        anchors.fill: parent
                        color: nextRoundButton.down ? "darkred":"transparent"
                        border.color: "red"
                        border.width: 3
                        radius: 10
                    }
                    onClicked: {lpClient.askNext()}
                    visible: owner ? true:false
                }
            }
        }
    }
}
