// W przypadku wywołania z jednym argumentem uruchamiany jest (za pomocą popen()) program mail. Program użytkownika ma wypisywać listę e-maili posortowaną
//  alfabetycznie wg. adresów e-mail (argument nadawca) lub wg. daty otrzymania e-maili (argument data)
// Jeżeli program zostanie wywołany z trzema argumentami, to (za pomocą popen()) uruchamiany jest program mail i za jego pomocą wysyłany jest e-mail 
// do określonego nadawcy z określonym tematem i treścią
#include <stdio.h>
#include <stdlib.h>

void printMails(){
    //FILE* fp = popen("mail", "w");
}

int main(int argc, char **argv){
    switch(argc){
        case 2:
            break;
        case 4:
            char* receiver = argv[1];
            char* title = argv[2];
            char* body = argv[3];

            char command[512];

            sprintf(command, "mail -s \"%s\" %s", title, receiver);

            FILE* mail_input = popen(command, "w");

            if(mail_input == NULL){
                perror("Cannot create pipe!");
                exit(-1);
            }

            fputs(body, mail_input);

            if(pclose(mail_input) == -1){
                perror("Cannot close pipe!");
                exit(-1);
            }

            printf("mail sent to %s", receiver);
            break;
        default:
            return -1;
    }
}