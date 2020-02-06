#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include "struct.h"

#ifndef SERVER_FUNCTION_H
#define SERVER_FUNCTION_H

#define NUMBER_OF_USERS 9
#define NUMBER_OF_GROUPS 3

User user;
int receiverId;

void showOptions() {
    printf("Enter a number to do something: \n"
           "1 - SHOW LIST OF LOGGED USERS\n"
           "2 - SHOW LIST OF USERS IN GROUPS\n"
           "3 - SHOW LIST OF GROUPS\n"
           "4 - SIGN IN TO GROUP\n"
           "5 - SIGN OUT FROM GROUP\n"
           "6 - SEND A MESSAGE TO USER\n"
           "7 - SEND A MESSAGE TO GROUP\n"
           "8 - READ MESSAGES\n"
           "9 - LOGOUT\n");
}

LoginUserDetailsRequestModel inputLoginData() {

    LoginUserDetailsRequestModel userInputData;
    printf("Enter your login \n");
    scanf("%s", userInputData.login);
    printf("Enter your password \n");
    scanf("%s", userInputData.password);

    return userInputData;
}

void loginUserRequest(LoginUserDetailsRequestModel loginUserDetails) {


    int bridge = msgget(0x200, 0666);
    loginUserDetails.type = 2;
    msgsnd(bridge, &loginUserDetails, sizeof(LoginUserDetailsRequestModel) - sizeof(long), 0);
//    sleep(1);
    //User user;
    int received = msgrcv(bridge, &user, sizeof(user) - sizeof(long), 3, 0);
    if(received == -1) {
        perror("Error: ");
    }
    if(user.logStatus == 1) {
        printf("Login successful, welcome back %s \n", user.login);
        //individualKey = 0x200 + user.id;
    }
    else {
        printf("Login unsuccessful, validate input data \n");
    }
}

void showLoggedUsersRequest() {
    int bridge = msgget(0x200, 0);

    user.type = 4;

    msgsnd(bridge, &user, sizeof(user) - sizeof(long), 0);
    sleep(1);
    LoggedUsers loggedUsers;
    int received = msgrcv(bridge, &loggedUsers, sizeof(loggedUsers) - sizeof(long), 5, 0);
    if(received == -1) {
        perror("Error: ");
    }
    else {
        printf("%s", loggedUsers.login);
    }
}

void showListOfGroupsRequest() {
    int bridge = msgget(0x200, 0);
    user.type = 6;

    int send = msgsnd(bridge, &user, sizeof(user) - sizeof(long), 0);
    if(send == -1) {
        perror("Send Error: ");
    }
    sleep(1);
    ExistingGroups existingGroups;
    int received = msgrcv(bridge, &existingGroups, sizeof(existingGroups) - sizeof(long), 7, 0);
    if(received == -1) {
        perror("Error: ");
    }
    else {
        printf("%s", existingGroups.name);
    }
}

void showGroupInfo() {
    printf("Choose group you want to sign in\n"
           "1 - Sport\n"
           "2 - Politics\n"
           "3 - Business\n");
}

int chooseAction() {
    int chosenGroup;
    scanf("%d", &chosenGroup);
    return chosenGroup;
}

void signInToGroupRequest() {
    int choice = 0;
    while(choice != 1 && choice != 2 && choice != 3) {
        showGroupInfo();
        choice = chooseAction();
    }
    int bridge = msgget(0x200, 0);
    Group chosenGroup;
    chosenGroup.id = choice;
    user.type = 8;
    chosenGroup.type = 10;

    msgsnd(bridge, &user, sizeof(user) - sizeof(long), 0);
    msgsnd(bridge, &chosenGroup, sizeof(chosenGroup) - sizeof(long), 0);
    sleep(1);
    SignInToGroupStatus signInToGroupStatus;
    int received = msgrcv(bridge, &signInToGroupStatus, sizeof(signInToGroupStatus) - sizeof(long), 11, 0);
    if(received == -1) {
        perror("Error: ");
    }
    else {
        if(signInToGroupStatus.result) {
            printf("Success, now you are a member of chosen group\n");
        }
        else {
            printf("Failure, you have already been a member of chosen group\n");
        }
    }
}

void signOutFromGroupRequest() {
    int choice = 0;
    while(choice != 1 && choice != 2 && choice != 3) {
        showGroupInfo();
        choice = chooseAction();
    }
    int bridge = msgget(0x200, 0);
    Group chosenGroup;
    chosenGroup.id = choice;
    user.type = 12;
    chosenGroup.type = 14;

    msgsnd(bridge, &user, sizeof(user) - sizeof(long), 0);
    msgsnd(bridge, &chosenGroup, sizeof(chosenGroup) - sizeof(long), 0);
    sleep(1);
    SignOutFromGroupStatus signOutFromGroupStatus;
    int received = msgrcv(bridge, &signOutFromGroupStatus, sizeof(signOutFromGroupStatus) - sizeof(long), 15, 0);
    if(received == -1) {
        perror("Error: ");
    }
    else {
        if(signOutFromGroupStatus.result) {
            printf("Success, you signed out from chosen group\n");
        }
        else {
            printf("Failure, you have already been out of chosen group\n");
        }
    }
}

void showUsersInGroupRequest() {
    int bridge = msgget(0x200, 0);
    user.type = 16;
    msgsnd(bridge, &user, sizeof(user) - sizeof(long), 0);
    sleep(1);

    GroupMembers groupSportMembers;
    GroupMembers groupPoliticsMembers;
    GroupMembers groupBusinessMembers;

    int received = msgrcv(bridge, &groupSportMembers, sizeof(groupSportMembers) - sizeof(long), 17, 0);
    if(received == -1) {
        perror("Error: ");
    }
    msgrcv(bridge, &groupPoliticsMembers, sizeof(groupPoliticsMembers) - sizeof(long), 18, 0);
    msgrcv(bridge, &groupBusinessMembers, sizeof(groupBusinessMembers) - sizeof(long), 19, 0);

    printf("Users signed in to Sport group: \n");
    printf("%s", groupSportMembers.login);
    printf("Users signed in to Politics group: \n");
    printf("%s", groupPoliticsMembers.login);
    printf("Users signed in to Business group: \n");
    printf("%s", groupBusinessMembers.login);
}

void logoutUserRequest() {
    int bridge = msgget(0x200, 0);
    user.type = 20;
    msgsnd(bridge, &user, sizeof(user) - sizeof(long), 0);
    sleep(1);
    int received = msgrcv(bridge, &user, sizeof(user) - sizeof(long), 21, 0);
    if(received == -1) {
        perror("Error: ");
    }
    else {
        if(user.logStatus == 0) {
            printf("User logged out.\n");
            exit(0);
        }
        else {
            printf("Logout unsuccessful\n");
            exit(-1);
        }
    }
}

void writeMessageToUser() {
    int bridge = msgget(0x200, 0);
    int msgBridge = msgget(0x201, 0);
    user.type = 22;
    User receiver;
    receiver.type = 24;
    msgsnd(bridge, &user, sizeof(user) - sizeof(long), 0);

    UserFoundResponse userFoundResponse;
    userFoundResponse.isFound = 0;
    while (userFoundResponse.isFound <= 0) {
        printf("Type receiver's login: \n");
        scanf("%s", receiver.login);
        msgsnd(bridge, &receiver, sizeof(receiver) - sizeof(long), 0);
        int received = msgrcv(bridge, &userFoundResponse, sizeof(userFoundResponse) - sizeof(long), 25, 0);
        sleep(1);
        if(userFoundResponse.isFound == 1) {
            printf("Found\n");
            receiverId = userFoundResponse.id;
        }
        else if(userFoundResponse.isFound == 0) {
            printf("Writing messages to yourself is unavailable\n");
        }
        else {
            printf("Not found\n");
        }
    }
    if(userFoundResponse.isFound == 1) {
        Message message;
        message.type = 26;
        strcpy(message.senderLogin, user.login);
        message.receiverId = receiverId;
        printf("Type your message to %s, when you want to exit writing mode type \"quit\" and press enter \n", receiver.login);
        scanf("%[^\n]%*c", message.text);
        msgsnd(msgBridge, &message, sizeof(message) - sizeof(long), 0);
    }
}

void readMessageRequest() {
    int msgBridge = msgget(0x202 + user.id, 0);
    Message message;
    int receivedMessage = msgrcv(msgBridge, &message, sizeof(message) - sizeof(long), 27, IPC_NOWAIT);
    if(receivedMessage == -1) {
        //perror("Error: ");
    }
    else {
        printf("%s: %s\n", message.senderLogin, message.text);
    }
}

#endif