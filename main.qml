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
        objectName: "menu"
//        x: menuButton.left
//        y: menuButton.bottom
        function setLanguage(msg: string){
            console.log(msg);
            let imsg=JSON.parse(msg);
            for(var i=0;i<menu.count;++i){
                var action=menu.actionAt(i);
                console.log(action.lid);
                if(action.lid===imsg.language){
                    action.checked=true;
                }
            }
        }
        Action {
            id: us
            text: "English US"
            property string lid: "en-US"
            checkable: true
            onToggled: {
                checked=true;
                for(var i=0;i<menu.count;++i){
                    var action=menu.actionAt(i);
                    console.log(action.text);
                    if(action.lid!=="en-US"){
                        action.checked=false;
                    }
                }
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
            id: uk
            text: "English UK"
            property string lid: "en-UK"
            checkable: true
            onToggled: {
                checked=true;
                for(var i=0;i<menu.count;++i){
                    var action=menu.actionAt(i);
                    console.log(action.text);
                    if(action.lid!=="en-UK"){
                        action.checked=false;
                    }
                }
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
            id: hu
            text: "Hungarian"
            property string lid: "hu-HU"
            checkable: true
            onToggled: {
                checked=true;
                for(var i=0;i<menu.count;++i){
                    var action=menu.actionAt(i);
                    console.log(action.text);
                    if(action.lid!=="hu-HU"){
                        action.checked=false;
                    }
                }
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
        //Add-scale and displaced animations are removed as two anims on adding an item broke the ui
        //after some time on android webview running wasm.
        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 250 }
            /*NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 250 }*/
        }
        /*displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 250; easing.type: Easing.OutBounce }
            //https://doc.qt.io/qt-5/qml-qtquick-viewtransition.html
            //Each newly added item undergoes an add transition, but before the transition can finish
            //the transition may be interrupted. Due to the interruption, the opacity and scale animations
            //are not completed, thus producing items with opacity and scale that are below 1.0.
            //To fix this, the displaced transition should additionally ensure the item properties are set
            //to the end values specified in the add transition, effectively resetting these values whenever
            //an item is displaced.
            NumberAnimation { property: "opacity"; to: 1.0; }
            NumberAnimation { property: "scale"; to: 1.0; }
        }*/

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
                    itemList.model=0;//Utter hack to force redraw by reassigning the model
                    itemList.model=textItemModel;
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
