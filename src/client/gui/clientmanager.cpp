#include "clientmanager.h"
#include "defaultwidget.h"

const key_type ClientManager::LOGIN = 0;
const key_type ClientManager::CREATEACCOUNT = 1;
const key_type ClientManager::MAINMENU = 2;
const key_type ClientManager::INPLAY = 3;
const key_type ClientManager::CREATEGAME = 4;
const key_type ClientManager::JOINGAME = 5;

ClientManager::ClientManager(char* hostname, int port) :
    stackedWidget(new QStackedWidget), layout(new QVBoxLayout), pages(), \
    socket(hostname, port), message(){

    socket.connectHost();
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(stackedWidget);

}

ClientManager::~ClientManager(){
    // FIXME SEGMENTION FAULT
    //delete layout;
    //delete stackedWidget;
    /*for(std::map<int, QWidget*>::iterator iterator = pages.begin(); iterator != pages.end(); iterator++) {
        std::cout<<"first = "<<iterator->first<<std::endl;
        std::cout<<"second = "<<iterator->second<<std::endl;
        //delete iterator->second;
    }*/
}


void ClientManager::set(key_type key, DefaultWidget* widget){
    if(!alreadySet(key)){
        pages[key] = widget;
        stackedWidget->addWidget(widget);
    }else{
        throw std::invalid_argument("This key is already set.");
    }
}

DefaultWidget* ClientManager::get(key_type key){
    if(alreadySet(key)){
        return pages[key];
    }else{
        return nullptr;
    }
}

bool ClientManager::alreadySet(key_type key){
    auto search = pages.find(key);
    return search != pages.end();
}

void ClientManager::setCurrentWidget(key_type key){
    get(key)->refresh();
    stackedWidget->setCurrentWidget(get(key));
}

QVBoxLayout* ClientManager::getLayout(){
    return layout;
}

void ClientManager::setRequest(std::string request){
    cleanMessage();
    message.setTopic(request);
}

void ClientManager::addInfo(std::string key, std::string value){
    message.set(key, value);
}

void ClientManager::cleanMessage(){
    message = SocketMessage();
}

void ClientManager::sendRequestAndRecv(){
    sendRequest();
    recvAnswer();
}

void ClientManager::sendRequest(){
    socket.write(message.toString());
}

void ClientManager::recvAnswer(){
    message = SocketMessage::parse(socket.read());
}

bool ClientManager::requestFailed(){
    return message.getTopic() == "failure";
}

std::string ClientManager::getFailureReason(){
    return message.get("reason");
}

void ClientManager::quit(){
    setRequest("quit");
    sendRequest();
}