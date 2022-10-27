import QtQuick 2.15
import QtQuick.Window 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: appMain
    objectName: "appMain"
    visible: true
    title: qsTr("hi")
    color: "#282828"
    function startUpFunction(){
        //This is relevant for the Android code
        //that intercepts console log and this
        //indicates that qml has loaded and
        //opening webmessage channels can be started.
        console.log('started');
    }

    header: ToolBar {
        id: toolBar
        RowLayout {
            anchors.fill: parent
            ToolButton {
                id: menuButton
                text: "⋮"
                //font.family: "Courier"
                font.pointSize: 20
                onClicked: {
                    console.log("ToolButton");
                    menu.open();
                }
            }
            Label {
                text: "hi"
                //font.family: "Courier"
                font.pointSize: 20
                anchors.centerIn: parent
            }
        }
    }

    Menu {
        id: menu
//        x: menuButton.left
//        y: menuButton.bottom
        Action {
            text: "English US"
            checkable: true
            onToggled: {
                console.log("English US");
                let msg={
                    returnObj: "",
                    returnFunction: "",
                    sender: "menu",
                    service: "",
                    method: "POST",
                    params: {
                        action: "setSTTLanguage",
                        language: "en-US"
                    }
                };
                msgBoard.sendMessage(msg);
            }
        }
        Action {
            text: "English UK"
            checkable: true
            onToggled: {
                console.log("English UK");
                let msg={
                    returnObj: "",
                    returnFunction: "",
                    sender: "menu",
                    service: "",
                    method: "POST",
                    params: {
                        action: "setSTTLanguage",
                        language: "en-UK"
                    }
                };
                msgBoard.sendMessage(msg);
            }
        }
        Action {
            text: "Hungarian"
            checkable: true
            onToggled: {
                console.log("Hungarian");
                let msg={
                    returnObj: "",
                    returnFunction: "",
                    sender: "menu",
                    service: "",
                    method: "POST",
                    params: {
                        action: "setSTTLanguage",
                        language: "hu-HU"
                    }
                };
                msgBoard.sendMessage(msg);
            }
        }
    }

    ListModel {
        id: textItemModel
        objectName: "textItemModel"
    }

    ListView{
        id: itemList
        spacing: 5
        //anchors.fill: parent
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        flickableDirection: Flickable.VerticalFlick
        reuseItems: true
        model: textItemModel
        delegate: Rectangle {
                    id: item
                    width: itemList.width
                    height: row.height
                    color: "#282828"
                    //border.color: itemColour
                    //border.width: 1
                    Row {
                        id: row
                        width: parent.width//rowText.width
                        spacing: 0
                        anchors.centerIn: parent
                        Text {
                            id: rowText
                            width: parent.width
                            text: {text:textItem}
                            color: itemColour
                            //font.family: "Courier"
                            font.pointSize: 20
                            wrapMode: Text.Wrap
                        }
                    }
                }
        focus: true
        clip: true
        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400 }
            NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 400 }
        }
        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBounce }
        }

        MouseArea {
            id: mouseArea
            objectName: "mouseArea"
            anchors.fill: parent
            onClicked: {
                console.log("clicked");
                let msg={
                    returnObj: "mouseArea",
                    returnFunction: "msgIn",
                    sender: "mouseArea",
                    service: "",
                    method: "GET",
                    params: {
                        action: "clicked"
                    }
                };
                msgBoard.sendMessage(msg);
            }
            function msgIn(msg: string){
                console.log(msg);
                let imsg=JSON.parse(msg);
                if(imsg.type==="text"){
                    if(imsg.data.speaker==="hi"){
                        textItemModel.insert(0,{"textItem":imsg.data.text,"itemColour":"#FFB000","speaker":imsg.data.speaker});
                    }
                    else{
                        textItemModel.insert(0,{"textItem":imsg.data.text,"itemColour":"#33FF00","speaker":imsg.data.speaker});
                    }
                    itemList.currentIndex=0;
                }
                else if(imsg.type==="script"){
                    //console.log(imsg.data);
                    let result=Function(imsg.data)();//using global Function object constructor instead of eval()
                    //console.log("result:"+JSON.stringify(result));
                    msg={
                        returnObj: "mouseArea",
                        returnFunction: "msgIn",
                        sender: "mouseArea",
                        service: "",
                        method: "GET",
                        params: {
                            action: result.action,
                            actionParams: result.params
                        }
                    };
                    msgBoard.sendMessage(msg);
                }
            }
        }
    }

    Component.onCompleted: startUpFunction();
}
